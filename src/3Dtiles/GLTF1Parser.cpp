// GLTF1Parser.cpp
#include "GLTF1Parser.h"
#include <iostream>
#include <stdexcept>
#include <cstring>
#include "Core/EndianUtils.h"
#include <glm/gtc/type_ptr.hpp>

using namespace Mirror::GLTF;

void GLTF1Parser::ValidateGLBHeader(const GLBHeader& header,
                                     const std::vector<uint8_t>& data,
                                     size_t baseOffset)
{
    if (std::memcmp(header.magic, "glTF", 4) != 0)
        throw std::runtime_error("Invalid GLB magic");
    uint32_t version = Mirror::Core::EndianUtils::FromLittleEndian(header.version);
    if (version != 1)
        throw std::runtime_error("Unsupported glTF version: " + std::to_string(version));
    uint32_t length = Mirror::Core::EndianUtils::FromLittleEndian(header.fileLength);
    if (length != data.size() - baseOffset)
        throw std::runtime_error("GLB length mismatch");
}

GLTF1Parser::MeshData GLTF1Parser::ParseImpl(const std::vector<uint8_t>& data) {
    size_t offset = 0;
    if (data.size() < sizeof(GLBHeader))
        throw std::runtime_error("Data too small for GLB header");

    // 1. Parse GLB header
    const GLBHeader* header = reinterpret_cast<const GLBHeader*>(data.data() + offset);
    ValidateGLBHeader(*header, data, offset);
    offset += sizeof(GLBHeader);
    std::cout << "[Debug] After GLBHeader, offset=" << offset << "\n";

    // 2. Parse JSON chunk
    if (offset + sizeof(ChunkHeader) > data.size())
        throw std::runtime_error("Missing JSON chunk header");
    const ChunkHeader* jChunk = reinterpret_cast<const ChunkHeader*>(data.data() + offset);
    uint32_t jLen  = Mirror::Core::EndianUtils::FromLittleEndian(jChunk->chunkLength);
    uint32_t jType = Mirror::Core::EndianUtils::FromLittleEndian(jChunk->chunkType);
    std::cout << "[Debug] JSON chunkLength=" << jLen
              << " chunkType=0x" << std::hex << jType << std::dec << "\n";

    if (offset + sizeof(ChunkHeader) + jLen > data.size())
        throw std::runtime_error("JSON chunk out of bounds");
    const char* jStart = reinterpret_cast<const char*>(data.data() + offset + sizeof(ChunkHeader));
    m_SceneJson = nlohmann::json::parse(jStart, jStart + jLen);
    offset += sizeof(ChunkHeader) + jLen;
    offset = (offset + 3) & ~3;  // align to 4 bytes
    std::cout << "[Debug] After JSON chunk, offset=" << offset << "\n";

    // 3. Locate BIN chunk start; ignore its chunkLength field and take all remaining bytes
    if (offset + sizeof(ChunkHeader) > data.size())
        throw std::runtime_error("Missing BIN chunk header");
    // skip the 8-byte header entirely
    size_t binStart = offset + sizeof(ChunkHeader);
    if (binStart > data.size())
        throw std::runtime_error("BIN start beyond data size");

    uint32_t binLen = static_cast<uint32_t>(data.size() - binStart);
    m_BinaryChunk     = data.data() + binStart;
    m_BinaryChunkSize = binLen;
    std::cout << "[Debug] binStart=" << binStart
              << ", using binLen=" << binLen << "\n";

    // 4. Parse scene graph
    ParseScene(m_SceneJson);
    return m_Result;
}

void GLTF1Parser::ParseScene(const nlohmann::json& root) {
    if (!root.contains("scene"))
        throw std::runtime_error("Missing default scene");
    auto scene = root["scenes"][root["scene"].get<std::string>()];
    for (auto& n : scene["nodes"])
        ParseNode(root["nodes"][n.get<std::string>()]);
}

void GLTF1Parser::ParseNode(const nlohmann::json& node) {
    if (node.contains("matrix")) {
        auto m = node["matrix"].get<std::vector<double>>();
        if (m.size() != 16)
            throw std::runtime_error("Invalid matrix size");
        glm::mat4 mat;
        for (int i = 0; i < 16; ++i)
            glm::value_ptr(mat)[i] = static_cast<float>(m[i]);
        m_Result.transform = mat;
    }
    if (node.contains("children"))
        for (auto& c : node["children"])
            ParseNode(m_SceneJson["nodes"][c.get<std::string>()]);
    if (node.contains("meshes"))
        for (auto& m : node["meshes"])
            ParseMesh(m_SceneJson["meshes"][m.get<std::string>()]);
}

void GLTF1Parser::ParseMesh(const nlohmann::json& mesh) {
    for (auto& p : mesh["primitives"])
        ParsePrimitive(p);
}

void GLTF1Parser::ParsePrimitive(const nlohmann::json& primitive) {
    if (primitive.value("mode", 4) != 4) return; // TRIANGLES only

    auto& attr = primitive["attributes"];
    if (!attr.contains("POSITION") || !attr.contains("NORMAL"))
        throw std::runtime_error("Missing POSITION or NORMAL attributes");

    // Positions
    auto accPos   = m_SceneJson["accessors"][attr["POSITION"].get<std::string>()];
    size_t vCount = accPos["count"].get<size_t>();
    size_t posOff = accPos.value("byteOffset", 0);
    const glm::vec3* posPtr = GetBufferViewData<glm::vec3>(
        accPos["bufferView"].get<std::string>(),
        posOff, vCount, sizeof(glm::vec3));

    // Normals
    auto accNor   = m_SceneJson["accessors"][attr["NORMAL"].get<std::string>()];
    size_t norOff = accNor.value("byteOffset", 0);
    const glm::vec3* norPtr = GetBufferViewData<glm::vec3>(
        accNor["bufferView"].get<std::string>(),
        norOff, vCount, sizeof(glm::vec3));

    // Merge vertices
    size_t baseV = m_Result.positions.size();
    m_Result.positions.insert(m_Result.positions.end(), posPtr, posPtr + vCount);
    m_Result.normals.insert(m_Result.normals.end(), norPtr, norPtr + vCount);

    // Indices
    auto accIdx     = m_SceneJson["accessors"][primitive["indices"].get<std::string>()];
    size_t idxCount = accIdx["count"].get<size_t>();
    uint32_t cType   = accIdx["componentType"].get<uint32_t>();
    size_t idxOff    = accIdx.value("byteOffset", 0);
    size_t idxStride = (cType == 5123 ? 2 : cType == 5125 ? 4 : 1);

    const uint8_t* raw = GetBufferViewData<uint8_t>(
        accIdx["bufferView"].get<std::string>(),
        idxOff, idxCount, idxStride);

    std::vector<uint32_t> idxBuf;
    idxBuf.reserve(idxCount);
    for (size_t i = 0; i < idxCount; ++i) {
        const uint8_t* ptr = raw + i * idxStride;
        uint32_t v = 0;
        if (cType == 5123) {
            uint16_t tmp; std::memcpy(&tmp, ptr, 2);
            v = Mirror::Core::EndianUtils::FromLittleEndian(tmp);
        } else if (cType == 5125) {
            std::memcpy(&v, ptr, 4);
            v = Mirror::Core::EndianUtils::FromLittleEndian(v);
        } else {
            v = *ptr;
        }
        if (v >= vCount) {
            std::stringstream ss;
            ss << "Index out of bounds: v=" << v << ", vCount=" << vCount;
            throw std::runtime_error(ss.str());
        }
        idxBuf.push_back(v);
    }

    for (auto iv : idxBuf)
        m_Result.indices.push_back(baseV + iv);
}

// BufferView helper
template<typename T>
const T* GLTF1Parser::GetBufferViewData(
    const std::string& id,
    size_t accessorOffset,
    size_t count,
    size_t elementSize)
{
    const auto& bv = m_SceneJson["bufferViews"][id];
    size_t bvOff    = bv.value("byteOffset", 0);
    size_t totalOff = bvOff + accessorOffset;
    size_t stride   = bv.value("byteStride", 0);
    size_t required = (stride ? stride * (count - 1) + elementSize : elementSize * count);
    if (totalOff + required > m_BinaryChunkSize)
        throw std::runtime_error("BufferView overflow: " + id);

    const uint8_t* ptr = m_BinaryChunk + totalOff;
    if (stride && stride != elementSize) {
        m_StridedCache = ReadStridedData<uint8_t>(ptr, count, stride, elementSize);
        return reinterpret_cast<const T*>(m_StridedCache.data());
    }
    return reinterpret_cast<const T*>(ptr);
}

// Strided read helper
template<typename T>
std::vector<T> GLTF1Parser::ReadStridedData(
    const uint8_t* basePtr,
    size_t count,
    size_t stride,
    size_t elementSize)
{
    std::vector<T> out(count);
    for (size_t i = 0; i < count; ++i)
        std::memcpy(&out[i], basePtr + i * stride, elementSize);
    return out;
}

// Explicit instantiation
template const glm::vec3* GLTF1Parser::GetBufferViewData<glm::vec3>(const std::string&, size_t, size_t, size_t);
template const glm::vec2* GLTF1Parser::GetBufferViewData<glm::vec2>(const std::string&, size_t, size_t, size_t);
template const uint8_t*  GLTF1Parser::GetBufferViewData<uint8_t>(const std::string&, size_t, size_t, size_t);
template const uint16_t* GLTF1Parser::GetBufferViewData<uint16_t>(const std::string&, size_t, size_t, size_t);
template const uint32_t* GLTF1Parser::GetBufferViewData<uint32_t>(const std::string&, size_t, size_t, size_t);
