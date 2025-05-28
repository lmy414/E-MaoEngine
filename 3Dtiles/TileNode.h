// TileNode.h
#pragma once
#include <string>
#include <vector>
#include <memory>
#include <glm/glm.hpp>

struct TileNode {
    std::string name;
    std::string path;
    double geometricError = 0.0;  // <== 新增
    
    // 新增字段
    glm::mat4 transform = glm::mat4(1.0f);  // 变换矩阵
    struct {
        glm::vec3 center = glm::vec3(0.0f);  // 包围盒中心
        glm::vec3 halfSize = glm::vec3(0.0f); // 包围盒半尺寸
    } boundingVolume;
    
    struct {
        double minimumPixelSize = 0.0;      // 最小像素尺寸
        bool additive = false;              // 是否为加法渲染
        std::string refinement = "REPLACE"; // 细化模式
    } refine;
    
    struct {
        std::string uri;                    // 内容URI
        std::string format = "B3DM";        // 内容格式
        double byteLength = 0.0;            // 字节长度
    } content;
    
    std::vector<std::shared_ptr<TileNode>> children;
    
    // 辅助函数：获取格式化的路径（处理过长的路径）
    std::string GetFormattedPath() const {
        const size_t maxLength = 50;
        if (path.length() <= maxLength) return path;
        
        size_t start = path.find_last_of("\\/", path.length() - maxLength);
        if (start == std::string::npos) return "..." + path.substr(path.length() - maxLength);
        return "..." + path.substr(start);
    }
};
