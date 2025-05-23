// ProgressiveLOD.cpp
#include "ProgressiveLOD.h"
#include "Mesh.h"  // 显式包含 Mesh 的完整定义
#include <glm/gtx/norm.hpp>
#include <algorithm>

ProgressiveLOD::ProgressiveLOD(Mesh& mesh)
    : target_mesh(mesh),
      current_vertex_count(mesh.GetVertices().size()),
      vertex_validity(mesh.GetVertices().size(), true) {}

void ProgressiveLOD::Precompute() {
    if (is_precomputed) return;

    original_vertices = target_mesh.GetVertices();
    original_indices = target_mesh.GetIndices();

    std::unordered_map<uint64_t, EdgeCollapse> edge_map;
    for (size_t i = 0; i < original_indices.size(); i += 3) {
        const auto* tri = &original_indices[i];
        for (int j = 0; j < 3; ++j) {
            size_t a = tri[j];
            size_t b = tri[(j+1)%3];
            if (a > b) std::swap(a, b);
            uint64_t edge_key = (static_cast<uint64_t>(a) << 32) | b;

            if (edge_map.find(edge_key) == edge_map.end()) {
                EdgeCollapse collapse;
                collapse.v1 = a;
                collapse.v2 = b;
                collapse.collapse_cost = ComputeCollapseCost(a, b);
                collapse.affected_triangles.push_back(i / 3);
                edge_map[edge_key] = collapse;
            } else {
                edge_map[edge_key].affected_triangles.push_back(i / 3);
            }
        }
    }

    for (auto& pair : edge_map) {
        collapse_candidates.push_back(pair.second);
    }
    RebuildPriorityQueue();
    is_precomputed = true;
}

float ProgressiveLOD::ComputeCollapseCost(size_t v1_index, size_t v2_index) const {
    const Vertex& v1 = original_vertices[v1_index];
    const Vertex& v2 = original_vertices[v2_index];

    float geom_error = active_params.geometry_weight * glm::distance(v1.Position, v2.Position);
    float normal_error = active_params.normal_weight * (1.0f - glm::dot(v1.Normal, v2.Normal));
    float uv_error = active_params.uv_weight * glm::distance(v1.TexCoords, v2.TexCoords);

    return geom_error + normal_error + uv_error;
}

void ProgressiveLOD::ApplyEdgeCollapse(const EdgeCollapse& collapse) {
    auto& indices = target_mesh.GetIndices();
    vertex_validity[collapse.v2] = false;

    for (auto& index : indices) {
        if (index == collapse.v2) index = collapse.v1;
    }

    auto& vertices = target_mesh.GetVertices();
    Vertex& merged_vertex = vertices[collapse.v1];
    const Vertex& v2_vertex = original_vertices[collapse.v2];

    merged_vertex.Position = (merged_vertex.Position + v2_vertex.Position) * 0.5f;
    merged_vertex.Normal = glm::normalize(merged_vertex.Normal + v2_vertex.Normal);
    merged_vertex.TexCoords = (merged_vertex.TexCoords + v2_vertex.TexCoords) * 0.5f;

    current_vertex_count--;
    current_error = collapse.collapse_cost;
}

void ProgressiveLOD::RemoveDegenerateTriangles() {
    auto& indices = target_mesh.GetIndices();

    std::vector<uint32_t> new_indices;
    for (size_t i = 0; i + 2 < indices.size(); i += 3) {
        uint32_t a = indices[i];
        uint32_t b = indices[i + 1];
        uint32_t c = indices[i + 2];

        if (!vertex_validity[a] || !vertex_validity[b] || !vertex_validity[c]) continue;
        if (a == b || b == c || c == a) continue;

        new_indices.push_back(a);
        new_indices.push_back(b);
        new_indices.push_back(c);
    }

    indices = std::move(new_indices);
}

void ProgressiveLOD::SimplifyTo(float ratio) {
    if (!is_precomputed) Precompute();

    ratio = glm::clamp(ratio, 0.0f, 1.0f);
    const size_t target_count = static_cast<size_t>(original_vertices.size() * (1.0f - ratio));

    int collapses_remaining = active_params.max_collapses_per_frame;
    while (current_vertex_count > target_count && !collapse_queue.empty() && collapses_remaining-- > 0) {
        const auto [cost, index] = collapse_queue.top();
        collapse_queue.pop();

        const EdgeCollapse& collapse = collapse_candidates[index];
        if (vertex_validity[collapse.v1] && vertex_validity[collapse.v2]) {
            ApplyEdgeCollapse(collapse);
        }
    }

    RemoveDegenerateTriangles();
    target_mesh.UpdateGPUData();
}

void ProgressiveLOD::UpdateParameters(const Parameters& new_params) {
    if (is_precomputed && active_params.preserve_topology != new_params.preserve_topology) {
        throw std::runtime_error("Cannot change topology preservation after precompute");
    }

    active_params = new_params;

    if (is_precomputed) {
        for (auto& collapse : collapse_candidates) {
            collapse.collapse_cost = ComputeCollapseCost(collapse.v1, collapse.v2);
        }
        RebuildPriorityQueue();
    }
}

void ProgressiveLOD::RebuildPriorityQueue() {
    while (!collapse_queue.empty()) collapse_queue.pop();

    for (size_t i = 0; i < collapse_candidates.size(); ++i) {
        collapse_queue.emplace(-collapse_candidates[i].collapse_cost, i);
    }
}