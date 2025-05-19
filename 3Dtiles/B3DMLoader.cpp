#include "B3DMLoader.h"
//#include <nlohmann/json.hpp>
#include <glm/glm.hpp>
#include <fstream>
#include <iostream>：
#include <sstream>

using json = nlohmann::json;

Mesh B3DMLoader::LoadFromFile(const std::string& filepath) {
    std::ifstream fin(filepath, std::ios::binary);
    if (!fin.is_open()) {
        throw std::runtime_error("无法打开B3DM文件: " + filepath);
    }
    // 在 B3DMLoader.cpp 的 LoadFromFile 函数开头添加：
    std::cout << "\n====== 开始加载 B3DM 文件 ======\n"
              << "文件路径: " << filepath << std::endl;

    // 读取B3DM头
    struct B3DMHeader {
        char magic[4];
        uint32_t version;
        uint32_t byteLength;
        uint32_t featureTableJSONLength;
        uint32_t featureTableBinaryLength;
        uint32_t batchTableJSONLength;
        uint32_t batchTableBinaryLength;
    } header;
    
    fin.read(reinterpret_cast<char*>(&header), sizeof(header));
    if (std::string(header.magic, 4) != "b3dm") {
        throw std::runtime_error("无效的B3DM文件头");
    }

    // 跳过特征表和批量表
    fin.seekg(header.featureTableJSONLength + header.featureTableBinaryLength + 
             header.batchTableJSONLength + header.batchTableBinaryLength, 
             std::ios::cur);

    // 读取glb数据
    std::vector<uint8_t> glbData(header.byteLength - 28);
    fin.read(reinterpret_cast<char*>(glbData.data()), glbData.size());

    // 解析glTF
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    ParseGLB(glbData, vertices, indices);

    std::cout << "\n===== 正在解析: " << filepath << " =====" << std::endl;
    ParseGLB(glbData, vertices, indices);
    std::cout << "解析完成 | 顶点数: " << vertices.size() 
              << " | 索引数: " << indices.size() << std::endl;

    return Mesh(std::move(vertices), std::move(indices));
}

void B3DMLoader::ParseGLB(const std::vector<uint8_t>& glbData,
                         std::vector<Vertex>& outVertices,
                         std::vector<unsigned int>& outIndices) {
    size_t pos = 0;
    
    // 验证glb头
    if (std::string_view(reinterpret_cast<const char*>(glbData.data()), 4) != "glTF") {
        throw std::runtime_error("Invalid glb header");
    }
    pos += 12; // 跳过magic和版本

    json jsonData;
    std::vector<uint8_t> binData;

    // 解析chunks
    while (pos < glbData.size()) {
        uint32_t chunkLength;
        memcpy(&chunkLength, glbData.data() + pos, 4);
        pos += 4;

        char chunkType[5] = {0};
        memcpy(chunkType, glbData.data() + pos, 4);
        pos += 4;

        if (std::string_view(chunkType, 4) == "JSON") {
            // 修复点：显式类型转换
            const char* jsonStart = reinterpret_cast<const char*>(glbData.data() + pos);
            jsonData = json::parse(jsonStart, jsonStart + chunkLength);
            pos += chunkLength;
        } else if (std::string_view(chunkType, 4) == "BIN ") {
            binData.assign(glbData.begin() + pos, glbData.begin() + pos + chunkLength);
            pos += chunkLength;
        } else {
            pos += chunkLength;
        }
    }


    // 解析访问器和缓冲视图
    std::vector<GLTFAccessor> accessors;
    for (const auto& acc : jsonData["accessors"]) {
        accessors.push_back({
            acc["bufferView"].get<uint32_t>(),
            acc["componentType"].get<uint32_t>(),
            acc["count"].get<uint32_t>(),
            acc["type"].get<std::string>()
        });
    }

    std::vector<GLTFBufferView> bufferViews;
    for (const auto& bv : jsonData["bufferViews"]) {
        bufferViews.push_back({
            bv["buffer"].get<uint32_t>(),
            bv["byteOffset"].get<uint32_t>(),
            bv["byteLength"].get<uint32_t>()
        });
    }

    // 处理每个mesh
    for (const auto& mesh : jsonData["meshes"]) {
        for (const auto& primitive : mesh["primitives"]) {
            ProcessPrimitive(primitive, binData, accessors, bufferViews, outVertices, outIndices);
        }
    }
}

void B3DMLoader::ProcessPrimitive(const json& primitive,
                                 const std::vector<uint8_t>& binData,
                                 const std::vector<GLTFAccessor>& accessors,
                                 const std::vector<GLTFBufferView>& bufferViews,
                                 std::vector<Vertex>& vertices,
                                 std::vector<unsigned int>& indices) {
    // 处理顶点数据
    const auto& attributes = primitive["attributes"];
    
    // 位置属性
    const auto& posAccessor = accessors[attributes["POSITION"].get<uint32_t>()];
    const auto& posView = bufferViews[posAccessor.bufferView];
    const float* positions = reinterpret_cast<const float*>(
        binData.data() + posView.byteOffset
    );

    // 法线属性（可选）
    const float* normals = nullptr;
    if (attributes.contains("NORMAL")) {
        const auto& normAccessor = accessors[attributes["NORMAL"].get<uint32_t>()];
        const auto& normView = bufferViews[normAccessor.bufferView];
        normals = reinterpret_cast<const float*>(
            binData.data() + normView.byteOffset
        );
    }

    // 纹理坐标（可选）
    const float* texCoords = nullptr;
    if (attributes.contains("TEXCOORD_0")) {
        const auto& uvAccessor = accessors[attributes["TEXCOORD_0"].get<uint32_t>()];
        const auto& uvView = bufferViews[uvAccessor.bufferView];
        texCoords = reinterpret_cast<const float*>(
            binData.data() + uvView.byteOffset
        );
    }

    // 填充顶点数据
    const size_t vertexCount = posAccessor.count;
    for (size_t i = 0; i < vertexCount; ++i) {
        Vertex v;
        memcpy(&v.Position, positions + i*3, 12);
        
        if (normals) {
            memcpy(&v.Normal, normals + i*3, 12);
        } else {
            v.Normal = glm::vec3(0.0f);
        }
        
        if (texCoords) {
            memcpy(&v.TexCoords, texCoords + i*2, 8);
        } else {
            v.TexCoords = glm::vec2(0.0f);
        }
        
        vertices.push_back(v);
    }

    // 处理索引数据
    const auto& indexAccessor = accessors[primitive["indices"].get<uint32_t>()];
    const auto& indexView = bufferViews[indexAccessor.bufferView];
    const uint8_t* indexBuffer = binData.data() + indexView.byteOffset;

    const size_t indexCount = indexAccessor.count;
    switch (indexAccessor.componentType) {
        case 5123: // UNSIGNED_SHORT
            for (size_t i = 0; i < indexCount; ++i) {
                uint16_t val;
                memcpy(&val, indexBuffer + i*2, 2);
                indices.push_back(val);
            }
            break;
        case 5125: // UNSIGNED_INT
            indices.insert(indices.end(),
                reinterpret_cast<const unsigned int*>(indexBuffer),
                reinterpret_cast<const unsigned int*>(indexBuffer + indexCount*4)
            );
            break;
        default:
            throw std::runtime_error("不支持的索引格式");
    }

    
}
