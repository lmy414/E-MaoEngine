// ImGuiManager.h
#pragma once
#include "../../imgui/imgui.h"
#include "../../imgui/imgui_impl_glfw.h"
#include "../../imgui/imgui_impl_opengl3.h"
//#include <GLFW/glfw3.h>   // 跨平台窗口管理库

class ImGuiManager {
public:
    static void Init(GLFWwindow* window);
    static void Shutdown();
    static void BeginFrame();
    static void EndFrame();
};
