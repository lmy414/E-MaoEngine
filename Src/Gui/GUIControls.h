// GUIControls.h
#pragma once
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include "../../imgui/imgui.h"

class GUIControls {
public:
    glm::vec3 triangleColor = {1.0f, 0.0f, 0.0f}; // 默认红色
    glm::vec3 clearColor = {0.2f, 0.3f, 0.3f};    // 默认背景色
    
    // 新增帧缓冲相关参数
    GLuint framebufferTexture = 0;
    int fbWidth = 0;
    int fbHeight = 0;
    
    void Render();
    
    // 设置帧缓冲参数的方法
    void SetFramebufferInfo(GLuint texID, int width, int height) {
        framebufferTexture = texID;
        fbWidth = width;
        fbHeight = height;
    }
};
