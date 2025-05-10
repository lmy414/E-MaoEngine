// CameraController.cpp
#include "CameraController.h"
#include <iostream>

CameraController::CameraController(GLFWwindow* window, Camera& camera)
    : window_(window), camera_(camera) {
    
    // 设置用户指针关联当前实例
    glfwSetWindowUserPointer(window_, this);
    
    // 注册回调函数
    glfwSetCursorPosCallback(window_, &CameraController::mouseCallback);
    glfwSetScrollCallback(window_, &CameraController::scrollCallback);
    glfwSetKeyCallback(window_, &CameraController::keyCallback);
    glfwSetMouseButtonCallback(window_, &CameraController::mouseButtonCallback);

    // 初始化鼠标位置
    double x, y;
    glfwGetCursorPos(window_, &x, &y);
    inputState_.lastX = x;
    inputState_.lastY = y;
}

CameraController::~CameraController() {
    // 清理回调绑定
    glfwSetCursorPosCallback(window_, nullptr);
    glfwSetScrollCallback(window_, nullptr);
    glfwSetKeyCallback(window_, nullptr);
    glfwSetMouseButtonCallback(window_, nullptr);
}

void CameraController::update(float deltaTime) {
    // 处理WASD移动
    static const std::unordered_map<int, Camera::Movement> keyMappings{
        {GLFW_KEY_W, Camera::Movement::Forward},
        {GLFW_KEY_S, Camera::Movement::Backward},
        {GLFW_KEY_A, Camera::Movement::Left},
        {GLFW_KEY_D, Camera::Movement::Right},
        {GLFW_KEY_SPACE, Camera::Movement::Up},
        {GLFW_KEY_LEFT_SHIFT, Camera::Movement::Down}
    };

    for (const auto& [key, movement] : keyMappings) {
        if (inputState_.keys[key]) {
            camera_.processKeyboard(movement, deltaTime);
        }
    }
}

//=== 静态回调路由 ===//
void CameraController::mouseCallback(GLFWwindow* w, double x, double y) {
    auto* controller = static_cast<CameraController*>(glfwGetWindowUserPointer(w));
    if (controller) controller->processMouseMovement(x, y);
}

void CameraController::scrollCallback(GLFWwindow* w, double x, double y) {
    auto* controller = static_cast<CameraController*>(glfwGetWindowUserPointer(w));
    if (controller) controller->processScroll(static_cast<float>(y));
}

void CameraController::keyCallback(GLFWwindow* w, int key, int, int action, int) {
    auto* controller = static_cast<CameraController*>(glfwGetWindowUserPointer(w));
    if (controller) controller->processKey(key, action);
}

void CameraController::mouseButtonCallback(GLFWwindow* w, int button, int action, int) {
    auto* controller = static_cast<CameraController*>(glfwGetWindowUserPointer(w));
    if (controller) controller->processMouseButton(button, action);
}

//=== 实际输入处理 ===//
void CameraController::processMouseMovement(double x, double y) {
    if (!inputState_.leftButtonPressed) return;

    float xOffset = static_cast<float>(x - inputState_.lastX);
    float yOffset = static_cast<float>(inputState_.lastY - y); // Y轴反转

    camera_.processMouseMovement(xOffset, yOffset);

    inputState_.lastX = x;
    inputState_.lastY = y;
}

void CameraController::processScroll(double offset) {
    camera_.processMouseScroll(static_cast<float>(offset));
}

void CameraController::processKey(int key, int action) {
    if (action == GLFW_PRESS) {
        inputState_.keys[key] = true;
    } else if (action == GLFW_RELEASE) {
        inputState_.keys[key] = false;
    }
}

void CameraController::processMouseButton(int button, int action) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        inputState_.leftButtonPressed = (action == GLFW_PRESS);
        
        // 更新鼠标初始位置
        if (inputState_.leftButtonPressed) {
            glfwGetCursorPos(window_, &inputState_.lastX, &inputState_.lastY);
        }
    }
}
