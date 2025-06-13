// GUIControls.h
#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <functional>
#include "Render/Entity.h"
#include "Render/SceneManager.h"
#include "Render/Light/Light.h"
#include "imgui/ImGuiFileDialog.h"
#include "3Dtiles/B3DMLoader.h"
#include "3Dtiles/TilesetParser.h"
#include "Render/ProgressiveLOD.h"
#include "Render/Material/DerivedMaterials.h"
#include <mutex>
#include <GLFW/glfw3.h>
#include "imgui/imgui.h"
#include "3Dtiles/TileNode.h"  

#if defined(__cpp_char8_t)
    #define U8(str) reinterpret_cast<const char*>(u8##str)
#else
    #define U8(str) u8##str
#endif

class GUIControls {
public:
    // Scene and entity
    void SetSceneManager(SceneManager* mgr) { sceneManager = mgr; }
    void SetTargetEntity(const std::shared_ptr<Entity>& entity);
    std::weak_ptr<Entity> GetTargetEntity() const { return targetEntity; }
    std::shared_ptr<TileNode> modelTree;  // 用于存储 TileNode 树
    // LOD Controller
    std::shared_ptr<ProgressiveLOD> lodController;
    ProgressiveLOD::Parameters lodParams;
    float lodRatio = 1.0f;

    // Colors and material
    glm::vec3 triangleColor = glm::vec3(1.0f);
    glm::vec3 clearColor = {0.2f, 0.3f, 0.3f};

    // Model transform
    glm::vec3 modelPosition{0.0f};
    glm::vec3 modelRotation{0.0f}; // Euler degrees
    glm::vec3 modelScale{1.0f};

    // Light
    void SetLight(Light* light) { currentLight = light; }

    // Camera reset
    void AddResetCameraCallback(std::function<void()> callback) { onCameraReset = callback; }

    // Framebuffer preview
    void SetFramebufferInfo(GLuint texID, int width, int height) {
        framebufferTexture = texID;
        fbWidth = width;
        fbHeight = height;
    }

    // Reset model transform
    void ResetModelTransform();

    // Main render UI
    void Render();

private:
    SceneManager* sceneManager = nullptr;
    std::weak_ptr<Entity> targetEntity;
    Light* currentLight = nullptr;
    std::function<void()> onCameraReset;
    ImGuiFileDialog fileDialog;
    std::mutex resourceMutex;

    // For stats
    size_t vertexCount = 0;
    size_t triangleCount = 0;

    // FPS tracking
    float lastTime = 0.0f;
    float deltaTime = 0.0f;
    float fps = 0.0f;
    int frameCount = 0;
    float fpsUpdateTime = 0.0f;
    static constexpr float FPS_UPDATE_INTERVAL = 0.5f; // Update FPS every 0.5 seconds

    // Framebuffer data
    GLuint framebufferTexture = 0;
    int fbWidth = 0, fbHeight = 0;

    std::shared_ptr<Entity> LoadEntityFromFile(const std::string& modelPath);
};