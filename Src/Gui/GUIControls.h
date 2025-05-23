// GUIControls.h
#pragma once
#include <glm/glm.hpp>
#include "../Render/Entity.h"
#include <GLFW/glfw3.h>
#include "../../imgui/imgui.h"
#include <functional>  
#include "../Render/Light/Light.h"
#include "../Render/Material/DerivedMaterials.h"
#include "../../imgui/ImGuiFileDialog.h"
#include "../Render/SceneManager.h"
#include "../../3Dtiles/B3DMLoader.h"
#include "../../3Dtiles/TilesetParser.h"

// 定义跨平台兼容宏
#if defined(__cpp_char8_t) // 检测C++20 char8_t特性
    #define U8(str) reinterpret_cast<const char*>(u8##str)
#else
    #define U8(str) u8##str
#endif


class GUIControls {

public:
    glm::vec3 triangleColor = glm::vec3(1.0f); // 新增材质颜色存储
    std::weak_ptr<Entity> GetTargetEntity() const { return targetEntity; }
    const glm::vec3& GetTriangleColor() const { return triangleColor; }  // 新增
    glm::vec3 clearColor = {0.2f, 0.3f, 0.3f};    // 默认背景色
    glm::vec3 modelScale {1.0f}; // 新增缩放变量
    size_t vertexCount = 0;
    size_t triangleCount = 0;

    void SetSceneManager(SceneManager* mgr) { sceneManager = mgr; }

    //灯光
    Light* currentLight = nullptr;
    void SetLight(Light* light) { 
        currentLight = light; 
    }
    
    // 新增帧缓冲相关参数
    GLuint framebufferTexture = 0;
    int fbWidth = 0;
    int fbHeight = 0;
    // 新增模型控制参数 ---------------------------------
    std::weak_ptr<Entity> targetEntity;  // 添加目标实体引用
    glm::vec3 modelPosition{0.0f};
    glm::vec3 modelRotation{0.0f}; // 存储为欧拉角（度）
    
    // 模型控制接口 --------------------------------------
    void SetTargetEntity(const std::shared_ptr<Entity>& entity) {
        targetEntity = entity;
        if (auto t = entity->transform) {
            modelPosition = t->position;
            modelRotation = glm::degrees(glm::eulerAngles(t->rotation)); // 四元数转欧拉角
            modelScale = t->scale;
        }
    }
    
    void Render();
    
    // 设置帧缓冲参数的方法
    void SetFramebufferInfo(GLuint texID, int width, int height) {
        framebufferTexture = texID;
        fbWidth = width;
        fbHeight = height;
    }
    // 新增重置功能相关成员
    void ResetModelTransform();
    void AddResetCameraCallback(std::function<void()> callback) { onCameraReset = callback; }
    
private:
    std::function<void()> onCameraReset; // 相机重置回调
    ImGuiFileDialog fileDialog;
    std::mutex resourceMutex;
    SceneManager* sceneManager = nullptr; // 新增场景管理器指针
    std::shared_ptr<Entity> LoadEntityFromFile(const std::string& modelPath);

};
