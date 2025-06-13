#include "ProgressiveLOD.h"
#include "Mesh.h"
#include <glm/gtx/norm.hpp>
#include <glm/gtc/constants.hpp>
#include <algorithm>

ProgressiveLOD::ProgressiveLOD(Mesh& mesh)
    : target_mesh(mesh),
      current_vertex_count(mesh.GetVertices().size()),
      vertex_validity(mesh.GetVertices().size(), true)
{ }

void ProgressiveLOD::Precompute() {
    if (is_precomputed) return;

    // 拷贝原始顶点/索引
    original_vertices = target_mesh.GetVertices();
    original_indices  = target_mesh.GetIndices();

    // 构建边到 EdgeCollapse 的映射
    std::unordered_map<uint64_t, EdgeCollapse> edge_map;
    for (size_t i = 0; i + 2 < original_indices.size(); i += 3) {
        uint32_t ia = original_indices[i],
                 ib = original_indices[i+1],
                 ic = original_indices[i+2];
        uint32_t tri[3] = { ia, ib, ic };
        for (int e = 0; e < 3; ++e) {
            size_t a = tri[e], b = tri[(e+1)%3];
            if (a > b) std::swap(a, b);
            uint64_t key = (uint64_t(a) << 32) | b;
            auto it = edge_map.find(key);
            if (it == edge_map.end()) {
                EdgeCollapse ec;
                ec.v1 = a; ec.v2 = b;
                ec.collapse_cost = ComputeCollapseCost(a, b);
                ec.affected_triangles.push_back(i / 3);
                edge_map[key] = std::move(ec);
            } else {
                it->second.affected_triangles.push_back(i / 3);
            }
        }
    }

    // 转移到候选列表
    collapse_candidates.reserve(edge_map.size());
    for (auto& kv : edge_map) {
        collapse_candidates.push_back(std::move(kv.second));
    }
    RebuildPriorityQueue();
    is_precomputed = true;
}

float ProgressiveLOD::ComputeCollapseCost(size_t v1, size_t v2) const {
    const Vertex& A = original_vertices[v1];
    const Vertex& B = original_vertices[v2];

    float geom  = active_params.geometry_weight * glm::distance(A.Position, B.Position);
    float norm  = active_params.normal_weight   * (1.0f - glm::dot(A.Normal, B.Normal));
    float uvErr = active_params.uv_weight       * glm::distance(A.TexCoords, B.TexCoords);
    return geom + norm + uvErr;
}

void ProgressiveLOD::ApplyEdgeCollapse(const EdgeCollapse& col) {
    auto& V = target_mesh.GetVertices();
    auto& I = target_mesh.GetIndices();
    // 标记 v2 无效
    vertex_validity[col.v2] = false;
    // 把所有 index==v2 的换成 v1
    for (auto& idx : I) {
        if (idx == col.v2) idx = uint32_t(col.v1);
    }
    // 合并顶点属性到 v1
    Vertex& dst = V[col.v1];
    const Vertex& src = original_vertices[col.v2];
    dst.Position  = (dst.Position  + src.Position)  * 0.5f;
    dst.Normal    = glm::normalize(dst.Normal + src.Normal);
    dst.TexCoords = (dst.TexCoords + src.TexCoords) * 0.5f;

    --current_vertex_count;
    current_error = col.collapse_cost;
}

void ProgressiveLOD::RemoveDegenerateTriangles() {
    auto& I = target_mesh.GetIndices();
    std::vector<uint32_t> clean;
    clean.reserve(I.size());
    for (size_t i = 0; i + 2 < I.size(); i += 3) {
        uint32_t a = I[i], b = I[i+1], c = I[i+2];
        if (!vertex_validity[a] || !vertex_validity[b] || !vertex_validity[c]) continue;
        if (a==b || b==c || c==a) continue;
        clean.push_back(a);
        clean.push_back(b);
        clean.push_back(c);
    }
    I.swap(clean);
}

void ProgressiveLOD::RebuildPriorityQueue() {
    while (!collapse_queue.empty()) collapse_queue.pop();
    for (size_t i = 0; i < collapse_candidates.size(); ++i) {
        // 存负值，使 top() 最小 cost
        collapse_queue.emplace(-collapse_candidates[i].collapse_cost, i);
    }
}

void ProgressiveLOD::ResetToOriginal() {
    // 恢复到原始快照
    target_mesh.GetVertices() = original_vertices;
    target_mesh.GetIndices()  = original_indices;
    // 恢复顶点有效性与计数
    vertex_validity.assign(original_vertices.size(), true);
    current_vertex_count = original_vertices.size();
    current_error = 0.0f;
    // 重建优先队列
    RebuildPriorityQueue();
}

void ProgressiveLOD::SimplifyTo(float ratio) {
    if (!is_precomputed) Precompute();

    ratio = glm::clamp(ratio, 0.0f, 1.0f);

    // 如果滑条向左（ratio 减小），恢复到原始再折叠
    if (ratio < last_ratio) {
        ResetToOriginal();
    }
    last_ratio = ratio;

    // 计算目标顶点数
    const size_t target_count =
        static_cast<size_t>(original_vertices.size() * (1.0f - ratio));

    int moves = active_params.max_collapses_per_frame;
    // 若已经到达目标，跳过；否则持续折叠
    while (current_vertex_count > target_count &&
           !collapse_queue.empty() &&
           moves-- > 0) {
        auto [negCost, idx] = collapse_queue.top();
        collapse_queue.pop();
        const auto& col = collapse_candidates[idx];
        if (vertex_validity[col.v1] && vertex_validity[col.v2]) {
            ApplyEdgeCollapse(col);
        }
    }

    RemoveDegenerateTriangles();
    target_mesh.UpdateGPUData();
}

void ProgressiveLOD::UpdateParameters(const Parameters& new_params) {
    if (is_precomputed &&
        active_params.preserve_topology != new_params.preserve_topology) {
        throw std::runtime_error(
            "Cannot change preserve_topology after precompute");
    }
    active_params = new_params;
    if (is_precomputed) {
        // 重新计算各边代价，并重建队列
        for (auto& col : collapse_candidates) {
            col.collapse_cost =
                ComputeCollapseCost(col.v1, col.v2);
        }
        RebuildPriorityQueue();
    }
}
