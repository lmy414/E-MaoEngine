#include "Opengl_Utils.h"
#include <iostream>

namespace OpenGLUtils {

    void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
    }

    GLFWwindow* InitializeOpenGL(int width, int height, const char* title) {
        // ��ʼ��GLFW
        if (!glfwInit()) {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            return nullptr;
        }

        // ����GLFW
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef _DEBUG
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

        // ��������
        GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr);
        if (!window) {
            std::cerr << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return nullptr;
        }

        // ����������
        glfwMakeContextCurrent(window);

        // ����֡�����С�ص�
        glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

        // ��ʼ��GLAD
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cerr << "Failed to initialize GLAD" << std::endl;
            return nullptr;
        }

        // �����ӿ�
        glViewport(0, 0, width, height);

#ifdef _DEBUG
        SetupDebugCallback();
#endif

        return window;
    }

    // ... ����������ʵ�� ...
} // namespace OpenGLUtils
