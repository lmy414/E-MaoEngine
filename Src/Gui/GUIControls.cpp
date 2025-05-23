// GUIControls.cpp
#include "GUIControls.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <filesystem>
#include <fstream>
#include <algorithm>

void GUIControls::SetTargetEntity(const std::shared_ptr<Entity>& entity) {
    targetEntity = entity;
    if (entity) {
        if (entity->transform) {
            modelPosition = entity->transform->position;
            modelRotation = glm::degrees(glm::eulerAngles(entity->transform->rotation));
            modelScale = entity->transform->scale;
        }
        lodController = entity->lodController;
        if (lodController) {
            lodParams = lodController->GetParameters();
            lodRatio = 1.0f;
        } else {
            lodParams = ProgressiveLOD::Parameters{};
            lodRatio = 1.0f;
        }
    } else {
        lodController.reset();
        lodParams = ProgressiveLOD::Parameters{};
        lodRatio = 1.0f;
    }
}

void GUIControls::ResetModelTransform() {
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

void GUIControls::Render() {
    // Scene loading panel
    if (fileDialog.Display("ChooseTilesetDlg")) {
        if (fileDialog.IsOk()) {
            std::string tilesetPath = fileDialog.GetFilePathName();
            try {
                auto modelPaths = TilesetParser::GetB3DMPaths(tilesetPath);
                sceneManager->ClearEntities();
                for (auto& path : modelPaths) {
                    auto entity = LoadEntityFromFile(path);
                    sceneManager->AddEntity(entity);
                }
                if (auto first = sceneManager->GetFirstEntity()) {
                    SetTargetEntity(first);
                }
            } catch (const std::exception& e) {
                ImGui::OpenPopup("加载错误");
                if (ImGui::BeginPopupModal("加载错误", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
                    ImGui::Text("加载失败: %s", e.what());
                    if (ImGui::Button("确定")) ImGui::CloseCurrentPopup();
                    ImGui::EndPopup();
                }
            }
        }
        fileDialog.Close();
    }

    // Control panel
    ImGui::Begin(U8("控制面板"));
    if (ImGui::Button(U8("加载场景"))) {
        fileDialog.OpenDialog("ChooseTilesetDlg", U8("选择场景文件"), ".json");
    }
    ImGui::ColorEdit3(U8("模型基础色"), glm::value_ptr(triangleColor));
    ImGui::ColorEdit3(U8("背景颜色"), glm::value_ptr(clearColor));
    ImGui::End();

    // LOD control
    ImGui::Begin(U8("LOD 控制"));
    if (lodController) {
        ImGui::Text(U8("当前顶点数: %zu"), lodController->GetCurrentVertices());
        ImGui::Text(U8("当前误差: %.4f"), lodController->GetCurrentError());
        if (ImGui::SliderFloat(U8("简化比例"), &lodRatio, 0.0f, 1.0f, "%.2f")) {
            lodController->SimplifyTo(lodRatio);
        }
        if (ImGui::SliderFloat(U8("几何权重"), &lodParams.geometry_weight, 0.0f, 1.0f, "%.2f") ||
            ImGui::SliderFloat(U8("法向权重"), &lodParams.normal_weight, 0.0f, 1.0f, "%.2f") ||
            ImGui::SliderFloat(U8("UV权重"), &lodParams.uv_weight, 0.0f, 1.0f, "%.2f") ||
            ImGui::DragInt(U8("每帧最大折叠"), &lodParams.max_collapses_per_frame, 1, 1, 1000) ||
            ImGui::Checkbox(U8("保持拓扑"), &lodParams.preserve_topology)) {
            try { lodController->UpdateParameters(lodParams); }
            catch (const std::exception& e) { ImGui::TextColored(ImVec4(1,0.3f,0.3f,1), "参数更新失败: %s", e.what()); }
        }
    } else {
        ImGui::TextColored(ImVec4(1,0.3f,0.3f,1), U8("未绑定LOD控制器"));
    }
    ImGui::End();

    // Light settings
    ImGui::Begin(U8("灯光设置"));
    if (currentLight) {
        ImGui::DragFloat3(U8("方向"), glm::value_ptr(currentLight->direction), 0.1f);
        ImGui::ColorEdit3(U8("颜色"), glm::value_ptr(currentLight->color));
        ImGui::DragFloat(U8("强度"), &currentLight->intensity, 0.1f, 0.0f, 10.0f);
    }
    ImGui::End();

    // Model transform
    ImGui::Begin(U8("模型变换"));
    if (auto entity = targetEntity.lock()) {
        ImGui::SeparatorText(U8("模型信息"));
        if (entity->mesh && entity->mesh->IsReady()) {
            vertexCount = entity->mesh->GetVertices().size();
            triangleCount = entity->mesh->GetIndices().size()/3;
            ImGui::Text(U8("顶点数: %zu"), vertexCount);
            ImGui::Text(U8("三角形数: %zu"), triangleCount);
        }
        ImGui::SeparatorText(U8("变换控制"));
        if (ImGui::Button(U8("完全重置"))) ResetModelTransform();
        ImGui::SeparatorText(U8("位置"));
        ImGui::DragFloat3("##pos", glm::value_ptr(modelPosition), 0.1f);
        ImGui::SeparatorText(U8("旋转"));
        ImGui::DragFloat3("##rot", glm::value_ptr(modelRotation), 1.0f, -180,180);
        ImGui::SeparatorText(U8("缩放"));
        ImGui::DragFloat3("##scale", glm::value_ptr(modelScale), 0.1f,0.0f,10.0f,"%.1f");
        entity->transform->position = modelPosition;
        entity->transform->rotation = glm::quat(glm::radians(modelRotation));
        entity->transform->scale = modelScale;
        entity->transform->MarkDirty();
    }
    ImGui::End();

    // Camera control
    ImGui::Begin(U8("相机控制"));
    ImGui::SeparatorText(U8("相机操作"));
    if (ImGui::Button(U8("重置相机"))) {
        if (onCameraReset) ImGui::OpenPopup(U8("确认重置相机？"));
        else ImGui::TextColored(ImVec4(1,0.3f,0.3f,1), U8("未设置相机控制器!"));
    }
    if (ImGui::BeginPopupModal(U8("确认重置相机？"), nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text(U8("确定要重置相机到默认位置吗？"));
        ImGui::Separator();
        if (ImGui::Button(U8("确定"))) { onCameraReset(); ImGui::CloseCurrentPopup(); }
        ImGui::SameLine();
        if (ImGui::Button(U8("取消"))) ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
    ImGui::End();

    // Framebuffer view
    if (framebufferTexture) {
        ImGui::Begin(U8("Frame Buffer View"), nullptr, ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoScrollWithMouse);
        ImVec2 avail = ImGui::GetContentRegionAvail();
        float aspect = float(fbWidth)/float(fbHeight);
        float h = avail.x / aspect;
        if (h > avail.y) { h = avail.y; avail.x = h*aspect; }
        ImGui::Image((ImTextureID)(intptr_t)framebufferTexture, ImVec2(avail.x,h), ImVec2(0,1), ImVec2(1,0));
        ImGui::End();
    }
}

std::shared_ptr<Entity> GUIControls::LoadEntityFromFile(const std::string& modelPath) {
    namespace fs = std::filesystem;
    if (!fs::exists(modelPath)) throw std::runtime_error("文件不存在: " + modelPath);
    auto ext = fs::path(modelPath).extension().string();
    std::transform(ext.begin(),ext.end(),ext.begin(),::tolower);
    std::shared_ptr<Mesh> mesh;
    if (ext==".b3dm") mesh = std::make_shared<Mesh>(B3DMLoader::LoadFromFile(modelPath));
    else if(ext==".glb") {
        std::ifstream f(modelPath, std::ios::binary);
        if(!f) throw std::runtime_error("无法打开: "+modelPath);
        std::vector<uint8_t> d((std::istreambuf_iterator<char>(f)),{});
        mesh = std::make_shared<Mesh>(Mirror::GLTF::GLBParser::Parse(d).ToMesh());
    } else throw std::runtime_error("不支持的格式: "+ext);
    auto entity = std::make_shared<Entity>();
    entity->mesh = mesh;
    entity->material = std::make_shared<DefaultMaterial>();
    entity->transform = std::make_shared<Transform>();
    entity->transform->position = glm::vec3(0.0f);
    entity->transform->scale = glm::vec3(1.0f);
    entity->name = fs::path(modelPath).stem().string();
    entity->lodController = std::make_shared<ProgressiveLOD>(*mesh);
    entity->lodController->Precompute();
    return entity;
}
