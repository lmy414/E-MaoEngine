#include "Opengl_Utils.h"
#include <iostream>

GLFWwindow* InitializeOpenGL(int width, int height, const char* title) {
    // 初始化GLFW库
    if (!glfwInit()) {
        std::cerr << "GLFW初始化失败" << std::endl;
        return nullptr;
    }

    // 设置OpenGL版本和配置
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);  // 主版本号3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);  // 次版本号3
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 使用核心模式

#ifdef __APPLE__
    // macOS需要的前向兼容设置
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 
#endif

    // 创建窗口对象
    GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
        std::cerr << "创建GLFW窗口失败" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);  // 设置当前上下文

    // 初始化GLAD加载OpenGL函数指针
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "GLAD初始化失败" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return nullptr;
    }

    // 配置全局OpenGL状态
    glViewport(0, 0, width, height);  // 设置初始视口尺寸
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);  // 注册窗口大小回调

    return window;
}

// 窗口大小改变回调函数
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);  // 调整视口匹配新窗口尺寸
}
