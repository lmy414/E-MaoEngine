// ImGuiManager.h
#pragma once
//#include <GLFW/glfw3.h>
#define IMGUI_ENABLE_DOCKING
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
//#include "../Render/Light/Light.h"


class ImGuiManager {
public:
    static void Init(GLFWwindow* window);
    static void Shutdown();
    static void BeginFrame();
    static void EndFrame();
};
