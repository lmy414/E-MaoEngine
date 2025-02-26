// GUIControls.h
#pragma once
#include <glm/glm.hpp>

class GUIControls {
public:
    glm::vec3 triangleColor = {1.0f, 0.0f, 0.0f}; // 默认红色
    glm::vec3 clearColor = {0.2f, 0.3f, 0.3f};    // 默认背景色
    
    void Render();
};
