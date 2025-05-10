// GUIControls.cpp
#include "GUIControls.h"

void GUIControls::Render() {
    // 引擎控制面板
    ImGui::Begin("Engine Controls");
    
    ImGui::ColorEdit3("Triangle Color", &triangleColor[0]);
    ImGui::ColorEdit3("Clear Color", &clearColor[0]);
    
    ImGui::End();
    ImGui::Begin("Model Transform");
    
    if (auto entity = targetEntity.lock()) {
        // 位置控制
        ImGui::SeparatorText("Position");
        ImGui::PushItemWidth(120);
        if (ImGui::DragFloat3("##pos", &modelPosition.x, 0.1f)) {
            entity->transform->position = modelPosition;
            entity->transform->MarkDirty();
        }
        // 旋转控制（使用Degrees）
        ImGui::SeparatorText("Rotation");
        if (ImGui::DragFloat3("##rot", &modelRotation.x, 1.0f, -180, 180)) {
            // 转四元数时需要把角度转成弧度
            glm::quat rot = glm::quat(glm::radians(modelRotation));
            entity->transform->rotation = rot;
            entity->transform->MarkDirty();
        }
        // 缩放控制（依需求添加）
        // glm::vec3 scale = entity->transform->scale;
        // if (ImGui::DragFloat3("Scale", &scale.x, 0.1f)) {
        //     entity->transform->scale = scale;
        //     entity->transform->MarkDirty();
        // }
    } else {
        ImGui::TextColored(ImVec4(1,0.3,0.3,1), "没有选择模型!");
    }
    
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
