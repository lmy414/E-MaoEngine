// Geometry.h
#pragma once
#include "../Src/Render/Mesh.h"

class Geometry {
public:
    virtual ~Geometry() = default;
    virtual Mesh CreateMesh() const = 0;
};
