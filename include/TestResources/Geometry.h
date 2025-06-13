// Geometry.h
#pragma once
#include "Render/Mesh.h"

class Geometry {
public:
    virtual ~Geometry() = default;
    virtual Mesh CreateMesh() const = 0; // 纯虚函数
};
