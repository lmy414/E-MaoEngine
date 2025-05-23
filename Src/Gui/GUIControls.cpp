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

void GUIControls::Render()
{ // --------- 全屏主窗口设置 Start ----------
    // 获取当前平台主视口
    ImGuiViewport* vp = ImGui::GetMainViewport();
    // 下次 Begin 的位置和大小
    ImGui::SetNextWindowPos(vp->WorkPos);
    ImGui::SetNextWindowSize(vp->WorkSize);
    //ImGui::SetNextWindowViewport(vp->ID);//兼容问题
    // 无标题栏、无边框、无缩放、无移动
    ImGuiWindowFlags flags = 
          ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoCollapse
        | ImGuiWindowFlags_NoBringToFrontOnFocus
        | ImGuiWindowFlags_NoNavFocus;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::Begin("控制中心", nullptr, flags);
    ImGui::PopStyleVar();
    // --------- 全屏主窗口设置 End ------------

    // 二列布局：左 250px 宽，右 自适应
    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, 250.0f);

    // —— 左侧：竖直一列所有面板 —— 
    ImGui::BeginChild("##LeftPane", ImVec2(0, 0), true);

    // 1) 加载 & 颜色
    ImGui::SeparatorText(U8("加载 & 颜色"));
    if (ImGui::Button(U8("加载 3D Tiles"), ImVec2(-1, 0))) {
        fileDialog.OpenDialog("ChooseTilesetDlg", U8("选择场景文件"), ".json");
    }
    if (fileDialog.Display("ChooseTilesetDlg")) {
        if (fileDialog.IsOk()) {
            std::string path = fileDialog.GetFilePathName();
            try {
                auto modelPaths = TilesetParser::GetB3DMPaths(path);
                sceneManager->ClearEntities();
                for (auto& p : modelPaths) {
                    auto e = LoadEntityFromFile(p);
                    if (e) sceneManager->AddEntity(e);
                }
                if (auto first = sceneManager->GetFirstEntity())
                    SetTargetEntity(first);
            } catch (const std::exception& e) {
                ImGui::OpenPopup("加载错误");
            }
        }
        fileDialog.Close();
    }
    if (ImGui::BeginPopupModal("加载错误", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("加载失败");
        if (ImGui::Button("确定")) ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
    ImGui::ColorEdit3(U8("模型基础色"), glm::value_ptr(triangleColor));
    ImGui::ColorEdit3(U8("背景颜色"), glm::value_ptr(clearColor));
    ImGui::Spacing();

    // 2) LOD 控制
    ImGui::SeparatorText(U8("LOD 控制"));
    if (lodController) {
        ImGui::Text(U8("顶点数: %zu"), lodController->GetCurrentVertices());
        ImGui::Text(U8("误差: %.4f"), lodController->GetCurrentError());
        ImGui::SliderFloat(U8("简化比例"), &lodRatio, 0.0f, 1.0f, "%.2f");
        lodController->SimplifyTo(lodRatio);
        if (ImGui::Button(U8("重置 LOD"), ImVec2(-1,0))) {
            lodRatio = 0.0f;
            lodController->SimplifyTo(lodRatio);
        }
        ImGui::Spacing();
    } else {
        ImGui::TextColored(ImVec4(1,0.3f,0.3f,1), U8("未绑定 LOD"));
        ImGui::Spacing();
    }

    // 3) 模型变换
    ImGui::SeparatorText(U8("模型变换"));
    if (auto e = targetEntity.lock()) {
        ImGui::Text(U8("顶点: %zu   三角: %zu"),
                    e->mesh->GetVertices().size(),
                    e->mesh->GetIndices().size()/3);
        ImGui::DragFloat3(U8("位置"), glm::value_ptr(modelPosition), 0.1f);
        ImGui::DragFloat3(U8("旋转"), glm::value_ptr(modelRotation), 1.0f, -180,180);
        ImGui::DragFloat3(U8("缩放"), glm::value_ptr(modelScale),    0.1f, 0.0f,10.0f,"%.1f");
        if (ImGui::Button(U8("完全重置"), ImVec2(-1,0))) ResetModelTransform();
        e->transform->position = modelPosition;
        e->transform->rotation = glm::quat(glm::radians(modelRotation));
        e->transform->scale    = modelScale;
        e->transform->MarkDirty();
    } else {
        ImGui::TextColored(ImVec4(1,0.3f,0.3f,1), U8("未选择模型"));
    }
    ImGui::Spacing();

    // 4) 灯光设置
    ImGui::SeparatorText(U8("灯光设置"));
    if (currentLight) {
        ImGui::DragFloat3(U8("方向"), glm::value_ptr(currentLight->direction), 0.1f);
        ImGui::ColorEdit3(U8("颜色"), glm::value_ptr(currentLight->color));
        ImGui::DragFloat(U8("强度"), &currentLight->intensity, 0.1f, 0.0f, 10.0f);
    } else {
        ImGui::TextColored(ImVec4(1,0.3f,0.3f,1), U8("未设置灯光"));
    }
    ImGui::Spacing();

    // 5) 相机控制
    ImGui::SeparatorText(U8("相机控制"));
    if (ImGui::Button(U8("重置相机"), ImVec2(-1,0))) {
        if (onCameraReset) onCameraReset();
    }

    ImGui::EndChild(); // 结束左侧

    // 切换到右侧
    ImGui::NextColumn();

    // —— 右侧：帧缓冲预览 —— 
    ImGui::BeginChild("##RightPane", ImVec2(1340.0f,1100.0f), true);
    ImGui::SeparatorText(U8("缓冲预览"));
    if (framebufferTexture) {
        ImVec2 avail = ImGui::GetContentRegionAvail();
        float aspect = float(fbWidth)/float(fbHeight);
        float h = avail.x / aspect;
        if (h > avail.y) { h = avail.y; avail.x = h*aspect; }
        ImGui::Image((ImTextureID)(intptr_t)framebufferTexture,
                     ImVec2(avail.x,h), ImVec2(0,1), ImVec2(1,0));
    } else {
        ImGui::TextDisabled(U8("暂无帧缓冲纹理"));
    }
    ImGui::EndChild();

    ImGui::Columns(1);
    ImGui::End();
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
