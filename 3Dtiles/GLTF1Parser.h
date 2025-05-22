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
                // 保持与Mesh类兼容的转换方法
                ::Mesh ToMesh() const {  // 使用全局命名空间符号
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
                char magic[4];        // 'g','l','T','F'
                uint32_t version;     // 小端存储
                uint32_t fileLength;  // 小端存储
            };

            struct ChunkHeader {
                uint32_t chunkLength; // 小端存储
                uint32_t chunkType;   // 大端ASCII
            };
#pragma pack(pop)

            static MeshData Parse(const std::vector<uint8_t>& glbData);

        private:

            // 实例方法封装
            MeshData ParseImpl(const std::vector<uint8_t>& glbData);
            void ValidateGLBHeader(const GLBHeader& header, const std::vector<uint8_t>& glbData);
            void ParseScene(const nlohmann::json& root);
            void ParseNode(const nlohmann::json& node);
            void ParseMesh(const nlohmann::json& mesh);
            void ParsePrimitive(const nlohmann::json& primitive);

            // 模板方法定义
            template<typename T>
            const T* GetBufferViewData(const std::string& bufferViewId, size_t count);

            // 实例状态
            const uint8_t* m_BinaryChunk = nullptr;
            size_t m_BinaryChunkSize = 0;
            nlohmann::json m_SceneJson;
            MeshData m_Result;
        };

    } // namespace GLTF
} // namespace Mirror
