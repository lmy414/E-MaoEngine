// GLBParser.h
#pragma once
#include "../Src/Render/Mesh.h"
#include <vector>
#include <cstdint>
#include <glm/glm.hpp>

class GLBParser {
public:
    struct GLBData {
        std::vector<Vertex> vertices; // 使用全局Vertex类型
        std::vector<unsigned int> indices;
        glm::mat4 transform = glm::mat4(1.0f);
    };

    static GLBData Parse(const std::vector<uint8_t>& glbData);
};
