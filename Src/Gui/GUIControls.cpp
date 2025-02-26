#include "GUIControls.h"
#include "../../imgui/imgui.h"

void GUIControls::Render() {
    ImGui::Begin("Engine Controls");
    
    // 三角形颜色控件
    ImGui::ColorEdit3("Triangle Color", &triangleColor[0]);
    
    // 背景颜色控件
    ImGui::ColorEdit3("Clear Color", &clearColor[0]);
    
    ImGui::End();
}
