#pragma once
#include "../../imgui/imgui.h" 
#include "../../imgui/imgui_impl_glfw.h"
#include "../../imgui/imgui_impl_opengl3.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

struct ImGuiContextWrapper {
    GLuint* fboTexture;
    float* clearColor;
};

void InitializeImGui(GLFWwindow* window);
void ShutdownImGui();
void BeginImGuiFrame();
void RenderImGuiUI(const ImGuiContextWrapper& context);
void EndImGuiFrame();