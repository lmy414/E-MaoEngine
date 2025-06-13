// CameraController.cpp
#include "CameraController.h"
#include <iostream>

CameraController::CameraController(GLFWwindow* window, Camera& camera)
    : window_(window), camera_(camera) {
    
    // �����û�ָ�������ǰʵ��
    glfwSetWindowUserPointer(window_, this);
    
    // ע��ص�����
    glfwSetCursorPosCallback(window_, &CameraController::mouseCallback);
    glfwSetScrollCallback(window_, &CameraController::scrollCallback);
    glfwSetKeyCallback(window_, &CameraController::keyCallback);
    glfwSetMouseButtonCallback(window_, &CameraController::mouseButtonCallback);

    // ��ʼ�����λ��
    double x, y;
    glfwGetCursorPos(window_, &x, &y);
    inputState_.lastX = x;
    inputState_.lastY = y;
}

CameraController::~CameraController() {
    // ����ص���
    glfwSetCursorPosCallback(window_, nullptr);
    glfwSetScrollCallback(window_, nullptr);
    glfwSetKeyCallback(window_, nullptr);
    glfwSetMouseButtonCallback(window_, nullptr);
}

void CameraController::update(float deltaTime) {
    static const std::unordered_map<int, Camera::Movement> keyMappings{
            {GLFW_KEY_W, Camera::Movement::Forward},
            {GLFW_KEY_S, Camera::Movement::Backward},
            {GLFW_KEY_A, Camera::Movement::Left},
            {GLFW_KEY_D, Camera::Movement::Right},
            {GLFW_KEY_SPACE, Camera::Movement::Up},
            {GLFW_KEY_C, Camera::Movement::Down} // Shift��Ϊƽ�ƹ���
    };
    for (const auto& [key, movement] : keyMappings) {
        if (inputState_.keys[key]) {
            camera_.processKeyboard(movement, deltaTime);
        }
    }

}

//=== ��̬�ص�·�� ===//
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

//=== ʵ�����봦�� ===//
void CameraController::processMouseMovement(double x, double y) {
    if (!inputState_.middleButtonPressed) return;

    const bool shiftPressed = 
       glfwGetKey(window_, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
       glfwGetKey(window_, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
    const float xOffset = static_cast<float>(x - inputState_.lastX);
    const float yOffset = static_cast<float>(inputState_.lastY - y);
    if (shiftPressed) {
        camera_.processPan(
            xOffset * camera_.getConfig().panSensitivity,
            yOffset * camera_.getConfig().panSensitivity
        );
    } else {
        camera_.processMouseMovement(xOffset, yOffset);
    }
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
    if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
        inputState_.middleButtonPressed = (action == GLFW_PRESS);
        
        // ��������ʼλ��
        if (inputState_.middleButtonPressed) {
            glfwGetCursorPos(window_, &inputState_.lastX, &inputState_.lastY);
        }
    }
}
