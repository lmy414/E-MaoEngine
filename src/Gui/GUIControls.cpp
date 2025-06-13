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
{ 
    // --------- 全屏主窗口设置 Start ----------
    ImGuiViewport* vp = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(vp->WorkPos);
    ImGui::SetNextWindowSize(vp->WorkSize);
    
    ImGuiWindowFlags flags = 
          ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoCollapse
        | ImGuiWindowFlags_NoBringToFrontOnFocus
        | ImGuiWindowFlags_NoNavFocus;

    ImGui::Begin(U8("主界面"), nullptr, flags);

    // 计算FPS
    float currentTime = static_cast<float>(glfwGetTime());
    deltaTime = currentTime - lastTime;
    lastTime = currentTime;
    
    frameCount++;
    fpsUpdateTime += deltaTime;
    
    if (fpsUpdateTime >= FPS_UPDATE_INTERVAL) {
        fps = frameCount / fpsUpdateTime;
        frameCount = 0;
        fpsUpdateTime = 0.0f;
    }

    // 二列布局：左 250px 宽，右 自适应
    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, 250.0f);

    // ?? 左侧：竖直一列所有面板 ?? 
    ImGui::BeginChild("##LeftPane", ImVec2(0, 0), true);
    
    // 显示FPS
    ImGui::Text(U8("FPS: %.1f"), fps);
    ImGui::Separator();

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
    ImGui::ColorEdit3(U8("背景颜色"), glm::value_ptr(clearColor));
    ImGui::Spacing();

    // 2) 材质颜色控制
    ImGui::SeparatorText(U8("材质颜色"));
    if (auto entity = targetEntity.lock()) {
        if (auto material = std::dynamic_pointer_cast<DefaultMaterial>(entity->material)) {
            if (ImGui::ColorEdit3(U8("模型颜色"), glm::value_ptr(triangleColor))) {
                material->SetColor(triangleColor);
            }
        } else {
            ImGui::TextColored(ImVec4(1,0.3f,0.3f,1), U8("不支持颜色修改的材质"));
        }
    } else {
        ImGui::TextColored(ImVec4(1,0.3f,0.3f,1), U8("未选择模型"));
    }
    ImGui::Spacing();

    // 3) LOD 控制
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

    // 4) 模型变换
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

    // 5) 灯光设置
    ImGui::SeparatorText(U8("灯光设置"));
    if (currentLight) {
        ImGui::DragFloat3(U8("方向"), glm::value_ptr(currentLight->direction), 0.1f);
        ImGui::ColorEdit3(U8("颜色"), glm::value_ptr(currentLight->color));
        ImGui::DragFloat(U8("强度"), &currentLight->intensity, 0.1f, 0.0f, 10.0f);
    } else {
        ImGui::TextColored(ImVec4(1,0.3f,0.3f,1), U8("未设置灯光"));
    }
    ImGui::Spacing();

    // 6) 相机控制
    ImGui::SeparatorText(U8("相机控制"));
    if (ImGui::Button(U8("重置相机"), ImVec2(-1,0))) {
        if (onCameraReset) onCameraReset();
    }

    ImGui::Spacing();

    // ?? 显示 TileNode 信息 ?? 
    ImGui::SeparatorText(U8("TileNode 信息"));
    if (modelTree) {
        // 使用子树形式组织信息，方便展开/折叠
        if (ImGui::TreeNode(U8("基本信息##basic"))) {
            ImGui::TextWrapped(U8("名称: %s"), modelTree->name.c_str());
            ImGui::TextWrapped(U8("路径: %s"), modelTree->GetFormattedPath().c_str());
            ImGui::Text(U8("几何误差: %.4f"), modelTree->geometricError);
            ImGui::TreePop();
        }

        // 变换矩阵信息
        if (ImGui::TreeNode(U8("变换矩阵##transform"))) {
            const auto& mat = modelTree->transform;
            for (int i = 0; i < 4; ++i) {
                ImGui::Text("[%.2f, %.2f, %.2f, %.2f]",
                    mat[i][0], mat[i][1], mat[i][2], mat[i][3]);
            }
            ImGui::TreePop();
        }

        // 包围盒信息
        if (ImGui::TreeNode(U8("包围盒##boundingbox"))) {
            const auto& bv = modelTree->boundingVolume;
            ImGui::Text(U8("中心点: (%.2f, %.2f, %.2f)"),
                bv.center.x, bv.center.y, bv.center.z);
            ImGui::Text(U8("半尺寸: (%.2f, %.2f, %.2f)"),
                bv.halfSize.x, bv.halfSize.y, bv.halfSize.z);
            ImGui::TreePop();
        }

        // 细化参数
        if (ImGui::TreeNode(U8("细化参数##refine"))) {
            const auto& ref = modelTree->refine;
            ImGui::Text(U8("最小像素尺寸: %.2f"), ref.minimumPixelSize);
            ImGui::Text(U8("渲染模式: %s"), ref.additive ? U8("加法") : U8("替换"));
            ImGui::Text(U8("细化方式: %s"), ref.refinement.c_str());
            ImGui::TreePop();
        }

        // 内容信息
        if (ImGui::TreeNode(U8("内容信息##content"))) {
            const auto& content = modelTree->content;
            if (!content.uri.empty()) {
                ImGui::TextWrapped(U8("URI: %s"), content.uri.c_str());
                ImGui::Text(U8("格式: %s"), content.format.c_str());
                if (content.byteLength > 0) {
                    std::string sizeStr;
                    if (content.byteLength >= 1024 * 1024) {
                        ImGui::Text(U8("大小: %.2f MB"), content.byteLength / (1024.0 * 1024.0));
                    } else if (content.byteLength >= 1024) {
                        ImGui::Text(U8("大小: %.2f KB"), content.byteLength / 1024.0);
                    } else {
                        ImGui::Text(U8("大小: %llu B"), content.byteLength);
                    }
                }
            } else {
                ImGui::TextColored(ImVec4(1,0.3f,0.3f,1), U8("无内容信息"));
            }
            ImGui::TreePop();
        }

        // 子节点信息
        if (!modelTree->children.empty()) {
            char label[64];
            snprintf(label, sizeof(label), U8("子节点列表 (%zu)##children"), modelTree->children.size());
            if (ImGui::TreeNode(label)) {
                for (size_t i = 0; i < modelTree->children.size(); i++) {
                    const auto& child = modelTree->children[i];
                    char childLabel[64];
                    snprintf(childLabel, sizeof(childLabel), U8("子节点 %zu##child%zu"), i + 1, i);
                    if (ImGui::TreeNode(childLabel)) {
                        // 子节点基本信息
                        ImGui::TextWrapped(U8("名称: %s"), child->name.c_str());
                        ImGui::TextWrapped(U8("路径: %s"), child->GetFormattedPath().c_str());
                        ImGui::Text(U8("几何误差: %.4f"), child->geometricError);

                        // 子节点包围盒
                        char bvLabel[64];
                        snprintf(bvLabel, sizeof(bvLabel), U8("包围盒##bv%zu"), i);
                        if (ImGui::TreeNode(bvLabel)) {
                            const auto& bv = child->boundingVolume;
                            ImGui::Text(U8("中心点: (%.2f, %.2f, %.2f)"),
                                bv.center.x, bv.center.y, bv.center.z);
                            ImGui::Text(U8("半尺寸: (%.2f, %.2f, %.2f)"),
                                bv.halfSize.x, bv.halfSize.y, bv.halfSize.z);
                            ImGui::TreePop();
                        }

                        // 子节点内容信息
                        char contentLabel[64];
                        snprintf(contentLabel, sizeof(contentLabel), U8("内容信息##content%zu"), i);
                        if (ImGui::TreeNode(contentLabel)) {
                            const auto& content = child->content;
                            if (!content.uri.empty()) {
                                ImGui::TextWrapped(U8("URI: %s"), content.uri.c_str());
                                ImGui::Text(U8("格式: %s"), content.format.c_str());
                            } else {
                                ImGui::TextColored(ImVec4(1,0.3f,0.3f,1), U8("无内容信息"));
                            }
                            ImGui::TreePop();
                        }

                        // 显示孙节点数量
                        if (!child->children.empty()) {
                            ImGui::Text(U8("包含 %zu 个子节点"), child->children.size());
                        }

                        ImGui::TreePop();
                    }
                }
                ImGui::TreePop();
            }
        } else {
            ImGui::Text(U8("无子节点"));
        }
    } else {
        ImGui::TextColored(ImVec4(1,0.3f,0.3f,1), U8("无模型数据"));
    }

    ImGui::EndChild(); // 结束左侧

    // 切换到右侧
    ImGui::NextColumn();

    // ?? 右侧：帧缓冲预览 ?? 
    ImGui::BeginChild("##RightPane", ImVec2(0.0f,0.0f), true);
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
    if (!fs::exists(modelPath)) throw std::runtime_error(U8("文件不存在: ") + modelPath);
    
    // 创建并设置 TileNode
    modelTree = std::make_shared<TileNode>();
    modelTree->name = fs::path(modelPath).stem().string();
    modelTree->path = modelPath;
    modelTree->geometricError = 0.0; // 可以根据实际情况设置
    
    auto ext = fs::path(modelPath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    std::shared_ptr<Mesh> mesh;
    
    try {
        if (ext == ".b3dm") {
            mesh = std::make_shared<Mesh>(B3DMLoader::LoadFromFile(modelPath));
        } else if (ext == ".glb") {
            std::ifstream f(modelPath, std::ios::binary);
            if (!f) throw std::runtime_error(U8("无法打开: ") + modelPath);
            std::vector<uint8_t> d((std::istreambuf_iterator<char>(f)), {});
            mesh = std::make_shared<Mesh>(Mirror::GLTF::GLBParser::Parse(d).ToMesh());
        } else {
            throw std::runtime_error(U8("不支持的格式: ") + ext);
        }
    } catch (const std::exception& e) {
        modelTree.reset(); // 如果加载失败，清除modelTree
        throw;
    }
    
    auto entity = std::make_shared<Entity>();
    entity->mesh = mesh;
    entity->material = std::make_shared<DefaultMaterial>();
    entity->transform = std::make_shared<Transform>();
    entity->transform->position = glm::vec3(0.0f);
    entity->transform->scale = glm::vec3(1.0f);
    entity->name = modelTree->name;
    entity->lodController = std::make_shared<ProgressiveLOD>(*mesh);
    entity->lodController->Precompute();
    
    // 设置初始颜色
    if (auto material = std::dynamic_pointer_cast<DefaultMaterial>(entity->material)) {
        material->SetColor(triangleColor);
    }
    
    return entity;
}
