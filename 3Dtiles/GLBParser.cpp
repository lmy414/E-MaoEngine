// GLBParser.cpp
#include "GLBParser.h"
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../Src/Resources/tiny_gltf.h" // 需要集成tinygltf库
#include <iostream>

GLBParser::GLBData GLBParser::Parse(const std::vector<uint8_t>& glbData) {
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err, warn;
    
    bool ret = loader.LoadBinaryFromMemory(
        &model, 
        &err, 
        &warn,
        glbData.data(), 
        glbData.size()
    );

    if (!warn.empty()) std::cerr << "[GLB警告] " << warn << std::endl;
    if (!err.empty()) throw std::runtime_error("[GLB错误] " + err);
    if (!ret) throw std::runtime_error("GLB解析失败");

    GLBData result;
    
    // 遍历所有网格
    for (const auto& mesh : model.meshes) {
        for (const auto& primitive : mesh.primitives) {
            // 顶点数据解析
            const auto& positions = model.accessors[primitive.attributes.at("POSITION")];
            const auto& normals = model.accessors[primitive.attributes.at("NORMAL")];
            const auto& texCoords = model.accessors[primitive.attributes.at("TEXCOORD_0")];
            
            const auto& posBuffer = model.bufferViews[positions.bufferView];
            const auto* posData = &model.buffers[posBuffer.buffer].data[posBuffer.byteOffset];
            
            const auto& normalBuffer = model.bufferViews[normals.bufferView];
            const auto* normalData = &model.buffers[normalBuffer.buffer].data[normalBuffer.byteOffset];
            
            const auto& uvBuffer = model.bufferViews[texCoords.bufferView];
            const auto* uvData = &model.buffers[uvBuffer.buffer].data[uvBuffer.byteOffset];

            // 填充顶点
            for (size_t i = 0; i < positions.count; ++i) {
                Vertex vertex; 

                // 位置
                const float* pos = reinterpret_cast<const float*>(posData + i * sizeof(float) * 3);
                vertex.Position = glm::vec3(pos[0], pos[1], pos[2]);
                
                // 法线
                const float* normal = reinterpret_cast<const float*>(normalData + i * sizeof(float) * 3);
                vertex.Normal = glm::vec3(normal[0], normal[1], normal[2]);
                
                // UV
                const float* uv = reinterpret_cast<const float*>(uvData + i * sizeof(float) * 2);
                vertex.TexCoords = glm::vec2(uv[0], uv[1]);
                
                result.vertices.push_back(vertex);
            }

            // 索引数据解析
            const auto& indices = model.accessors[primitive.indices];
            const auto& indexBuffer = model.bufferViews[indices.bufferView];
            const auto* indexData = &model.buffers[indexBuffer.buffer].data[indexBuffer.byteOffset];

            const size_t indexCount = indices.count;
            for (size_t i = 0; i < indexCount; ++i) {
                uint32_t index = 0;
                switch (indices.componentType) {
                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                        index = *reinterpret_cast<const uint32_t*>(indexData + i * sizeof(uint32_t));
                        break;
                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                        index = *reinterpret_cast<const uint16_t*>(indexData + i * sizeof(uint16_t));
                        break;
                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                        index = *reinterpret_cast<const uint8_t*>(indexData + i * sizeof(uint8_t));
                        break;
                    default:
                        throw std::runtime_error("不支持的索引格式");
                }
                result.indices.push_back(index);
            }
        }
    }

    return result;
}
