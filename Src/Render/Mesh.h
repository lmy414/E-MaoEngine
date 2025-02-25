#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>

// 顶点结构体
struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

class Mesh {
public:
    Mesh(const std::vector<Vertex>& vertices, 
        const std::vector<unsigned int>& indices);
    
    void Draw();

private:
    void SetupMesh();

    // 网格数据
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    
    // OpenGL对象
    unsigned int VAO, VBO, EBO;
};
