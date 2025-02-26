// Texture.h
#pragma once
#include <glad/glad.h>
#include <string>
#include <unordered_map>

enum class TextureLabel {
    BaseColor,
    Normal,
    Specular,
    Roughness,
    Metallic,
    AmbientOcclusion
};

enum class TextureType {
    RGB,
    RGBA,
    SRGB,
    SRGBA
};

class Texture {
public:
    Texture() = default;
    
    // 构造函数（延迟加载）
    Texture(const std::string& path, TextureLabel label, TextureType type);
    
    // 构造函数（立即加载）
    static Texture Load(const std::string& path, TextureLabel label, TextureType type);
    
    // 析构函数
    ~Texture();
    
    // 绑定纹理到指定纹理单元
    void Bind(unsigned int unit = 0) const;
    
    // 属性访问
    GLuint GetID() const { return textureID; }
    int GetWidth() const { return width; }
    int GetHeight() const { return height; }
    TextureLabel GetLabel() const { return label; }
    TextureType GetType() const { return type; }
    const std::string& GetPath() const { return path; }

    // 静态纹理管理器
    static const Texture& Get(const std::string& path);
    static void ClearCache();

private:
    GLuint textureID;
    std::string name;
    TextureLabel label;
    TextureType type;
    std::string path;
    int width;
    int height;
    
    // 私有加载方法
    void LoadFromFile();
    
    // 纹理缓存
    static std::unordered_map<std::string, Texture> textureCache;
};
