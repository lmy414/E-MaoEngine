// TilesetParser.h
#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <json.hpp>
#include <unordered_set>
#include "TileNode.h"

namespace fs = std::filesystem;

class TilesetParser {
public:
    static std::vector<std::string> GetB3DMPaths(const std::string& rootTilesetPath);

    // 新增接口：构建 TileNode 树
    static std::shared_ptr<TileNode> BuildTileTree(const std::string& rootTilesetPath);

private:
    static void ParseNode(const nlohmann::json& node, 
                        const fs::path& basePath,
                        std::vector<std::string>& result,
                        std::unordered_set<std::string>& processedFiles);

    static void ProcessContentUrl(const nlohmann::json& content,
                                const fs::path& basePath,
                                std::vector<std::string>& result,
                                std::unordered_set<std::string>& processedFiles);

    // 新增函数：用于构建树
    static std::shared_ptr<TileNode> ParseTreeRecursive(
        const nlohmann::json& node,
        const fs::path& basePath,
        std::unordered_set<std::string>& processedFiles
    );
};
