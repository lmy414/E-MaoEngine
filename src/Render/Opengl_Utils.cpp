#include "Opengl_Utils.h"
#include <iostream>

namespace OpenGLUtils {

    void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
    }

    GLFWwindow* InitializeOpenGL(int width, int height, const char* title) {
        // 初始化GLFW
        if (!glfwInit()) {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            return nullptr;
        }

        // 配置GLFW
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef _DEBUG
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

        // 创建窗口
        GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr);
        if (!window) {
            std::cerr << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return nullptr;
        }

        // 设置上下文
        glfwMakeContextCurrent(window);

        // 设置帧缓冲大小回调
        glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

        // 初始化GLAD
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cerr << "Failed to initialize GLAD" << std::endl;
            return nullptr;
        }

        // 设置视口
        glViewport(0, 0, width, height);

#ifdef _DEBUG
        SetupDebugCallback();
#endif

        return window;
    }

    // ... 其他函数的实现 ...
} // namespace OpenGLUtils
