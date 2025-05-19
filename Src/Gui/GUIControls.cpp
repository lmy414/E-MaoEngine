// GUIControls.cpp
#include "GUIControls.h"
#include <glm/gtc/type_ptr.hpp>

void GUIControls::Render()
{
    // 引擎控制面板
    ImGui::Begin("Engine Controls");
    
    // 颜色控件参数传递
    if (ImGui::ColorEdit3("Triangle Color", glm::value_ptr(triangleColor))) // 使用glm::value_ptr
    {
        if (auto entity = targetEntity.lock()) {
            if (auto material = entity->GetMaterial<DefaultMaterial>()) {
                material->SetColor(triangleColor);
            }
        }
    }
    
    ImGui::ColorEdit3("Clear Color", glm::value_ptr(clearColor)); // 同样修正此处
    ImGui::End();

    //灯光控制
    ImGui::Begin("Light Settings");
    if (currentLight) {
        ImGui::DragFloat3("Direction", glm::value_ptr(currentLight->direction), 0.1f);
        ImGui::ColorEdit3("Color", glm::value_ptr(currentLight->color));
        ImGui::DragFloat("Intensity", &currentLight->intensity, 0.1f, 0.0f, 10.0f);
    }
    ImGui::End();

    // 模型变换面板
    ImGui::Begin("Model Transform");
    if (auto entity = targetEntity.lock()) {
        // 位置控制
        ImGui::SeparatorText("Position");
        if (ImGui::DragFloat3("##pos", glm::value_ptr(modelPosition), 0.1f)) {
            entity->transform->position = modelPosition;
            entity->transform->MarkDirty();
        }
        // 旋转控制（使用Degrees）
        ImGui::SeparatorText("Rotation");
        if (ImGui::DragFloat3("##rot", glm::value_ptr(modelRotation), 1.0f, -180, 180)) {
            glm::quat rot = glm::quat(glm::radians(modelRotation));
            entity->transform->rotation = rot;
            entity->transform->MarkDirty();
        }
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