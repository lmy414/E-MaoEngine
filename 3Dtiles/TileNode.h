// TileNode.h
#pragma once
#include <string>
#include <vector>
#include <memory>

struct TileNode {
    std::string name;
    std::string path;
    double geometricError = 0.0;  // <== 新增
    std::vector<std::shared_ptr<TileNode>> children;
};
