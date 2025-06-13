/**
 * @file Framebuffer.h
 * @brief OpenGL帧缓冲区封装
 * @author MirrorEngine Team
 * @date 2024
 */
#pragma once
#include <glad/glad.h>    // OpenGL函数加载器

class Framebuffer {
public:
    Framebuffer(int width, int height);
    ~Framebuffer();

    // 禁用拷贝和移动（保持原样）
    Framebuffer(const Framebuffer&) = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;
    Framebuffer(Framebuffer&&) = delete;
    Framebuffer& operator=(Framebuffer&&) = delete;

    void Bind() const;
    static void Unbind();

    // 修改：添加 public 访问器
    int Width() const { return width; }   // 保持原变量名但公开访问
    int Height() const { return height; } // 保持原变量名但公开访问
    GLuint GetTexture() const { return texture; }

private:
    GLuint FBO = 0;
    GLuint texture = 0;
    GLuint RBO = 0;
    int width = 0;       // 保持原变量名
    int height = 0;      // 保持原变量名
};
