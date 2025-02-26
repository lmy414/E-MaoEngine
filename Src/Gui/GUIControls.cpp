// GUIControls.cpp
#include "GUIControls.h"

void GUIControls::Render() {
    // 引擎控制面板
    ImGui::Begin("Engine Controls");
    
    ImGui::ColorEdit3("Triangle Color", &triangleColor[0]);
    ImGui::ColorEdit3("Clear Color", &clearColor[0]);
    
    ImGui::End();

    // 帧缓冲显示窗口
    if(framebufferTexture != 0) {
        ImGui::Begin("Frame Buffer View", nullptr, 
                    ImGuiWindowFlags_NoScrollbar | 
                    ImGuiWindowFlags_NoScrollWithMouse);
        
        // 自动调整显示尺寸
        ImVec2 contentSize = ImGui::GetContentRegionAvail();
        float aspect = (float)fbWidth / (float)fbHeight;
        float displayHeight = contentSize.x / aspect;
        
        if(displayHeight > contentSize.y) {
            displayHeight = contentSize.y;
            contentSize.x = displayHeight * aspect;
        }
        
        // 显示纹理（翻转Y轴）
        ImGui::Image(
        (ImTextureID)(intptr_t)framebufferTexture,
            ImVec2(contentSize.x, displayHeight),
            ImVec2(0, 1),  // UV起点（左下）
            ImVec2(1, 0)   // UV终点（右上）
        );
        
        ImGui::End();
    }
}
