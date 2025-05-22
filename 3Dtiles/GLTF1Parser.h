#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <json.hpp>
#include <iostream>
#include "../Src/Render/Mesh.h"

namespace Mirror {
    namespace GLTF {

        class GLTF1Parser {
        public:
            struct MeshData {
                std::vector<glm::vec3> positions;
                std::vector<glm::vec3> normals;
                std::vector<glm::vec2> texCoords;
                std::vector<uint32_t> indices;
                glm::mat4 transform = glm::mat4(1.0f);
                
                ::Mesh ToMesh() const {
                    try {
                        std::vector<Vertex> verts;
                        for(size_t i=0; i<positions.size(); ++i) {
                            verts.push_back({positions[i], normals[i], texCoords[i]});
                        }
                        return ::Mesh(std::move(verts), 
                                    {indices.begin(), indices.end()});
                    } catch(...) {
                        std::cerr << "ToMesh conversion failed" << std::endl;
                        throw;
                    }
                }
            };

#pragma pack(push, 1)
            struct GLBHeader {
                char magic[4];
                uint32_t version;
                uint32_t fileLength;
            };

            struct ChunkHeader {
                uint32_t chunkLength;
                uint32_t chunkType;
            };
#pragma pack(pop)

            static MeshData Parse(const std::vector<uint8_t>& glbData);

        private:
            MeshData ParseImpl(const std::vector<uint8_t>& glbData);
            void ValidateGLBHeader(const GLBHeader& header, const std::vector<uint8_t>& glbData);
            void ParseScene(const nlohmann::json& root);
            void ParseNode(const nlohmann::json& node);
            void ParseMesh(const nlohmann::json& mesh);
            void ParsePrimitive(const nlohmann::json& primitive);

            template<typename T>
            const T* GetBufferViewData(
                const std::string& bufferViewId,
                size_t accessorByteOffset,
                size_t count,
                size_t elementSize
            );

            template<typename T>
            std::vector<T> ReadStridedData(
                const uint8_t* basePtr,
                size_t count,
                size_t stride,
                size_t elementSize
            );

            const uint8_t* m_BinaryChunk = nullptr;
            size_t m_BinaryChunkSize = 0;
            nlohmann::json m_SceneJson;
            MeshData m_Result;
            std::vector<uint8_t> m_StridedCache; // 用于存储跨步数据的临时缓存
        };

        // 显式模板实例化声明
        extern template const glm::vec3* GLTF1Parser::GetBufferViewData<glm::vec3>(
            const std::string&, size_t, size_t, size_t);
        extern template const glm::vec2* GLTF1Parser::GetBufferViewData<glm::vec2>(
            const std::string&, size_t, size_t, size_t);
        extern template const uint16_t* GLTF1Parser::GetBufferViewData<uint16_t>(
            const std::string&, size_t, size_t, size_t);

    } // namespace GLTF
} // namespace Mirror
