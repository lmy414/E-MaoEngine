// B3DMLoader.h
#pragma once
#include "GLBParser.h"
#include "../Src/Render/Mesh.h"
#include <fstream>

class B3DMLoader {
public:
    static Mesh LoadFromFile(const std::string& path) {
        std::ifstream file(path, std::ios::binary);
        if (!file) throw std::runtime_error("无法打开文件: " + path);

        // 读取文件头
        struct Header {
            char magic[4];
            uint32_t version;
            uint32_t byteLength;
        } header;
        file.read(reinterpret_cast<char*>(&header), sizeof(Header));

        // 验证文件格式
        if (std::string(header.magic, 4) != "b3dm") {
            throw std::runtime_error("无效的B3DM文件头");
        }

        // 读取GLB数据
        std::vector<uint8_t> glbData(header.byteLength - sizeof(Header));
        file.read(reinterpret_cast<char*>(glbData.data()), glbData.size());

        // 使用GLBParser解析
        auto glbResult = GLBParser::Parse(glbData);
        
        // 创建网格（使用移动语义）
        return Mesh(
            std::move(glbResult.vertices),
            std::move(glbResult.indices)
        );
    }
};
