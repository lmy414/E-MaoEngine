#include "TilesetParser.h"
#include <iostream>
#include <unordered_set>
#include <functional>

using json = nlohmann::json;

std::vector<std::string> TilesetParser::GetB3DMPaths(const std::string& tilesetPath) {
    std::ifstream file(tilesetPath);
    if (!file) throw std::runtime_error("无法打开 tileset.json: " + tilesetPath);

    json tilesetJson;
    file >> tilesetJson;

    std::vector<std::string> results;
    std::unordered_set<std::string> processedFiles;
    fs::path basePath = fs::path(tilesetPath).parent_path();

    if (tilesetJson.contains("root")) {
        ParseNode(tilesetJson["root"], basePath, results, processedFiles);
    }

    return results;
}

void TilesetParser::ParseNode(const json& node,
                              const fs::path& basePath,
                              std::vector<std::string>& result,
                              std::unordered_set<std::string>& processedFiles) {
    // 处理当前节点的 content
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
    std::string relativeUrl;
    if (content.contains("uri")) {
        relativeUrl = content["uri"].get<std::string>();
    } else if (content.contains("url")) {
        relativeUrl = content["url"].get<std::string>();
    } else {
        return;
    }

    fs::path fullPath = (basePath / relativeUrl).lexically_normal();

    try {
        fullPath = fs::canonical(fullPath);

        // 检查是否已处理
        if (processedFiles.count(fullPath.string())) return;
        processedFiles.insert(fullPath.string());

        const std::string extension = fullPath.extension().string();

        if (extension == ".b3dm" || extension == ".glb") {
            result.push_back(fullPath.string());
            std::cout << "[Info] Found B3DM: " << fullPath << std::endl;
        } else if (extension == ".json") {
            // 嵌套 tileset
            std::ifstream file(fullPath);
            if (!file.is_open()) throw std::runtime_error("无法打开嵌套 tileset");

            json childData;
            file >> childData;

            const auto& childRoot = childData.contains("root") ? childData["root"] : childData;
            ParseNode(childRoot, fullPath.parent_path(), result, processedFiles);
        }
    } catch (const std::exception& e) {
        std::cerr << "[TilesetParser] Error processing " << fullPath << ": " << e.what() << std::endl;
    }
}

std::shared_ptr<TileNode> TilesetParser::BuildTileTree(const std::string& rootTilesetPath) {
    std::ifstream file(rootTilesetPath);
    if (!file) throw std::runtime_error("无法打开 tileset.json: " + rootTilesetPath);

    json tilesetJson;
    file >> tilesetJson;

    std::unordered_set<std::string> processedFiles;
    fs::path basePath = fs::path(rootTilesetPath).parent_path();

    if (tilesetJson.contains("root")) {
        return ParseTreeRecursive(tilesetJson["root"], basePath, processedFiles);
    }

    return nullptr;
}

std::shared_ptr<TileNode> TilesetParser::ParseTreeRecursive(
    const json& node,
    const fs::path& basePath,
    std::unordered_set<std::string>& processedFiles
) {
    auto tile = std::make_shared<TileNode>();
    if (node.contains("geometricError") && node["geometricError"].is_number()) {
        tile->geometricError = node["geometricError"].get<double>();
    }

    std::string uri;
    if (node.contains("content")) {
        if (node["content"].contains("uri")) uri = node["content"]["uri"].get<std::string>();
        else if (node["content"].contains("url")) uri = node["content"]["url"].get<std::string>();
    }

    fs::path fullPath = (basePath / uri).lexically_normal();
    tile->name = uri.empty() ? "Unnamed Tile" : uri;
    tile->path = fullPath.string();

    // 嵌套 json tileset 处理
    if (!uri.empty()) {
        const std::string ext = fullPath.extension().string();
        if (ext == ".json" && !processedFiles.count(fullPath.string())) {
            processedFiles.insert(fullPath.string());

            try {
                std::ifstream subFile(fullPath);
                if (subFile.is_open()) {
                    json childJson;
                    subFile >> childJson;

                    const auto& childRoot = childJson.contains("root") ? childJson["root"] : childJson;
                    auto childNode = ParseTreeRecursive(childRoot, fullPath.parent_path(), processedFiles);
                    if (childNode) tile->children.push_back(childNode);
                }
            } catch (...) {
                std::cerr << "[Error] Failed to parse nested tileset: " << fullPath << std::endl;
            }
        }
    }

    // 普通子节点处理
    if (node.contains("children")) {
        for (const auto& child : node["children"]) {
            auto childNode = ParseTreeRecursive(child, basePath, processedFiles);
            if (childNode) tile->children.push_back(childNode);
        }
    }

    return tile;
}
