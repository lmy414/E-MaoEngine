/**
 * @file Vertex.h
 * @brief 3D顶点数据结构定义
 * @author MirrorEngine Team
 * @date 2024
 */
#pragma once
#include <glm/glm.hpp>

/**
 * @struct Vertex
 * @brief 表示3D模型中的一个顶点
 * 
 * 包含顶点的位置、法线和纹理坐标信息。
 * 内存布局符合OpenGL顶点属性规范。
 */
struct Vertex {
    glm::vec3 Position;    ///< 顶点在3D空间中的位置
    glm::vec3 Normal;      ///< 顶点的法线向量
    glm::vec2 TexCoords;   ///< 顶点的纹理坐标 (UV)

    /**
     * @brief 默认构造函数
     */
    Vertex() = default;

    /**
     * @brief 构造具有完整属性的顶点
     * @param pos 顶点位置
     * @param norm 顶点法线
     * @param tex 纹理坐标
     */
    Vertex(const glm::vec3& pos, const glm::vec3& norm = glm::vec3(0.0f), 
           const glm::vec2& tex = glm::vec2(0.0f))
        : Position(pos), Normal(norm), TexCoords(tex) {}
};
