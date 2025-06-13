// GLBParser.h
#pragma once
#include <vector>
#include <glm/glm.hpp>
// 在包含GLM头文件的位置添加
#include <glm/gtc/type_ptr.hpp>  // 必须包含的value_ptr来源
#include "Render//Mesh.h" // 确保正确包含路径
#include "Resources/tiny_gltf.h" // 需要集成tinygltf库


// GLB头结构
#pragma pack(push, custom_alignment)
struct GLBHeader {
    char magic[4];
    uint32_t version;
    uint32_t fileLength;
};
struct ChunkHeader {
    uint32_t chunkLength;
    uint32_t chunkType;
};


namespace Mirror
{
    namespace GLTF
    {
class GLBParser {
public:
    struct GLBData {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        glm::mat4 transform = glm::mat4(1.0f);
        
        Mesh ToMesh() const;
    };
    static GLBData Parse(const std::vector<uint8_t>& glbData);
private:
    static void ProcessModel(const tinygltf::Model& model, GLBData& result);
    static void ProcessPrimitive(const tinygltf::Model& model,
                               const tinygltf::Primitive& primitive,
                               GLBData& result);
    // 模板定义直接实现在头文件中
    template <typename T>
    static const T* GetBufferData(const tinygltf::Model& model,
                                const tinygltf::Accessor& accessor) {
        const auto& bufferView = model.bufferViews[accessor.bufferView];
        const auto& buffer = model.buffers[bufferView.buffer];
        return reinterpret_cast<const T*>(
            &buffer.data[bufferView.byteOffset + accessor.byteOffset]
        );
    }
    template <typename SrcType>
    static void AppendIndices(const tinygltf::Model& model,
                            const tinygltf::Accessor& accessor,
                            std::vector<unsigned int>& output,
                            size_t vertexOffset) {
        const auto* src = GetBufferData<SrcType>(model, accessor);
        const size_t count = accessor.count;
        for (size_t i = 0; i < count; ++i) {
            output.push_back(static_cast<unsigned int>(src[i]) + vertexOffset);
        }
    }
};
    }
}

