// ImGuiManager.cpp
#include "ImGuiManager.h"

void ImGuiManager::Init(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    
    // ���÷��
    ImGui::StyleColorsDark();
    io.Fonts->AddFontFromFileTTF("c:/windows/Fonts/simhei.ttf", 13.0f, NULL, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
    // ƽ̨/��Ⱦ����˳�ʼ��
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void ImGuiManager::Shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
}

void ImGuiManager::BeginFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
}

void ImGuiManager::EndFrame() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
