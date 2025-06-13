// Mesh.cpp
#include "Mesh.h"
#include <iostream>
#include <sstream>


// 静态断言验证顶点结构体大小（考虑不同平台的字节对齐）
static_assert(sizeof(Vertex) == (sizeof(glm::vec3) * 2) + sizeof(glm::vec2),
             "Vertex 结构体内存布局不符合预期，请检查编译器对齐设置");

// 安全偏移量转换函数（头文件中声明为静态成员）
/*
private:
    static constexpr GLvoid* OffsetToPointer(size_t offset) noexcept {
        return reinterpret_cast<GLvoid*>(static_cast<uintptr_t>(offset));
    }
*/

Mesh::Mesh(std::vector<Vertex>&& vertices, 
         std::vector<unsigned int>&& indices)
    : vertices(std::move(vertices)), 
      indices(std::move(indices)) 
{
    SetupBuffers();
}

Mesh::~Mesh() {
    ClearGPUResources();
}

// 将内联实现移出头文件到Mesh.cpp中
ProgressiveLOD& Mesh::GetLODController() {
    if (!lod_controller) {
        lod_controller = std::make_unique<ProgressiveLOD>(*this);
    }
    return *lod_controller;
}

// 移动构造函数
Mesh::Mesh(Mesh&& other) noexcept
    : vertices(std::move(other.vertices)),
      indices(std::move(other.indices)),
      VAO(other.VAO),
      VBO(other.VBO),
      EBO(other.EBO),
      isUploaded(other.isUploaded) 
{
    other.VAO = other.VBO = other.EBO = 0;
    other.isUploaded = false;
}

// 移动赋值运算符
Mesh& Mesh::operator=(Mesh&& other) noexcept {
    if (this != &other) {
        ClearGPUResources();
        vertices = std::move(other.vertices);
        indices = std::move(other.indices);
        VAO = other.VAO;
        VBO = other.VBO;
        EBO = other.EBO;
        isUploaded = other.isUploaded;
        other.VAO = other.VBO = other.EBO = 0;
        other.isUploaded = false;
    }
    return *this;
}

void Mesh::SetupBuffers() {
    // 检查 OpenGL 上下文有效性
    if (!gladLoadGL()) {
        throw std::runtime_error("OpenGL 上下文未正确初始化");
    }

    ClearGPUResources();

    // 生成缓冲对象
    glGenVertexArrays(1, &VAO);
    CheckGLError(__LINE__);
    glGenBuffers(1, &VBO);
    CheckGLError(__LINE__);
    glGenBuffers(1, &EBO);
    CheckGLError(__LINE__);

    glBindVertexArray(VAO);
    CheckGLError(__LINE__);

    // 安全类型转换
    const auto vertexDataSize = static_cast<GLsizeiptr>(vertices.size() * sizeof(Vertex));
    const auto indexDataSize = static_cast<GLsizeiptr>(indices.size() * sizeof(unsigned int));

    // 顶点缓冲
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    CheckGLError(__LINE__);
    glBufferData(GL_ARRAY_BUFFER, vertexDataSize, vertices.data(), GL_DYNAMIC_DRAW);
    CheckGLError(__LINE__);

    // 索引缓冲
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    CheckGLError(__LINE__);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexDataSize, indices.data(), GL_DYNAMIC_DRAW);
    CheckGLError(__LINE__);

    // 顶点属性配置（使用安全转换）
    constexpr GLsizei stride = sizeof(Vertex);
    const auto positionOffset = OffsetToPointer(offsetof(Vertex, Position));
    const auto normalOffset = OffsetToPointer(offsetof(Vertex, Normal));
    const auto texCoordOffset = OffsetToPointer(offsetof(Vertex, TexCoords));

    // 位置属性
    glEnableVertexAttribArray(0);
    CheckGLError(__LINE__);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, positionOffset);
    CheckGLError(__LINE__);
    
    // 法线属性
    glEnableVertexAttribArray(1);
    CheckGLError(__LINE__);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, normalOffset);
    CheckGLError(__LINE__);
    
    // 纹理坐标属性
    glEnableVertexAttribArray(2);
    CheckGLError(__LINE__);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, texCoordOffset);
    CheckGLError(__LINE__);

    glBindVertexArray(0);
    isUploaded = true;
    
    static bool hasPrinted = false;
    if (!hasPrinted && !vertices.empty()) {
        std::cout << "\n-----  -----\n";
        // 打印VAO绑定的顶点属性
        std::cout << "VAO：" << std::endl;
        std::cout << "  - 0 (P): " << std::endl;
        std::cout << "  - 1 (N): " << std::endl;
        std::cout << "  - 2 (T): \n" << std::endl;
        // 打印VBO中的原始数据
        std::cout << "VBO：" << std::endl;
        for (size_t i = 0; i < std::min(size_t(20), vertices.size()); ++i) {
            const auto& v = vertices[i];
            printf("  [%2zu] Pos=(%6.2f, %6.2f, %6.2f) | Normal=(%6.2f, %6.2f, %6.2f) | UV=(%4.2f, %4.2f)\n",
                   i,
                   v.Position.x, v.Position.y, v.Position.z,
                   v.Normal.x, v.Normal.y, v.Normal.z,
                   v.TexCoords.x, v.TexCoords.y);
        }
        // 打印EBO中的索引数据（修正类型不匹配）
        if (!indices.empty()) {
            std::cout << "\nEBO：" << std::endl;
            for (size_t i = 0; i < std::min(size_t(20), indices.size()); ++i) {
                std::cout << "  " << indices[i] << (i % 3 == 2 ? "\n" : ", ");
            }
        }
        std::cout << "----------------------------------------\n" << std::endl;
        hasPrinted = true;
    }


}

void Mesh::Draw() const {
    if (!isUploaded || VAO == 0) {
        std::cerr << "警告：尝试渲染未上传的网格" << std::endl;
        return;
    }
    
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 
                  static_cast<GLsizei>(indices.size()), 
                  GL_UNSIGNED_INT, 
                  nullptr);
    glBindVertexArray(0);
}

void Mesh::CalculateNormals() {
    // 初始化法线
    for (auto& vertex : vertices) {
        vertex.Normal = glm::vec3(0.0f);
    }

    // 遍历三角形面片
    for (size_t i = 0; i < indices.size(); i += 3) {
        Vertex& v0 = vertices[indices[i]];
        Vertex& v1 = vertices[indices[i+1]];
        Vertex& v2 = vertices[indices[i+2]];

        glm::vec3 edge1 = v1.Position - v0.Position;
        glm::vec3 edge2 = v2.Position - v0.Position;
        glm::vec3 faceNormal = glm::normalize(glm::cross(edge1, edge2));

        v0.Normal += faceNormal;
        v1.Normal += faceNormal;
        v2.Normal += faceNormal;
    }

    // 标准化法线
    for (auto& vertex : vertices) {
        vertex.Normal = glm::normalize(vertex.Normal);
    }
}

void Mesh::Destroy() {
    ClearGPUResources();
    vertices.clear();
    indices.clear();
}

void Mesh::ClearGPUResources() {
    if (VAO) {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }
    if (VBO) {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }
    if (EBO) {
        glDeleteBuffers(1, &EBO);
        EBO = 0;
    }
    isUploaded = false;
}


void Mesh::UpdateGPUData() {
    if (!IsReady()) {
        SetupBuffers(); // 首次上传数据
        return;
    }

    glBindVertexArray(VAO);
    CheckGLError(__LINE__);

    // 更新顶点缓冲区
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 
                vertices.size() * sizeof(Vertex), 
                vertices.data(), 
                GL_DYNAMIC_DRAW); // 修改为动态模式

    // 更新索引缓冲区
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                indices.size() * sizeof(unsigned int),
                indices.data(),
                GL_DYNAMIC_DRAW); // 修改为动态模式

    glBindVertexArray(0);
    isUploaded = true;
}

// 错误检查函数
void Mesh::CheckGLError(int line) {
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::ostringstream oss;
        oss << "OpenGL 错误 (0x" << std::hex << err << ") 在行号: " << line;
        throw std::runtime_error(oss.str());
    }
}
