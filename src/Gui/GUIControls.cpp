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
    // --------- ȫ������������ Start ----------
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

    ImGui::Begin(U8("������"), nullptr, flags);

    // ����FPS
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

    // ���в��֣��� 250px ���� ����Ӧ
    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, 250.0f);

    // ?? ��ࣺ��ֱһ��������� ?? 
    ImGui::BeginChild("##LeftPane", ImVec2(0, 0), true);
    
    // ��ʾFPS
    ImGui::Text(U8("FPS: %.1f"), fps);
    ImGui::Separator();

    // 1) ���� & ��ɫ
    ImGui::SeparatorText(U8("���� & ��ɫ"));
    if (ImGui::Button(U8("���� 3D Tiles"), ImVec2(-1, 0))) {
        fileDialog.OpenDialog("ChooseTilesetDlg", U8("ѡ�񳡾��ļ�"), ".json");
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
                ImGui::OpenPopup("���ش���");
            }
        }
        fileDialog.Close();
    }
    if (ImGui::BeginPopupModal("���ش���", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("����ʧ��");
        if (ImGui::Button("ȷ��")) ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
    ImGui::ColorEdit3(U8("������ɫ"), glm::value_ptr(clearColor));
    ImGui::Spacing();

    // 2) ������ɫ����
    ImGui::SeparatorText(U8("������ɫ"));
    if (auto entity = targetEntity.lock()) {
        if (auto material = std::dynamic_pointer_cast<DefaultMaterial>(entity->material)) {
            if (ImGui::ColorEdit3(U8("ģ����ɫ"), glm::value_ptr(triangleColor))) {
                material->SetColor(triangleColor);
            }
        } else {
            ImGui::TextColored(ImVec4(1,0.3f,0.3f,1), U8("��֧����ɫ�޸ĵĲ���"));
        }
    } else {
        ImGui::TextColored(ImVec4(1,0.3f,0.3f,1), U8("δѡ��ģ��"));
    }
    ImGui::Spacing();

    // 3) LOD ����
    ImGui::SeparatorText(U8("LOD ����"));
    if (lodController) {
        ImGui::Text(U8("������: %zu"), lodController->GetCurrentVertices());
        ImGui::Text(U8("���: %.4f"), lodController->GetCurrentError());
        ImGui::SliderFloat(U8("�򻯱���"), &lodRatio, 0.0f, 1.0f, "%.2f");
        lodController->SimplifyTo(lodRatio);
        if (ImGui::Button(U8("���� LOD"), ImVec2(-1,0))) {
            lodRatio = 0.0f;
            lodController->SimplifyTo(lodRatio);
        }
        ImGui::Spacing();
    } else {
        ImGui::TextColored(ImVec4(1,0.3f,0.3f,1), U8("δ�� LOD"));
        ImGui::Spacing();
    }

    // 4) ģ�ͱ任
    ImGui::SeparatorText(U8("ģ�ͱ任"));
    if (auto e = targetEntity.lock()) {
        ImGui::Text(U8("����: %zu   ����: %zu"),
                    e->mesh->GetVertices().size(),
                    e->mesh->GetIndices().size()/3);
        ImGui::DragFloat3(U8("λ��"), glm::value_ptr(modelPosition), 0.1f);
        ImGui::DragFloat3(U8("��ת"), glm::value_ptr(modelRotation), 1.0f, -180,180);
        ImGui::DragFloat3(U8("����"), glm::value_ptr(modelScale),    0.1f, 0.0f,10.0f,"%.1f");
        if (ImGui::Button(U8("��ȫ����"), ImVec2(-1,0))) ResetModelTransform();
        e->transform->position = modelPosition;
        e->transform->rotation = glm::quat(glm::radians(modelRotation));
        e->transform->scale    = modelScale;
        e->transform->MarkDirty();
    } else {
        ImGui::TextColored(ImVec4(1,0.3f,0.3f,1), U8("δѡ��ģ��"));
    }
    ImGui::Spacing();

    // 5) �ƹ�����
    ImGui::SeparatorText(U8("�ƹ�����"));
    if (currentLight) {
        ImGui::DragFloat3(U8("����"), glm::value_ptr(currentLight->direction), 0.1f);
        ImGui::ColorEdit3(U8("��ɫ"), glm::value_ptr(currentLight->color));
        ImGui::DragFloat(U8("ǿ��"), &currentLight->intensity, 0.1f, 0.0f, 10.0f);
    } else {
        ImGui::TextColored(ImVec4(1,0.3f,0.3f,1), U8("δ���õƹ�"));
    }
    ImGui::Spacing();

    // 6) �������
    ImGui::SeparatorText(U8("�������"));
    if (ImGui::Button(U8("�������"), ImVec2(-1,0))) {
        if (onCameraReset) onCameraReset();
    }

    ImGui::Spacing();

    // ?? ��ʾ TileNode ��Ϣ ?? 
    ImGui::SeparatorText(U8("TileNode ��Ϣ"));
    if (modelTree) {
        // ʹ��������ʽ��֯��Ϣ������չ��/�۵�
        if (ImGui::TreeNode(U8("������Ϣ##basic"))) {
            ImGui::TextWrapped(U8("����: %s"), modelTree->name.c_str());
            ImGui::TextWrapped(U8("·��: %s"), modelTree->GetFormattedPath().c_str());
            ImGui::Text(U8("�������: %.4f"), modelTree->geometricError);
            ImGui::TreePop();
        }

        // �任������Ϣ
        if (ImGui::TreeNode(U8("�任����##transform"))) {
            const auto& mat = modelTree->transform;
            for (int i = 0; i < 4; ++i) {
                ImGui::Text("[%.2f, %.2f, %.2f, %.2f]",
                    mat[i][0], mat[i][1], mat[i][2], mat[i][3]);
            }
            ImGui::TreePop();
        }

        // ��Χ����Ϣ
        if (ImGui::TreeNode(U8("��Χ��##boundingbox"))) {
            const auto& bv = modelTree->boundingVolume;
            ImGui::Text(U8("���ĵ�: (%.2f, %.2f, %.2f)"),
                bv.center.x, bv.center.y, bv.center.z);
            ImGui::Text(U8("��ߴ�: (%.2f, %.2f, %.2f)"),
                bv.halfSize.x, bv.halfSize.y, bv.halfSize.z);
            ImGui::TreePop();
        }

        // ϸ������
        if (ImGui::TreeNode(U8("ϸ������##refine"))) {
            const auto& ref = modelTree->refine;
            ImGui::Text(U8("��С���سߴ�: %.2f"), ref.minimumPixelSize);
            ImGui::Text(U8("��Ⱦģʽ: %s"), ref.additive ? U8("�ӷ�") : U8("�滻"));
            ImGui::Text(U8("ϸ����ʽ: %s"), ref.refinement.c_str());
            ImGui::TreePop();
        }

        // ������Ϣ
        if (ImGui::TreeNode(U8("������Ϣ##content"))) {
            const auto& content = modelTree->content;
            if (!content.uri.empty()) {
                ImGui::TextWrapped(U8("URI: %s"), content.uri.c_str());
                ImGui::Text(U8("��ʽ: %s"), content.format.c_str());
                if (content.byteLength > 0) {
                    std::string sizeStr;
                    if (content.byteLength >= 1024 * 1024) {
                        ImGui::Text(U8("��С: %.2f MB"), content.byteLength / (1024.0 * 1024.0));
                    } else if (content.byteLength >= 1024) {
                        ImGui::Text(U8("��С: %.2f KB"), content.byteLength / 1024.0);
                    } else {
                        ImGui::Text(U8("��С: %llu B"), content.byteLength);
                    }
                }
            } else {
                ImGui::TextColored(ImVec4(1,0.3f,0.3f,1), U8("��������Ϣ"));
            }
            ImGui::TreePop();
        }

        // �ӽڵ���Ϣ
        if (!modelTree->children.empty()) {
            char label[64];
            snprintf(label, sizeof(label), U8("�ӽڵ��б� (%zu)##children"), modelTree->children.size());
            if (ImGui::TreeNode(label)) {
                for (size_t i = 0; i < modelTree->children.size(); i++) {
                    const auto& child = modelTree->children[i];
                    char childLabel[64];
                    snprintf(childLabel, sizeof(childLabel), U8("�ӽڵ� %zu##child%zu"), i + 1, i);
                    if (ImGui::TreeNode(childLabel)) {
                        // �ӽڵ������Ϣ
                        ImGui::TextWrapped(U8("����: %s"), child->name.c_str());
                        ImGui::TextWrapped(U8("·��: %s"), child->GetFormattedPath().c_str());
                        ImGui::Text(U8("�������: %.4f"), child->geometricError);

                        // �ӽڵ��Χ��
                        char bvLabel[64];
                        snprintf(bvLabel, sizeof(bvLabel), U8("��Χ��##bv%zu"), i);
                        if (ImGui::TreeNode(bvLabel)) {
                            const auto& bv = child->boundingVolume;
                            ImGui::Text(U8("���ĵ�: (%.2f, %.2f, %.2f)"),
                                bv.center.x, bv.center.y, bv.center.z);
                            ImGui::Text(U8("��ߴ�: (%.2f, %.2f, %.2f)"),
                                bv.halfSize.x, bv.halfSize.y, bv.halfSize.z);
                            ImGui::TreePop();
                        }

                        // �ӽڵ�������Ϣ
                        char contentLabel[64];
                        snprintf(contentLabel, sizeof(contentLabel), U8("������Ϣ##content%zu"), i);
                        if (ImGui::TreeNode(contentLabel)) {
                            const auto& content = child->content;
                            if (!content.uri.empty()) {
                                ImGui::TextWrapped(U8("URI: %s"), content.uri.c_str());
                                ImGui::Text(U8("��ʽ: %s"), content.format.c_str());
                            } else {
                                ImGui::TextColored(ImVec4(1,0.3f,0.3f,1), U8("��������Ϣ"));
                            }
                            ImGui::TreePop();
                        }

                        // ��ʾ��ڵ�����
                        if (!child->children.empty()) {
                            ImGui::Text(U8("���� %zu ���ӽڵ�"), child->children.size());
                        }

                        ImGui::TreePop();
                    }
                }
                ImGui::TreePop();
            }
        } else {
            ImGui::Text(U8("���ӽڵ�"));
        }
    } else {
        ImGui::TextColored(ImVec4(1,0.3f,0.3f,1), U8("��ģ������"));
    }

    ImGui::EndChild(); // �������

    // �л����Ҳ�
    ImGui::NextColumn();

    // ?? �Ҳࣺ֡����Ԥ�� ?? 
    ImGui::BeginChild("##RightPane", ImVec2(0.0f,0.0f), true);
    ImGui::SeparatorText(U8("����Ԥ��"));
    if (framebufferTexture) {
        ImVec2 avail = ImGui::GetContentRegionAvail();
        float aspect = float(fbWidth)/float(fbHeight);
        float h = avail.x / aspect;
        if (h > avail.y) { h = avail.y; avail.x = h*aspect; }
        ImGui::Image((ImTextureID)(intptr_t)framebufferTexture,
                     ImVec2(avail.x,h), ImVec2(0,1), ImVec2(1,0));
    } else {
        ImGui::TextDisabled(U8("����֡��������"));
    }
    ImGui::EndChild();

    ImGui::Columns(1);
    ImGui::End();
}

std::shared_ptr<Entity> GUIControls::LoadEntityFromFile(const std::string& modelPath) {
    namespace fs = std::filesystem;
    if (!fs::exists(modelPath)) throw std::runtime_error(U8("�ļ�������: ") + modelPath);
    
    // ���������� TileNode
    modelTree = std::make_shared<TileNode>();
    modelTree->name = fs::path(modelPath).stem().string();
    modelTree->path = modelPath;
    modelTree->geometricError = 0.0; // ���Ը���ʵ���������
    
    auto ext = fs::path(modelPath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    std::shared_ptr<Mesh> mesh;
    
    try {
        if (ext == ".b3dm") {
            mesh = std::make_shared<Mesh>(B3DMLoader::LoadFromFile(modelPath));
        } else if (ext == ".glb") {
            std::ifstream f(modelPath, std::ios::binary);
            if (!f) throw std::runtime_error(U8("�޷���: ") + modelPath);
            std::vector<uint8_t> d((std::istreambuf_iterator<char>(f)), {});
            mesh = std::make_shared<Mesh>(Mirror::GLTF::GLBParser::Parse(d).ToMesh());
        } else {
            throw std::runtime_error(U8("��֧�ֵĸ�ʽ: ") + ext);
        }
    } catch (const std::exception& e) {
        modelTree.reset(); // �������ʧ�ܣ����modelTree
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
    
    // ���ó�ʼ��ɫ
    if (auto material = std::dynamic_pointer_cast<DefaultMaterial>(entity->material)) {
        material->SetColor(triangleColor);
    }
    
    return entity;
}
