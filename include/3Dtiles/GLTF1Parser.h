#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <json.hpp>
#include "Render/Mesh.h"
#include "Core/EndianUtils.h"

namespace Mirror { namespace GLTF {

class GLTF1Parser {
public:
    struct MeshData {
        std::vector<glm::vec3> positions;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> texCoords;
        std::vector<uint32_t> indices;
        glm::mat4 transform = glm::mat4(1.0f);

        ::Mesh ToMesh() const {
            std::vector<Vertex> verts;
            verts.reserve(positions.size());
            for (size_t i = 0; i < positions.size(); ++i) {
                verts.push_back({positions[i], normals[i], texCoords.size()>i?texCoords[i]:glm::vec2(0.0f)});
            }
            return ::Mesh(std::move(verts), std::vector<unsigned int>(indices.begin(), indices.end()));
        }
    };

    #pragma pack(push, 1)
    struct GLBHeader {
        char     magic[4];
        uint32_t version;
        uint32_t fileLength;
    };
    struct ChunkHeader {
        uint32_t chunkLength;
        uint32_t chunkType;
    };
    #pragma pack(pop)

    static MeshData Parse(const std::vector<uint8_t>& data) { return GLTF1Parser().ParseImpl(data); }

private:
    MeshData ParseImpl(const std::vector<uint8_t>& data);
    void ValidateGLBHeader(const GLBHeader& header, const std::vector<uint8_t>& data, size_t baseOffset);
    void ParseScene(const nlohmann::json& root);
    void ParseNode(const nlohmann::json& node);
    void ParseMesh(const nlohmann::json& mesh);
    void ParsePrimitive(const nlohmann::json& primitive);

    template<typename T>
    const T* GetBufferViewData(const std::string& bufferViewId, size_t accessorOffset, size_t count, size_t elementSize);
    template<typename T>
    std::vector<T> ReadStridedData(const uint8_t* basePtr, size_t count, size_t stride, size_t elementSize);

    const uint8_t* m_BinaryChunk = nullptr;
    size_t m_BinaryChunkSize = 0;
    nlohmann::json m_SceneJson;
    MeshData m_Result;
    std::vector<uint8_t> m_StridedCache;
};

}} // namespace Mirror::GLTF
