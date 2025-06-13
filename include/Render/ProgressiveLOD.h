#pragma once
#include "Vertex.h"
#include <queue>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <glm/glm.hpp>

class Mesh; // 前向声明

class ProgressiveLOD {
public:
    struct Parameters {
        float geometry_weight = 0.7f;
        float normal_weight   = 0.2f;
        float uv_weight       = 0.1f;
        bool  preserve_topology     = true;
        int   max_collapses_per_frame = 50;
    };

    explicit ProgressiveLOD(Mesh& mesh);

    /// 预先计算所有边的 collapse 候选和优先队列
    void Precompute();
    /// 按给定 ratio（[0,1]）进行可逆简化
    void SimplifyTo(float ratio);
    /// 更新参数（geometry/normal/uv 权重、拓扑保持、每帧最大折叠数）
    void UpdateParameters(const Parameters& new_params);

    size_t GetCurrentVertices() const { return current_vertex_count; }
    float  GetCurrentError()    const { return current_error; }
    const Parameters& GetParameters() const { return active_params; }

private:
    // 一条边的 collapse 候选
    struct EdgeCollapse {
        size_t v1, v2;
        float  collapse_cost;
        std::vector<size_t> affected_triangles;
    };

    // —— 内部方法 —— 
    float ComputeCollapseCost(size_t v1, size_t v2) const;
    void  ApplyEdgeCollapse(const EdgeCollapse& collapse);
    void  RemoveDegenerateTriangles();
    void  RebuildPriorityQueue();
    void  ResetToOriginal();

    // 原始网格数据快照
    std::vector<Vertex>   original_vertices;
    std::vector<uint32_t> original_indices;

    // 当前有效性与候选列表
    std::vector<bool>           vertex_validity;
    std::vector<EdgeCollapse>   collapse_candidates;
    std::priority_queue<std::pair<float, size_t>> collapse_queue;

    Mesh&              target_mesh;
    Parameters         active_params;
    size_t             current_vertex_count;
    float              current_error   = 0.0f;
    bool               is_precomputed  = false;

    // 用于可逆控制
    float last_ratio = 0.0f;
};
