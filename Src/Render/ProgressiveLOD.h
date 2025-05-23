// ProgressiveLOD.h
#pragma once
#include "Vertex.h"
#include <queue>
#include <vector>
#include <unordered_map>
#include <stdexcept>

class Mesh; // 前向声明，避免循环依赖

class ProgressiveLOD {
public:
    struct Parameters {
        float geometry_weight = 0.7f;
        float normal_weight = 0.2f;
        float uv_weight = 0.1f;
        bool preserve_topology = true;
        int max_collapses_per_frame = 50;
    };

    explicit ProgressiveLOD(Mesh& mesh);
    void Precompute();
    void SimplifyTo(float ratio);
    void UpdateParameters(const Parameters& new_params);

    size_t GetCurrentVertices() const { return current_vertex_count; }
    float GetCurrentError() const { return current_error; }

    const Parameters& GetParameters() const { return active_params; }
    void SetParameters(const Parameters& params) { UpdateParameters(params); }


private:
    struct EdgeCollapse {
        size_t v1;
        size_t v2;
        float collapse_cost;
        std::vector<size_t> affected_triangles;
    };

    float ComputeCollapseCost(size_t v1_index, size_t v2_index) const;
    void ApplyEdgeCollapse(const EdgeCollapse& collapse);
    void RemoveDegenerateTriangles();
    void RebuildPriorityQueue();

    std::vector<Vertex> original_vertices;
    std::vector<uint32_t> original_indices;
    std::vector<bool> vertex_validity;
    std::vector<EdgeCollapse> collapse_candidates;
    std::priority_queue<std::pair<float, size_t>> collapse_queue;

    Mesh& target_mesh;
    Parameters active_params;
    size_t current_vertex_count;
    float current_error = 0.0f;
    bool is_precomputed = false;
};
