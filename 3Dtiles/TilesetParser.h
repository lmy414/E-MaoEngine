// TilesetParser.h
#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <json.hpp>
#include <unordered_set>  // 添加这个头文件


namespace fs = std::filesystem;

class TilesetParser {
public:
    static std::vector<std::string> GetB3DMPaths(const std::string& rootTilesetPath);

private:
    static void ParseNode(const nlohmann::json& node, 
                        const fs::path& basePath,
                        std::vector<std::string>& result,
                        std::unordered_set<std::string>& processedFiles);
    
    static void ProcessContentUrl(const nlohmann::json& content,
                                const fs::path& basePath,
                                std::vector<std::string>& result,
                                std::unordered_set<std::string>& processedFiles);
};
