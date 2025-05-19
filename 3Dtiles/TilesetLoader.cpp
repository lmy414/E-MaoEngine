// TilesetLoader.cpp
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include <glm/glm.hpp>
#include "TilesetLoader.h"
#include <json.hpp>

using json = nlohmann::json;
namespace fs = std::filesystem;

namespace {
    // 缓存已加载的tileset（使用weak_ptr避免内存泄漏）
    std::unordered_map<std::string, std::weak_ptr<TileNode>> tileCache;
    
    // 最大递归深度保护
    constexpr int MAX_RECURSION_DEPTH = 16;

    std::string resolveUri(const std::string& basePath, const std::string& uri) {
        try {
            fs::path base(basePath);
            fs::path relative(uri);
            
            // 处理URI转义字符
            std::string decoded = uri;
            size_t pos = 0;
            while ((pos = decoded.find("%20")) != std::string::npos) {
                decoded.replace(pos, 3, " ");
            }
            
            return (base / decoded).lexically_normal().string();
        } catch (const fs::filesystem_error& e) {
            throw std::runtime_error("路径解析失败: " + std::string(e.what()));
        }
    }

    std::unique_ptr<TileNode> parseTile(const json& tileJson, 
                                      const std::string& basePath,
                                      int currentDepth);
}

namespace TilesetParser {
    // 前向声明
    std::unique_ptr<TileNode> loadTilesetImpl(const std::string& filepath, int currentDepth);

    // 内部实现方法
    std::unique_ptr<TileNode> loadTilesetImpl(const std::string& filepath, int currentDepth) {
        // 检查缓存（使用weak_ptr解决循环引用）
        if (auto it = tileCache.find(filepath); it != tileCache.end()) {
            if (auto cached = it->second.lock()) {
                return cached->clone(); // 返回深拷贝
            }
        }

        // 加载并解析JSON
        std::ifstream fin(filepath);
        if (!fin.is_open()) {
            throw std::runtime_error("无法打开文件: " + filepath);
        }
        
        json data;
        try {
            data = json::parse(fin);
        } catch (const json::parse_error& e) {
            throw std::runtime_error("JSON解析失败: " + std::string(e.what()));
        }

        auto root = std::make_unique<TileNode>();
        const auto& rootJson = data["root"];
        
        // 解析根节点
        const auto& rootSphere = rootJson["boundingVolume"]["sphere"];
        root->volume = {
            { rootSphere[0].get<double>(), rootSphere[1].get<double>(), rootSphere[2].get<double>() },
            rootSphere[3].get<double>()
        };
        root->geometricError = rootJson["geometricError"].get<double>();

        // 获取基础路径
        const std::string basePath = fs::path(filepath).parent_path().string();

        // 解析子节点
        if (rootJson.contains("children")) {
            for (const auto& child : rootJson["children"]) {
                root->children.push_back(parseTile(child, basePath, currentDepth));
            }
        }

        // 加入缓存
        auto sharedRoot = std::shared_ptr<TileNode>(root.release());
        tileCache[filepath] = sharedRoot;
        return sharedRoot->clone();
    }

    // 公开接口
    std::unique_ptr<TileNode> loadTileset(const std::string& filepath) {
        return loadTilesetImpl(filepath, 0);
    }

    void PrintDebugInfo(const TileNode* node, int depth) {
        const std::string indent(depth * 2, ' ');
        
        // 保存原始格式状态
        std::ios::fmtflags originalFlags = std::cout.flags();
        
        // 打印节点信息
        std::cout << indent << "┌── Tile Node [" << depth << "]\n"
                << indent << "│   Geometric Error: " 
                << std::fixed << std::setprecision(3) << node->geometricError << "\n"
                << indent << "│   Bounding Sphere: \n"
                << indent << "│     Center: ("
                << std::setw(12) << std::setprecision(2) << node->volume.center.x << ", "
                << std::setw(12) << node->volume.center.y << ", "
                << std::setw(12) << node->volume.center.z << ")\n"
                << indent << "│     Radius: " 
                << std::setw(12) << std::setprecision(2) << node->volume.radius << "\n";

        if (!node->contentUri.empty()) {
            std::cout << indent << "│   Content URI: " << node->contentUri << "\n";
        }

        std::cout << indent << "└── Children: " << node->children.size()
                << (node->children.empty() ? "" : " ▼") << "\n";

        // 恢复格式状态
        std::cout.flags(originalFlags);

        // 递归打印
        for (const auto& child : node->children) {
            PrintDebugInfo(child.get(), depth + 1);
        }
    }

    void ClearCache() {
        tileCache.clear();
    }
}

namespace {
    std::unique_ptr<TileNode> parseTile(const json& tileJson, 
                                      const std::string& basePath,
                                      int currentDepth) {
        auto node = std::make_unique<TileNode>();
        
        // 解析基础字段
        const auto& sphere = tileJson["boundingVolume"]["sphere"];
        node->volume = {
            { sphere[0].get<double>(), sphere[1].get<double>(), sphere[2].get<double>() },
            sphere[3].get<double>()
        };
        node->geometricError = tileJson["geometricError"].get<double>();

        // 处理内容URI
        if (tileJson.contains("content")) {
            const auto& content = tileJson["content"];
            std::string uri = content["url"].get<std::string>();
            node->contentUri = resolveUri(basePath, uri);
            
            // 检测并加载外链JSON
            if (uri.find(".json") != std::string::npos) {
                try {
                    if (currentDepth >= MAX_RECURSION_DEPTH) {
                        throw std::runtime_error("达到最大递归深度");
                    }
                    
                    auto external = TilesetParser::loadTilesetImpl(node->contentUri, currentDepth + 1);
                    if (external) {
                        // 移动子节点所有权
                        node->children = std::move(external->children);
                    }
                } catch (const std::exception& e) {
                    std::cerr << "外链加载失败: " << node->contentUri 
                            << " - " << e.what() << std::endl;
                }
            }
        }

        // 递归解析子节点
        if (tileJson.contains("children")) {
            for (const auto& child : tileJson["children"]) {
                node->children.push_back(parseTile(child, basePath, currentDepth));
            }
        }

        return node;
    }
}
