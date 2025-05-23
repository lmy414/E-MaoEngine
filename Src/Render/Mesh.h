// Mesh.h
#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include "ProgressiveLOD.h" // 新增关键包含
#include "Vertex.h"


class ProgressiveLOD; // 前向声明

class Mesh {
public:
    // 使用移动语义避免复制大数据
    Mesh(std::vector<Vertex>&& vertices, 
        std::vector<unsigned int>&& indices);
    
    ~Mesh();
    
    // 禁止拷贝，允许移动
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;

    bool IsReady() const { return VAO != 0; } // 根据实际渲染资源初始化状态判断

    void Draw() const;
    void Destroy(); // 显式释放资源
    
    // 顶点数据处理
    void CalculateNormals();
    void UpdateGPUData();
    
    // 访问器
    const std::vector<Vertex>& GetVertices() const { return vertices; }
    const std::vector<unsigned int>& GetIndices() const { return indices; }

    std::vector<Vertex>& GetVertices() { return vertices; }
    std::vector<unsigned int>& GetIndices() { return indices; }


private:
    ProgressiveLOD& GetLODController(); 
    void SetupBuffers();
    void ClearGPUResources();
    void CheckGLError(int line);
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
