// B3DMLoader.h
#pragma once
#include <vector>
#include <string>
#include <fstream>
#include "Core/EndianUtils.h"
#include "GLTF1Parser.h"
#include "GLBParser.h"

class B3DMLoader {
public:
    #pragma pack(push, 1)
    struct PackedB3DMHeader {
        char magic[4];
        uint8_t version[4];
        uint8_t byteLength[4];
        uint8_t ftJSONLen[4];
        uint8_t ftBinLen[4];
        uint8_t btJSONLen[4];
        uint8_t btBinLen[4];
    };
    #pragma pack(pop)

    struct B3DMHeader {
        char magic[4];
        uint32_t version;
        uint32_t byteLength;
        uint32_t ftJSONLen;
        uint32_t ftBinLen;
        uint32_t btJSONLen;
        uint32_t btBinLen;
    };

    static Mesh LoadFromFile(const std::string& path) {
        std::ifstream file(path, std::ios::binary);
        if (!file) throw std::runtime_error("Failed to open file");

        // 读取并转换头部
        PackedB3DMHeader packedHeader;
        file.read(reinterpret_cast<char*>(&packedHeader), sizeof(PackedB3DMHeader));

        B3DMHeader header;
        auto readU32 = [](const uint8_t* bytes) {
            return Mirror::Core::EndianUtils::FromLittleEndian(
                *reinterpret_cast<const uint32_t*>(bytes));
        };

        memcpy(header.magic, packedHeader.magic, 4);
        header.version       = readU32(packedHeader.version);
        header.byteLength   = readU32(packedHeader.byteLength);
        header.ftJSONLen    = readU32(packedHeader.ftJSONLen);
        header.ftBinLen     = readU32(packedHeader.ftBinLen);
        header.btJSONLen    = readU32(packedHeader.btJSONLen);
        header.btBinLen     = readU32(packedHeader.btBinLen);

        // 验证魔术字
        if (std::string_view(header.magic, 4) != "b3dm") {
            throw std::runtime_error("Invalid B3DM magic");
        }

        // 计算GLB偏移
        auto calcPaddedSize = [](uint32_t size) { 
            return (size + 3) & ~3;
        };

        const size_t glbStartOffset = sizeof(PackedB3DMHeader)
            + calcPaddedSize(header.ftJSONLen)
            + calcPaddedSize(header.ftBinLen)
            + calcPaddedSize(header.btJSONLen)
            + calcPaddedSize(header.btBinLen);

        // 定位并读取GLB
        file.seekg(glbStartOffset);
        if (file.fail()) {
            throw std::runtime_error("Invalid GLB offset");
        }

        // 读取GLB头
        Mirror::GLTF::GLTF1Parser::GLBHeader glbHeader;
        file.read(reinterpret_cast<char*>(&glbHeader), sizeof( Mirror::GLTF::GLTF1Parser::GLTF1Parser::GLBHeader));

        // 验证GLB
        if (std::string_view(glbHeader.magic, 4) != "glTF") {
            throw std::runtime_error("Invalid GLB magic");
        }

        const uint32_t glbVersion = Mirror::Core::EndianUtils::FromLittleEndian(glbHeader.version);
        const uint32_t glbLength = Mirror::Core::EndianUtils::FromLittleEndian(glbHeader.fileLength);

        
        std::cout << "[B3DM] Header Analysis:\n"
                  << "Magic: " << std::string(header.magic, 4) << "\n"
                  << "Version: " << header.version << "\n"
                  << "Total Length: " << header.byteLength << "\n"
                  << "FeatureTableJSON: " << header.ftJSONLen << " (padded: " 
                  << calcPaddedSize(header.ftJSONLen) << ")\n"
                  << "FeatureTableBinary: " << header.ftBinLen << " (padded: " 
                  << calcPaddedSize(header.ftBinLen) << ")\n"
                  << "BatchTableJSON: " << header.btJSONLen << " (padded: " 
                  << calcPaddedSize(header.btJSONLen) << ")\n"
                  << "BatchTableBinary: " << header.btBinLen << " (padded: " 
                  << calcPaddedSize(header.btBinLen) << ")\n"
                  << "Calculated GLB Start Offset: " << glbStartOffset << "\n";
                  //<< "File Size: " << fileSize << "\n";


        // 读取完整GLB数据
        std::vector<uint8_t> glbData(glbLength);
        file.seekg(glbStartOffset);
        file.read(reinterpret_cast<char*>(glbData.data()), glbLength);

        try {
            // 修改后的GLB版本处理
            if (glbVersion == 1) {
                return Mirror::GLTF::GLTF1Parser::Parse(glbData).ToMesh();
            } else if (glbVersion == 2) {
                return Mirror::GLTF::GLBParser::Parse(glbData).ToMesh(); // 确保GLBParser存在
            } else {
                throw std::runtime_error("Unsupported GLB version: " + std::to_string(glbVersion));
            }
        } catch (const std::exception& e) {
            std::ofstream("debug.glb", std::ios::binary)
                .write(reinterpret_cast<const char*>(glbData.data()), glbData.size());
            throw;
           
        }
    }
    
};
