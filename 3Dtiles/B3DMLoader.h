#pragma once
#include "../Src/Render/Mesh.h"
#include <glm/glm.hpp>
#include <vector>
#include <fstream>
#include <json.hpp>

class B3DMLoader {
public:
    static Mesh LoadFromFile(const std::string& filepath);
    
private:
    struct GLTFAccessor {
        uint32_t bufferView;
        uint32_t componentType;
        uint32_t count;
        std::string type;
    };

    struct GLTFBufferView {
        uint32_t buffer;
        uint32_t byteOffset;
        uint32_t byteLength;
    };

    static void ParseGLB(const std::vector<uint8_t>& glbData, 
                        std::vector<Vertex>& outVertices,
                        std::vector<unsigned int>& outIndices);
    
    static void ProcessPrimitive(const nlohmann::json& json,
                               const std::vector<uint8_t>& binData,
                               const std::vector<GLTFAccessor>& accessors,
                               const std::vector<GLTFBufferView>& bufferViews,
                               std::vector<Vertex>& vertices,
                               std::vector<unsigned int>& indices);
};