#ifndef OPENGL_UTILS_H
#define OPENGL_UTILS_H

// 禁止GLFW自动包含OpenGL头文件（由GLAD负责）
#define GLFW_INCLUDE_NONE 

// 包含必要的图形库头文件
#include <glad/glad.h>    // OpenGL函数加载器
#include <GLFW/glfw3.h>   // 跨平台窗口管理库

/**
 * @brief 初始化OpenGL上下文并创建窗口
 * @param width 窗口宽度（像素）
 * @param height 窗口高度（像素）
 * @param title 窗口标题
 * @return 成功返回GLFWwindow指针，失败返回nullptr
 */
GLFWwindow* InitializeOpenGL(int width, int height, const char* title);

/**
 * @brief 帧缓冲大小变化回调函数
 * @param window 关联的窗口对象
 * @param width 新的帧缓冲宽度
 * @param height 新的帧缓冲高度
 */
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

// 注意：此处可以继续添加其他输入处理回调的声明
// 例如：void key_callback(GLFWwindow* window, ...);

#endif // OPENGL_UTILS_H

// 现代编译器支持的简写版头文件保护（与#ifndef方式共存）
#pragma once
