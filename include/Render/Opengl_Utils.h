/**
 * @file Opengl_Utils.h
 * @brief OpenGL工具函数集
 * @author MirrorEngine Team
 * @date 2024
 */
#pragma once

 // 禁止GLFW自动包含OpenGL头文件（由GLAD负责）
#define GLFW_INCLUDE_NONE 

// 包含必要的图形库头文件
#include <glad/glad.h>    // OpenGL函数加载器
#include <GLFW/glfw3.h>   // 跨平台窗口管理库

namespace OpenGLUtils {

	/**
	 * @brief 初始化OpenGL上下文并创建窗口
	 *
	 * 该函数完成以下任务：
	 * 1. 初始化GLFW库
	 * 2. 配置OpenGL上下文版本和模式
	 * 3. 创建窗口和OpenGL上下文
	 * 4. 加载OpenGL函数指针
	 *
	 * @param width 窗口宽度（像素）
	 * @param height 窗口高度（像素）
	 * @param title 窗口标题
	 * @return 成功返回GLFWwindow指针，失败返回nullptr
	 * @throws std::runtime_error 如果初始化失败
	 */
	GLFWwindow* InitializeOpenGL(int width, int height, const char* title);

	/**
	 * @brief 帧缓冲大小变化回调函数
	 *
	 * 当窗口大小改变时自动调用，用于更新视口大小。
	 *
	 * @param window 关联的窗口对象
	 * @param width 新的帧缓冲宽度
	 * @param height 新的帧缓冲高度
	 */
	void FramebufferSizeCallback(GLFWwindow* window, int width, int height);

} // namespace OpenGLUtils

// 用于简化OpenGL错误检查的宏
#ifdef _DEBUG
#define GL_CHECK() OpenGLUtils::CheckGLError(__FILE__, __LINE__)
#else
#define GL_CHECK() ((void)0)
#endif
