// TilesetLoader.h
#pragma once
#include <vector>
#include <string>
#include <memory>
#include <glm/glm.hpp>

struct BoundingSphere {
    glm::dvec3 center;
    double radius;
};

struct TileNode {
    // 必需字段
    BoundingSphere volume{};
    double geometricError = 0.0;
    std::string contentUri;
    std::vector<std::unique_ptr<TileNode>> children;

    // 构造函数
    TileNode() = default; // 默认构造函数
    TileNode(TileNode&&) = default;
    TileNode& operator=(TileNode&&) = default;

    // 禁用拷贝
    TileNode(const TileNode&) = delete;
    TileNode& operator=(const TileNode&) = delete;

    std::unique_ptr<TileNode> clone() const {
        auto newNode = std::make_unique<TileNode>();
        newNode->volume = this->volume;
        newNode->geometricError = this->geometricError;
        newNode->contentUri = this->contentUri;
        
        for (const auto& child : this->children) {
            newNode->children.push_back(child->clone());
        }
        return newNode;
    }
};

namespace TilesetParser {
    std::unique_ptr<TileNode> loadTileset(const std::string& filepath);
    void PrintDebugInfo(const TileNode* node, int depth = 0);
    void ClearCache();
}
