// GUIControls.cpp
#include "GUIControls.h"
#include <glm/gtc/type_ptr.hpp>
void GUIControls::ResetModelTransform()
{
    modelPosition = glm::vec3(0.0f);
    modelRotation = glm::vec3(0.0f);
    modelScale = glm::vec3(1.0f);
    
    if (auto entity = targetEntity.lock()) {
        if (entity->transform) {
            entity->transform->position = modelPosition;
            entity->transform->rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
            entity->transform->scale = modelScale;
            entity->transform->MarkDirty();
        }
    }
}
void GUIControls::Render()
{
    // 引擎控制面板
    ImGui::Begin(U8("控制面板"));  
    
    // 颜色控件参数传递
    if (ImGui::ColorEdit3(U8("模型基础色"), glm::value_ptr(triangleColor))) 
    {
        if (auto entity = targetEntity.lock()) {
            if (auto material = entity->GetMaterial<DefaultMaterial>()) {
                material->SetColor(triangleColor);
            }
        }
    }
    
    ImGui::ColorEdit3(U8("背景颜色"), glm::value_ptr(clearColor)); 
    ImGui::End();
    // 灯光控制
    ImGui::Begin(U8("灯光设置"));  
    if (currentLight) {
        ImGui::DragFloat3(U8("方向"), glm::value_ptr(currentLight->direction), 0.1f); // 修改点5
        ImGui::ColorEdit3(U8("颜色"), glm::value_ptr(currentLight->color)); // 修改点6
        ImGui::DragFloat(U8("强度"), &currentLight->intensity, 0.1f, 0.0f, 10.0f); // 修改点7
    }
    ImGui::End();
    // 模型变换面板
    ImGui::Begin(U8("模型变换")); 
    if (auto entity = targetEntity.lock()) {

        // 新增模型信息显示 ---------------------------
        ImGui::SeparatorText(U8("模型信息"));
        if (entity->mesh && entity->mesh->IsReady()) {
            vertexCount = entity->mesh->GetVertices().size();
            triangleCount = entity->mesh->GetIndices().size() / 3;
            ImGui::Text(U8("顶点数: %zu"), vertexCount);
            ImGui::Text(U8("三角形数: %zu"), triangleCount);
        } else {
            ImGui::TextColored(ImVec4(1,0.3,0.3,1), U8("模型未就绪"));
        }
        // -------------------------------------------

        // 重置按钮组
        ImGui::SeparatorText(U8("变换控制")); 
        if (ImGui::Button(U8("完全重置"), ImVec2(100, 0))) { 
            ResetModelTransform();
        }
        ImGui::SameLine();
        
        // 组合重置按钮
        ImGui::BeginGroup();
        {
            if (ImGui::Button(U8("位置重置"))) { 
                modelPosition = glm::vec3(0.0f);
                entity->transform->position = modelPosition;
                entity->transform->MarkDirty();
            }
            ImGui::SameLine();
            if (ImGui::Button(U8("旋转重置"))) { 
                modelRotation = glm::vec3(0.0f);
                entity->transform->rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
                entity->transform->MarkDirty();
            }
            ImGui::SameLine();
            if (ImGui::Button(U8("缩放重置"))) { 
                modelScale = glm::vec3(1.0f);
                entity->transform->scale = modelScale;
                entity->transform->MarkDirty();
            }
        }
        ImGui::EndGroup();
        // 位置控制
        ImGui::SeparatorText(U8("位置")); 
        if (ImGui::DragFloat3("##pos", glm::value_ptr(modelPosition), 0.1f)) {
            entity->transform->position = modelPosition;
            entity->transform->MarkDirty();
        }
        // 旋转控制（使用Degrees）
        ImGui::SeparatorText(U8("旋转")); 
        if (ImGui::DragFloat3("##rot", glm::value_ptr(modelRotation), 1.0f, -180, 180)) {
            glm::quat rot = glm::quat(glm::radians(modelRotation));
            entity->transform->rotation = rot;
            entity->transform->MarkDirty();
        }
        ImGui::SeparatorText(U8("缩放")); 
        if (ImGui::DragFloat3("##scale", glm::value_ptr(modelScale), 0.1f, 0.0f, 10.0f, "%.1f"))
        {
            entity->transform->scale = modelScale;
            entity->transform->MarkDirty();
        }
        } else {
            ImGui::TextColored(ImVec4(1,0.3,0.3,1), U8("没有选择模型!"));
        }
        ImGui::End();
    
    ImGui::Begin(U8("相机控制")); 
    {
        ImGui::SeparatorText(U8("相机操作")); 
        
        // 重置相机按钮
        if (ImGui::Button(U8("重置相机"), ImVec2(100, 0))) { 
            if (onCameraReset) {
                ImGui::OpenPopup(U8("确认重置相机？")); 
            } else {
                ImGui::TextColored(ImVec4(1,0.3,0.3,1), U8("未设置相机控制器!")); 
            }
        }
        // 重置确认对话框
        if (ImGui::BeginPopupModal(U8("确认重置相机？"), nullptr,  
            ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar))
        {
            ImGui::Text(U8("确定要重置相机到默认位置吗？")); 
            ImGui::Separator();
            
            if (ImGui::Button(U8("确定"), ImVec2(120, 0))) {
                onCameraReset();
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button(U8("取消"), ImVec2(120, 0))) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
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
