// Triangle.h
#pragma once
#include "Geometry.h"

class Triangle : public Geometry {
public:
    Mesh CreateMesh() const override {
        std::vector<Vertex> vertices = {
            {glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f)},
            {glm::vec3( 0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f)},
            {glm::vec3( 0.0f,  0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.5f, 1.0f)}
        };

        std::vector<unsigned int> indices = {0, 1, 2};

        return Mesh(vertices, indices);
    }
};
