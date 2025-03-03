// Mesh.h
#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    
    // 未来扩展其他顶点属性
    // glm::vec3 Tangent;
    // glm::vec3 Bitangent;
};

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
    //void UploadToGPU(); 
    
    // 访问器
    const std::vector<Vertex>& GetVertices() const { return vertices; }
    const std::vector<unsigned int>& GetIndices() const { return indices; }

private:
    void SetupBuffers();
    void ClearGPUResources();
    void CheckGLError(int line);
    static inline GLvoid* OffsetToPointer(size_t offset) noexcept {
        return reinterpret_cast<GLvoid*>(offset); // 简化类型转换
    }

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    
    // OpenGL对象
    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint EBO = 0;
    bool isUploaded = false;
};
