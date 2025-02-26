#pragma once
#include <glad/glad.h>    // OpenGL函数加载器

class Framebuffer {
public:
    GLuint FBO;          // 帧缓冲对象
    GLuint texture;      // 颜色附件纹理
    GLuint RBO;          // 渲染缓冲对象（深度/模板）
    int width, height;   // 缓冲尺寸

    // 构造函数（创建帧缓冲）
    Framebuffer(int width, int height);
    
    // 析构函数（释放资源）
    ~Framebuffer();
    
    // 绑定帧缓冲
    void Bind() const;
    
    // 解绑帧缓冲（绑定到默认缓冲）
    static void Unbind();
    
    // 获取颜色附件纹理
    GLuint GetTexture() const { return texture; }
};
