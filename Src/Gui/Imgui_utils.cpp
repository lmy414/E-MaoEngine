#include "Imgui_utils.h"

void InitializeImGui(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    
    // 设置ImGui填充整个窗口
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 0.0f; // 去除窗口圆角
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f); // 透明背景

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void ShutdownImGui() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void BeginImGuiFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void RenderImGuiUI(const ImGuiContextWrapper& context) {
    // 视口窗口
    ImGui::Begin("Viewport", nullptr, 
        ImGuiWindowFlags_NoTitleBar | 
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize);

    // 获取可用区域并设置窗口位置
    ImVec2 viewportSize = ImGui::GetContentRegionAvail();
    ImGui::SetWindowPos(ImVec2(0, 0));
    ImGui::SetWindowSize(viewportSize);

    // 显示渲染纹理
    ImGui::Image(
        (ImTextureID)(intptr_t)context.fboTexture,
        viewportSize, 
        ImVec2(0, 1), 
        ImVec2(1, 0)
    );
    ImGui::End();

    // 右侧控制面板
    ImGui::Begin("Settings", nullptr, 
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_AlwaysAutoResize);
    {
        ImGui::ColorEdit3("Clear Color", context.clearColor);
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    }
    ImGui::End();
}



void EndImGuiFrame() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}