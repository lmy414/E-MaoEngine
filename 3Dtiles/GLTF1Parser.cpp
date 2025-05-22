// GLTF1Parser.cpp
#include "GLTF1Parser.h"
#include <fstream>
#include <sstream>
#include <glm/gtc/type_ptr.hpp>
#include "../Src/Core/EndianUtils.h"

using namespace Mirror::GLTF;

GLTF1Parser::MeshData GLTF1Parser::Parse(const std::vector<uint8_t>& glbData) {
    return GLTF1Parser().ParseImpl(glbData);
}

GLTF1Parser::MeshData GLTF1Parser::ParseImpl(const std::vector<uint8_t>& glbData) {
    // 基础验证
    if (glbData.size() < sizeof(GLBHeader)) {
        throw std::runtime_error("Invalid GLB size");
    }

    // 解析头部
    const GLBHeader* header = reinterpret_cast<const GLBHeader*>(glbData.data());
    ValidateGLBHeader(*header, glbData);

    // 解析JSON分块
    size_t offset = sizeof(GLBHeader);
    const ChunkHeader* jsonChunk = reinterpret_cast<const ChunkHeader*>(glbData.data() + offset);
    
    const uint32_t jsonChunkLength = Mirror::Core::EndianUtils::FromLittleEndian(jsonChunk->chunkLength);
    const uint32_t jsonChunkType = Mirror::Core::EndianUtils::FromBigEndian(jsonChunk->chunkType);

    // ============ 新增容错逻辑开始 ============
    bool forceParseJson = false;
    if (jsonChunkType != 0x4E4F534A) { // 非标准JSON分块
        if (jsonChunkType == 0 && jsonChunkLength > 0) {
            // 检测JSON特征
            const char* jsonStart = reinterpret_cast<const char*>(glbData.data() + offset + sizeof(ChunkHeader));
            if (jsonStart[0] == '{' && jsonChunkLength <= (glbData.size() - offset - sizeof(ChunkHeader))) {
                forceParseJson = true;
                std::cerr << "[Warning] Forcing JSON chunk parsing with invalid type 0x0. "
                          << "This may indicate a toolchain issue." << std::endl;
            }
        }
        
        if (!forceParseJson) {
            std::stringstream ss;
            ss << "Expected JSON chunk, got: 0x" << std::hex << jsonChunkType;
            throw std::runtime_error(ss.str());
        }
    }
    // ============ 新增容错逻辑结束 ============

    // 解析JSON数据
    const char* jsonStart = reinterpret_cast<const char*>(glbData.data() + offset + sizeof(ChunkHeader));
    try {
        m_SceneJson = nlohmann::json::parse(jsonStart, jsonStart + jsonChunkLength);
    } catch (const std::exception& e) {
        throw std::runtime_error("JSON parsing failed: " + std::string(e.what()));
    }

    // 定位二进制分块
    offset += sizeof(ChunkHeader) + jsonChunkLength;
    offset = (offset + 3) & ~3; // 4字节对齐

    if (offset + sizeof(ChunkHeader) > glbData.size()) {
        throw std::runtime_error("Missing BIN chunk");
    }

    const ChunkHeader* binChunk = reinterpret_cast<const ChunkHeader*>(glbData.data() + offset);
    const uint32_t binChunkType = Mirror::Core::EndianUtils::FromBigEndian(binChunk->chunkType);
    
    if (binChunkType != 0x004E4942) { // 'BIN'
        std::stringstream ss;
        ss << "BIN chunk type error at 0x" << std::hex << offset 
        << "\nActual type: 0x";
    }

    m_BinaryChunkSize = Mirror::Core::EndianUtils::FromLittleEndian(binChunk->chunkLength);
    m_BinaryChunk = glbData.data() + offset + sizeof(ChunkHeader);

    // 解析场景数据
    ParseScene(m_SceneJson);
    return m_Result;
}

void GLTF1Parser::ValidateGLBHeader(const GLBHeader& header, const std::vector<uint8_t>& glbData) {
    if (memcmp(header.magic, "glTF", 4) != 0) {
        throw std::runtime_error("Invalid GLB magic");
    }

    const uint32_t version = Mirror::Core::EndianUtils::FromLittleEndian(header.version);
    if (version != 1) {
        throw std::runtime_error("Unsupported glTF version: " + std::to_string(version));
    }

    const uint32_t length = Mirror::Core::EndianUtils::FromLittleEndian(header.fileLength);
    if (length != glbData.size()) {
        throw std::runtime_error("GLB length mismatch");
    }
}

template<typename T>
const T* GLTF1Parser::GetBufferViewData(
    const std::string& bufferViewId,
    size_t accessorByteOffset,
    size_t count,
    size_t elementSize
) {
    const auto& bufferView = m_SceneJson["bufferViews"][bufferViewId];
    const size_t bufferViewByteOffset = bufferView["byteOffset"];
    const size_t totalByteOffset = bufferViewByteOffset + accessorByteOffset;
    const size_t byteStride = bufferView.value("byteStride", 0);
    size_t requiredSpace = 0;
    if (byteStride == 0) {
        requiredSpace = elementSize * count;
    } else {
        requiredSpace = byteStride * (count - 1) + elementSize;
    }
    if (totalByteOffset + requiredSpace > m_BinaryChunkSize) {
        throw std::runtime_error("BufferView overflow: " + bufferViewId);
    }
    if (byteStride != 0 && byteStride < elementSize) {
        throw std::runtime_error("Invalid stride in buffer view: " + bufferViewId);
    }
    const uint8_t* dataPtr = m_BinaryChunk + totalByteOffset;
    // 处理跨步数据
    if (byteStride != 0 && byteStride != elementSize) {
        m_StridedCache = ReadStridedData<uint8_t>(
            dataPtr, 
            count * elementSize,
            byteStride,
            elementSize
        );
        return reinterpret_cast<const T*>(m_StridedCache.data());
    }
    return reinterpret_cast<const T*>(dataPtr);
}
template<typename T>
std::vector<T> GLTF1Parser::ReadStridedData(
    const uint8_t* basePtr,
    size_t count,
    size_t stride,
    size_t elementSize
) {
    std::vector<T> result(count);
    for (size_t i = 0; i < count; ++i) {
        const auto* src = basePtr + i * stride;
        memcpy(&result[i], src, elementSize);
    }
    return result;
}

void GLTF1Parser::ParseScene(const nlohmann::json& root) {
    if (!root.contains("scene")) {
        throw std::runtime_error("Missing default scene");
    }

    const auto& scene = root["scenes"][root["scene"].get<std::string>()];
    for (const auto& nodeRef : scene["nodes"]) {
        ParseNode(root["nodes"][nodeRef.get<std::string>()]);
    }
}

void GLTF1Parser::ParseNode(const nlohmann::json& node) {
    // 解析变换矩阵
    if (node.contains("matrix")) {
        auto matrixValues = node["matrix"].get<std::vector<double>>();
        if (matrixValues.size() != 16) {
            throw std::runtime_error("Invalid matrix size");
        }

        float matrix[16];
        for (int i = 0; i < 16; ++i) {
            matrix[i] = static_cast<float>(matrixValues[i]);
        }
        m_Result.transform = glm::make_mat4(matrix);
    }

    // 处理子节点
    if (node.contains("children")) {
        for (const auto& childRef : node["children"]) {
            ParseNode(m_SceneJson["nodes"][childRef.get<std::string>()]);
        }
    }

    // 处理网格
    if (node.contains("meshes")) {
        for (const auto& meshRef : node["meshes"]) {
            ParseMesh(m_SceneJson["meshes"][meshRef.get<std::string>()]);
        }
    }
}

void GLTF1Parser::ParseMesh(const nlohmann::json& mesh) {
    for (const auto& primitive : mesh["primitives"]) {
        ParsePrimitive(primitive);
    }
}

void GLTF1Parser::ParsePrimitive(const nlohmann::json& primitive) {
    if (primitive.value("mode", 4) != 4) return;
    const auto& attributes = primitive["attributes"];
    
    // 位置
    const auto& posAccessor = m_SceneJson["accessors"][attributes["POSITION"].get<std::string>()];
    const size_t vertexCount = posAccessor["count"];
    const size_t posAccessorOffset = posAccessor.value("byteOffset", 0);
    const auto* positions = GetBufferViewData<glm::vec3>(
        posAccessor["bufferView"].get<std::string>(),
        posAccessorOffset,
        vertexCount,
        sizeof(glm::vec3)
    );
    // 法线
    const auto& normalAccessor = m_SceneJson["accessors"][attributes["NORMAL"].get<std::string>()];
    const size_t normalAccessorOffset = normalAccessor.value("byteOffset", 0);
    const auto* normals = GetBufferViewData<glm::vec3>(
        normalAccessor["bufferView"].get<std::string>(),
        normalAccessorOffset,
        vertexCount,
        sizeof(glm::vec3)
    );
    // 纹理坐标
    std::vector<glm::vec2> uvs(vertexCount);
    if (attributes.contains("TEXCOORD_0")) {
        const auto& uvAccessor = m_SceneJson["accessors"][attributes["TEXCOORD_0"].get<std::string>()];
        const size_t uvAccessorOffset = uvAccessor.value("byteOffset", 0);
        const auto* uvData = GetBufferViewData<glm::vec2>(
            uvAccessor["bufferView"].get<std::string>(),
            uvAccessorOffset,
            vertexCount,
            sizeof(glm::vec2)
        );
        std::copy(uvData, uvData + vertexCount, uvs.begin());
    }
    // 索引
    const auto& indicesAccessor = m_SceneJson["accessors"][primitive["indices"].get<std::string>()];
    const size_t indexCount = indicesAccessor["count"];
    const auto componentType = indicesAccessor["componentType"].get<uint32_t>();
    std::vector<uint32_t> indices;
    if (componentType == 5123) { // UNSIGNED_SHORT
        const size_t indicesOffset = indicesAccessor.value("byteOffset", 0);
        const auto* raw = GetBufferViewData<uint16_t>(
            indicesAccessor["bufferView"].get<std::string>(),
            indicesOffset,      // 访问器偏移
            indexCount,         // 元素数量
            sizeof(uint16_t)   // 元素字节大小
        );
        indices.reserve(indexCount);
        for (size_t i = 0; i < indexCount; ++i) {
            indices.push_back(Mirror::Core::EndianUtils::FromLittleEndian(raw[i]));
        }
    } else if (componentType == 5125) { // UNSIGNED_INT
        const size_t indicesOffset = indicesAccessor.value("byteOffset", 0);
        const auto* raw = GetBufferViewData<uint32_t>(
            indicesAccessor["bufferView"].get<std::string>(),
            indicesOffset,      // 访问器偏移
            indexCount,         // 元素数量
            sizeof(uint32_t)    // 元素字节大小
        );
        indices.reserve(indexCount);
        for (size_t i = 0; i < indexCount; ++i) {
            indices.push_back(
                Mirror::Core::EndianUtils::FromLittleEndian(raw[i])
            );
        }
    } else {
        throw std::runtime_error("Unsupported index type");
    }

    std::cout << "Position accessor: " << attributes["POSITION"] << "\n"
              << "Normal accessor: " << attributes["NORMAL"] << "\n"
              << "POS bufferView: " << posAccessor["bufferView"] << "\n"
              << "NORMAL bufferView: " << normalAccessor["bufferView"] << "\n";
    // 合并数据
    const size_t baseIndex = m_Result.positions.size();
    
    m_Result.positions.insert(m_Result.positions.end(), positions, positions + vertexCount);
    m_Result.normals.insert(m_Result.normals.end(), normals, normals + vertexCount);
    m_Result.texCoords.insert(m_Result.texCoords.end(), uvs.begin(), uvs.end());
    
    for (auto index : indices) {
        m_Result.indices.push_back(baseIndex + index);
    }
}
// 显式模板实例化
template const glm::vec3* GLTF1Parser::GetBufferViewData<glm::vec3>(
    const std::string&, size_t, size_t, size_t);
template const glm::vec2* GLTF1Parser::GetBufferViewData<glm::vec2>(
    const std::string&, size_t, size_t, size_t);
template const uint16_t* GLTF1Parser::GetBufferViewData<uint16_t>(
    const std::string&, size_t, size_t, size_t);