#pragma once
#include <glad/glad.h>
#include <string>
#include <unordered_map>
#include <stdexcept>

/**
 * @enum TextureLabel
 * @brief 纹理用途标签
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
 */
enum class TextureType {
    RGB,                 ///< RGB格式（线性色彩空间）
    RGBA,                ///< RGBA格式（线性色彩空间）
    SRGB,                ///< sRGB格式（gamma色彩空间）
    SRGBA                ///< sRGBA格式（gamma色彩空间）
};

/**
 * @class Texture
 * @brief OpenGL纹理封装类
 */
class Texture {
public:
    Texture() = default;

    // 禁用拷贝
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    // 启用移动
    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;

    /**
     * @brief 构造函数（不加载纹理）
     * @param path 纹理文件路径
     * @param label 纹理用途标签
     * @param type 纹理数据格式
     */
    Texture(const std::string& path, TextureLabel label, TextureType type);

    /**
     * @brief 加载纹理文件
     * @param path 纹理文件路径
     * @param label 纹理用途标签 (默认: BaseColor)
     * @param type 纹理数据格式 (默认: RGB)
     * @return 纹理对象的常量引用
     * @throws std::runtime_error 如果加载失败
     */
    static const Texture& Load(const std::string& path,
        TextureLabel label = TextureLabel::BaseColor,
        TextureType type = TextureType::RGB);

    ~Texture();

    /**
     * @brief 绑定纹理到指定纹理单元
     * @param unit 纹理单元索引 (默认: 0)
     */
    void Bind(unsigned int unit = 0) const;

    /**
     * @brief 检查纹理是否有效（已加载）
     * @return 如果纹理有效返回 true
     */
    [[nodiscard]] bool IsValid() const { return textureID != 0; }

    // Getters
    [[nodiscard]] GLuint GetID() const { return textureID; }         ///< 获取纹理ID
    [[nodiscard]] int GetWidth() const { return width; }             ///< 获取纹理宽度
    [[nodiscard]] int GetHeight() const { return height; }           ///< 获取纹理高度
    [[nodiscard]] TextureLabel GetLabel() const { return label; }    ///< 获取纹理标签
    [[nodiscard]] TextureType GetType() const { return type; }       ///< 获取纹理类型
    [[nodiscard]] const std::string& GetPath() const { return path; }///< 获取纹理路径

    /**
     * @brief 获取已加载的纹理
     * @param path 纹理文件路径
     * @return 纹理对象的常量引用
     * @throws std::runtime_error 如果纹理未加载
     */
    static const Texture& Get(const std::string& path);

    /**
     * @brief 清除所有缓存的纹理
     */
    static void ClearCache();

private:
    GLuint textureID{ 0 };              ///< OpenGL纹理对象ID
    TextureLabel label{};              ///< 纹理用途标签
    TextureType type{};                ///< 纹理数据格式
    std::string path;                  ///< 纹理文件路径
    int width{ 0 };                      ///< 纹理宽度
    int height{ 0 };                     ///< 纹理高度

    /**
     * @brief 从文件加载纹理数据
     * @throws std::runtime_error 如果加载失败或格式不支持
     */
    void LoadFromFile();

    /**
     * @brief 释放纹理资源
     */
    void Release();

    /// 纹理缓存（路径到纹理对象的映射）
    static std::unordered_map<std::string, Texture> textureCache;
};
