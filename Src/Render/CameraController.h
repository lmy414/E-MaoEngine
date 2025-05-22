// CameraController.h
#pragma once
#include "Camera.h"
#include <GLFW/glfw3.h>
#include <unordered_map>

class CameraController {
public:
    CameraController(GLFWwindow* window, Camera& camera);
    ~CameraController();

    // 每帧更新移动状态（基于deltaTime）
    void update(float deltaTime);

    // 禁用复制
    CameraController(const CameraController&) = delete;
    CameraController& operator=(const CameraController&) = delete;

private:
    GLFWwindow* window_;
    Camera& camera_;
    
    // 输入状态管理
    struct {
        bool middleButtonPressed  = false;
        double lastX = 0.0, lastY = 0.0;
        std::unordered_map<int, bool> keys{};
    } inputState_;

    // 回调包装器
    static void mouseCallback(GLFWwindow* w, double x, double y);
    static void scrollCallback(GLFWwindow* w, double x, double y);
    static void keyCallback(GLFWwindow* w, int key, int scancode, int action, int mods);
    static void mouseButtonCallback(GLFWwindow* w, int button, int action, int mods);

    // 实例方法
    void processMouseMovement(double x, double y);
    void processScroll(double offset);
    void processKey(int key, int action);
    void processMouseButton(int button, int action);
};
