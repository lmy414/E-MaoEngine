#ifndef COMMON_H
#define COMMON_H

// GUI 模块
// 管理图形用户界面的相关操
#include "../imgui/ImGuiFileDialog.h"
//#include "../Src/Gui/ImGuiControls.h"90

// 渲染模块
// 封装了 OpenGL 常用工具函数
#include "../Src/Render/Opengl_Utils.h"
// 封装着色器类，管理 OpenGL 着色器的加载、编译与使用

// 封装网格渲染，渲染一个网格对象
//封装摄像机，管理相机位置，方向，视角等

//#include"../Src/Render/Render.h"
//封装着色器类
//#include"../Src/Render/Material.h"
//封装光源类
// 工具模块
// 封装了MVP矩阵


// GLM 数学库
// 用于矢量、矩阵运算以及变换计算
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// 标准库
// 包含输入输出流的头文件，常用于调试和日志打印
#include <iostream>

#endif // COMMON_H




















