#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cstring>
#include "Core/EndianUtils.h"
#include <iostream> 
#include "GLTF1Parser.h"
#include <filesystem>
#include "GLBParser.h"  // Assume similar interface for glTF2

namespace fs = std::filesystem;
class B3DMLoader
{
public:
#pragma pack(push, custom_alignment)
    struct PackedB3DMHeader {
        char     magic[4];
        uint8_t  version[4];
        uint8_t  byteLength[4];
        uint8_t  ftJSONLen[4];
        uint8_t  ftBinLen[4];
        uint8_t  btJSONLen[4];
        uint8_t  btBinLen[4];
    };
#pragma pack(pop)

    struct B3DMHeader {
        char     magic[4];
        uint32_t version;
        uint32_t byteLength;
        uint32_t ftJSONLen;
        uint32_t ftBinLen;
        uint32_t btJSONLen;
        uint32_t btBinLen;
    };

    static Mesh LoadFromFile(const std::string& path) {
        std::cout << "开始加载模型文件: " << path << std::endl;

        std::ifstream file(path, std::ios::binary);
        if (!file) throw std::runtime_error("无法打开文件: " + path);

        std::string ext = fs::path(path).extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

        if (ext == ".glb") {
            std::cout << " 识别为 GLB 文件，准备直接解析..." << std::endl;

            file.seekg(0, std::ios::end);
            size_t fileSize = file.tellg();
            file.seekg(0, std::ios::beg);

            std::vector<uint8_t> glbData(fileSize);
            file.read(reinterpret_cast<char*>(glbData.data()), fileSize);
            if (!file) throw std::runtime_error("无法完整读取GLB数据");

            std::cout << "读取GLB数据完毕，共 " << fileSize << " 字节" << std::endl;

            Mirror::GLTF::GLTF1Parser::GLBHeader glbHeader;
            std::memcpy(&glbHeader, glbData.data(), sizeof(glbHeader));

            if (std::string_view(glbHeader.magic, 4) != "glTF")
                throw std::runtime_error("无效的 GLB 魔数");

            uint32_t glbVersion = Mirror::Core::EndianUtils::FromLittleEndian(glbHeader.version);
            std::cout << "GLB版本: " << glbVersion << std::endl;

            if (glbVersion == 1) {
                std::cout << "使用 GLTF 1.0 解析器..." << std::endl;
                return Mirror::GLTF::GLTF1Parser::Parse(glbData).ToMesh();
            } else if (glbVersion == 2) {
                std::cout << "使用 GLTF 2.0 解析器..." << std::endl;
                return Mirror::GLTF::GLBParser::Parse(glbData).ToMesh();
            } else {
                throw std::runtime_error("不支持的GLB版本: " + std::to_string(glbVersion));
            }
        }

        // === 默认处理 .b3dm 文件 ===
        std::cout << "识别为 B3DM 文件，准备解析..." << std::endl;

        PackedB3DMHeader packed;
        file.read(reinterpret_cast<char*>(&packed), sizeof(packed));
        if (!file) throw std::runtime_error("无法读取 B3DM Header");

        auto readU32 = [&](const uint8_t* bytes) {
            uint32_t v;
            std::memcpy(&v, bytes, sizeof(v));
            return Mirror::Core::EndianUtils::FromLittleEndian(v);
        };

        B3DMHeader hdr;
        std::memcpy(hdr.magic, packed.magic, 4);
        hdr.version    = readU32(packed.version);
        hdr.byteLength = readU32(packed.byteLength);
        hdr.ftJSONLen  = readU32(packed.ftJSONLen);
        hdr.ftBinLen   = readU32(packed.ftBinLen);
        hdr.btJSONLen  = readU32(packed.btJSONLen);
        hdr.btBinLen   = readU32(packed.btBinLen);

        if (std::string_view(hdr.magic, 4) != "b3dm")
            throw std::runtime_error("无效的B3DM魔数");

        auto pad4 = [](uint32_t n){ return (n + 3) & ~3u; };
        size_t offset = sizeof(PackedB3DMHeader)
                      + pad4(hdr.ftJSONLen)
                      + pad4(hdr.ftBinLen)
                      + pad4(hdr.btJSONLen)
                      + pad4(hdr.btBinLen);

        std::cout << "B3DM头部信息解析:\n"
                  << " - 版本: " << hdr.version << "\n"
                  << " - 总长度: " << hdr.byteLength << "\n"
                  << " - FeatureTable JSON 长度: " << hdr.ftJSONLen << "（对齐后: " << pad4(hdr.ftJSONLen) << "）\n"
                  << " - FeatureTable Binary 长度: " << hdr.ftBinLen << "（对齐后: " << pad4(hdr.ftBinLen) << "）\n"
                  << " - BatchTable JSON 长度: " << hdr.btJSONLen << "（对齐后: " << pad4(hdr.btJSONLen) << "）\n"
                  << " - BatchTable Binary 长度: " << hdr.btBinLen << "（对齐后: " << pad4(hdr.btBinLen) << "）\n"
                  << " - 计算得到的GLB偏移: " << offset << "\n";

        file.seekg(offset);
        std::vector<uint8_t> glbData(hdr.byteLength - offset);
        file.read(reinterpret_cast<char*>(glbData.data()), glbData.size());
        if (!file) throw std::runtime_error("无法读取嵌入的GLB数据");

        std::cout << "嵌入GLB数据读取成功，大小: " << glbData.size() << " 字节" << std::endl;

        Mirror::GLTF::GLTF1Parser::GLBHeader glbHeader;
        std::memcpy(&glbHeader, glbData.data(), sizeof(glbHeader));
        if (std::string_view(glbHeader.magic, 4) != "glTF")
            throw std::runtime_error("嵌入GLB魔数无效");

        uint32_t glbVersion = Mirror::Core::EndianUtils::FromLittleEndian(glbHeader.version);
        std::cout << "嵌入GLB版本: " << glbVersion << std::endl;

        if (glbVersion == 1) {
            std::cout << "使用 GLTF 1.0 解析器解析嵌入GLB..." << std::endl;
            return Mirror::GLTF::GLTF1Parser::Parse(glbData).ToMesh();
        } else if (glbVersion == 2) {
            std::cout << "使用 GLTF 2.0 解析器解析嵌入GLB..." << std::endl;
            return Mirror::GLTF::GLBParser::Parse(glbData).ToMesh();
        } else {
            throw std::runtime_error("不支持的嵌入GLB版本: " + std::to_string(glbVersion));
        }
    }
};
