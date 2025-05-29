/**
 * @file Texture.h
 * @brief 纹理资源管理
 * @author MirrorEngine Team
 * @date 2024
 */
#pragma once
#include <glad/glad.h>
#include <string>
#include <unordered_map>

/**
 * @enum TextureLabel
 * @brief 纹理用途标签
 * 
 * 用于标识PBR材质系统中不同类型的纹理贴图。
 */
enum class TextureLabel {
    BaseColor,           ///< 基础颜色贴图
    Normal,              ///< 法线贴图
    Specular,            ///< 高光贴图
    Roughness,           ///< 粗糙度贴图
    Metallic,            ///< 金属度贴图
    AmbientOcclusion     ///< 环境光遮蔽贴图
};

/**
 * @enum TextureType
 * @brief 纹理数据格式
 * 
 * 定义纹理数据的颜色格式和色彩空间。
 */
enum class TextureType {
    RGB,                 ///< RGB格式（线性色彩空间）
    RGBA,                ///< RGBA格式（线性色彩空间）
    SRGB,                ///< sRGB格式（gamma色彩空间）
    SRGBA                ///< sRGBA格式（gamma色彩空间）
};

/**
 * @class Texture
 * @brief 纹理资源管理类
 * 
 * 提供纹理资源的加载、缓存和管理功能。
 * 支持多种纹理格式和用途，实现资源的自动管理。
 */
class Texture {
public:
    /**
     * @brief 默认构造函数
     */
    Texture() = default;
    
    // 禁用拷贝
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    // 启用移动
    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;

    /**
     * @brief 构造并延迟加载纹理
     * @param path 纹理文件路径
     * @param label 纹理用途标签
     * @param type 纹理数据格式
     */
    Texture(const std::string& path, TextureLabel label, TextureType type);
    
    /**
     * @brief 加载或获取已缓存的纹理
     * @param path 纹理文件路径
     * @param label 纹理用途标签
     * @param type 纹理数据格式
     * @return 纹理对象的常量引用
     */
    static const Texture& Load(const std::string& path, 
                             TextureLabel label = TextureLabel::BaseColor,
                             TextureType type = TextureType::RGB);
    
    ~Texture();
    
    /**
     * @brief 绑定纹理到指定纹理单元
     * @param unit 纹理单元索引
     */
    void Bind(unsigned int unit = 0) const;

    /**
     * @brief 检查纹理是否有效
     * @return 如果纹理已成功加载则返回true
     */
    [[nodiscard]] bool IsValid() const { return texture_id_ != 0; }
    
    // Getters
    [[nodiscard]] GLuint GetID() const { return texture_id_; }
    [[nodiscard]] int GetWidth() const { return width_; }
    [[nodiscard]] int GetHeight() const { return height_; }
    [[nodiscard]] TextureLabel GetLabel() const { return label_; }
    [[nodiscard]] TextureType GetType() const { return type_; }
    [[nodiscard]] const std::string& GetPath() const { return path_; }

    /**
     * @brief 获取已缓存的纹理
     * @param path 纹理文件路径
     * @return 纹理对象的常量引用
     * @throws std::runtime_error 如果纹理未找到
     */
    static const Texture& Get(const std::string& path);

    /**
     * @brief 清理纹理缓存
     */
    static void ClearCache();

private:
    GLuint texture_id_{0};                ///< OpenGL纹理对象ID
    TextureLabel label_{};                ///< 纹理用途标签
    TextureType type_{};                  ///< 纹理数据格式
    std::string path_;                    ///< 纹理文件路径
    int width_{0};                        ///< 纹理宽度
    int height_{0};                       ///< 纹理高度
    
    /**
     * @brief 从文件加载纹理数据
     */
    void LoadFromFile();

    /**
     * @brief 释放纹理资源
     */
    void Release();

    static std::unordered_map<std::string, Texture> texture_cache_;  ///< 纹理缓存
};
