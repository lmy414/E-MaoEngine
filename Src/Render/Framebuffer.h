/**
 * @file Framebuffer.h
 * @brief OpenGL帧缓冲区封装
 * @author MirrorEngine Team
 * @date 2024
 */
#pragma once
#include <glad/glad.h>    // OpenGL函数加载器

/**
 * @class Framebuffer
 * @brief OpenGL帧缓冲区对象的封装
 * 
 * 提供帧缓冲区的创建、绑定和管理功能。
 * 包含颜色附件和深度/模板附件的处理。
 */
class Framebuffer {
public:
    /**
     * @brief 构造帧缓冲区对象
     * @param width 缓冲区宽度
     * @param height 缓冲区高度
     */
    Framebuffer(int width, int height);
    
    /**
     * @brief 析构函数，释放OpenGL资源
     */
    ~Framebuffer();

    // 禁止拷贝
    Framebuffer(const Framebuffer&) = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;

    // 允许移动
    Framebuffer(Framebuffer&& other) noexcept;
    Framebuffer& operator=(Framebuffer&& other) noexcept;

    /**
     * @brief 绑定此帧缓冲区为当前渲染目标
     */
    void Bind() const;
    
    /**
     * @brief 解绑帧缓冲区，恢复默认帧缓冲
     */
    static void Unbind();
    
    /**
     * @brief 获取颜色附件纹理ID
     * @return OpenGL纹理对象ID
     */
    [[nodiscard]] GLuint GetTexture() const { return texture_; }

    /**
     * @brief 获取缓冲区宽度
     * @return 缓冲区宽度（像素）
     */
    [[nodiscard]] int GetWidth() const { return width_; }

    /**
     * @brief 获取缓冲区高度
     * @return 缓冲区高度（像素）
     */
    [[nodiscard]] int GetHeight() const { return height_; }

private:
    GLuint fbo_{0};          ///< 帧缓冲对象ID
    GLuint texture_{0};      ///< 颜色附件纹理ID
    GLuint rbo_{0};          ///< 渲染缓冲对象ID（深度/模板）
    int width_{0};           ///< 缓冲区宽度
    int height_{0};          ///< 缓冲区高度

    /**
     * @brief 检查帧缓冲区状态
     * @return 如果帧缓冲区完整则返回true
     */
    [[nodiscard]] bool CheckStatus() const;

    /**
     * @brief 清理OpenGL资源
     */
    void Cleanup() noexcept;
};
