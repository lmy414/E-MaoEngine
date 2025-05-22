// TilesetParser.cpp
#include "TilesetParser.h"
#include <iostream>
#include <unordered_set>  // 添加这个头文件


using json = nlohmann::json;

std::vector<std::string> TilesetParser::GetB3DMPaths(const std::string& tilesetPath) {
    std::ifstream file(tilesetPath);
    if (!file) throw std::runtime_error("无法打开 tileset.json: " + tilesetPath);

    nlohmann::json tilesetJson;
    file >> tilesetJson;

    std::vector<std::string> results;

    std::function<void(const nlohmann::json&)> traverse;
    traverse = [&](const nlohmann::json& node) {
        if (node.contains("content") && node["content"].contains("uri")) {
            fs::path contentPath = fs::path(tilesetPath).parent_path() / node["content"]["uri"].get<std::string>();
            results.push_back(contentPath.string());  // ✅ 转换为 string
        }

        if (node.contains("children")) {
            for (const auto& child : node["children"]) {
                traverse(child);
            }
        }
    };

    if (tilesetJson.contains("root")) {
        traverse(tilesetJson["root"]);
    }

    return results;
}



void TilesetParser::ParseNode(const json& node,
                            const fs::path& basePath,
                            std::vector<std::string>& result,
                            std::unordered_set<std::string>& processedFiles) {
    // 处理当前节点的content
    if (node.contains("content")) {
        ProcessContentUrl(node["content"], basePath, result, processedFiles);
    }

    // 递归处理子节点
    if (node.contains("children")) {
        for (const auto& child : node["children"]) {
            ParseNode(child, basePath, result, processedFiles);
        }
    }
}

void TilesetParser::ProcessContentUrl(const json& content,
                                    const fs::path& basePath,
                                    std::vector<std::string>& result,
                                    std::unordered_set<std::string>& processedFiles) {
    if (!content.contains("url")) return;

    const std::string relativeUrl = content["url"].get<std::string>();
    fs::path fullPath = (basePath / relativeUrl).lexically_normal();

    try {
        fullPath = fs::canonical(fullPath);
        
        // 检查文件是否已处理过
        if (processedFiles.count(fullPath.string())) return;
        processedFiles.insert(fullPath.string());

        const std::string extension = fullPath.extension().string();

        if (extension == ".b3dm"||extension == ".glb") {
            result.push_back(fullPath.string());
            std::cout << "[Info] Found B3DM: " << fullPath << std::endl;
        } else if (extension == ".json") {
            // 递归处理嵌套的tileset
            json childData;
            {
                std::ifstream file(fullPath);
                if (!file.is_open()) throw std::runtime_error("Failed to open nested tileset");
                file >> childData;
            }
            
            const fs::path newBasePath = fullPath.parent_path();
            if (childData.contains("root")) {
                ParseNode(childData["root"], newBasePath, result, processedFiles);
            } else {
                ParseNode(childData, newBasePath, result, processedFiles);
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "[TilesetParser] Error processing " << fullPath 
                 << ": " << e.what() << std::endl;
    }
}
