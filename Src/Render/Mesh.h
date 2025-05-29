/**
 * @file Mesh.h
 * @brief 3D网格模型的数据结构和渲染管理类
 * @author MirrorEngine Team
 * @date 2024
 */
#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include "ProgressiveLOD.h" // 新增关键包含
#include "Vertex.h"


class ProgressiveLOD; // 前向声明

/**
 * @class Mesh
 * @brief 表示一个3D网格模型，管理顶点数据和渲染状态
 * 
 * 该类负责管理网格的顶点数据、索引数据，以及相关的GPU资源。
 * 支持移动语义但禁止拷贝，以优化性能和资源管理。
 */
class Mesh {
public:
    /**
     * @brief 构造一个新的网格对象
     * @param vertices 顶点数据数组
     * @param indices 索引数据数组
     */
    Mesh(std::vector<Vertex>&& vertices, 
        std::vector<unsigned int>&& indices);
    
    ~Mesh();
    
    // 禁止拷贝，允许移动
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;

    /**
     * @brief 检查网格是否已准备好进行渲染
     * @return 如果GPU资源已初始化则返回true
     */
    bool IsReady() const { return VAO != 0; } // 根据实际渲染资源初始化状态判断

    /**
     * @brief 渲染网格
     */
    void Draw() const;

    /**
     * @brief 显式释放GPU资源
     */
    void Destroy(); // 显式释放资源
    
    /**
     * @brief 计算网格的法线数据
     */
    void CalculateNormals();

    /**
     * @brief 将数据更新到GPU
     */
    void UpdateGPUData();
    
    // Getters
    const std::vector<Vertex>& GetVertices() const { return vertices; }
    const std::vector<unsigned int>& GetIndices() const { return indices; }
    std::vector<Vertex>& GetVertices() { return vertices; }
    std::vector<unsigned int>& GetIndices() { return indices; }

private:
    ProgressiveLOD& GetLODController(); 
    void SetupBuffers();
    void ClearGPUResources();
    void CheckGLError(int line);

    /**
     * @brief 将偏移量转换为指针
     * @param offset 字节偏移量
     * @return GLvoid* 转换后的指针
     */
    static inline GLvoid* OffsetToPointer(size_t offset) noexcept {
        return reinterpret_cast<GLvoid*>(offset); // 简化类型转换
    }

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::unique_ptr<ProgressiveLOD> lod_controller;
    // OpenGL对象
    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint EBO = 0;
    bool isUploaded = false;
};
