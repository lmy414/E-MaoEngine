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
    
    // 禁用拷贝
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    // 移动语义支持
    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;

    // 带参数的构造函数（延迟加载）
    Texture(const std::string& path, TextureLabel label, TextureType type);
    
    // 静态加载方法（返回const引用）
    static const Texture& Load(const std::string& path, 
                              TextureLabel label = TextureLabel::BaseColor,
                              TextureType type = TextureType::RGB);
    
    ~Texture();
    
    void Bind(unsigned int unit = 0) const;
    bool IsValid() const { return textureID != 0; }
    
    GLuint GetID() const { return textureID; }
    int GetWidth() const { return width; }
    int GetHeight() const { return height; }
    TextureLabel GetLabel() const { return label; }
    TextureType GetType() const { return type; }
    const std::string& GetPath() const { return path; }

    static const Texture& Get(const std::string& path);
    static void ClearCache();

private:
    GLuint textureID = 0;
    TextureLabel label;
    TextureType type;
    std::string path;
    int width = 0;
    int height = 0;
    
    void LoadFromFile();
    void Release();

    static std::unordered_map<std::string, Texture> textureCache;
};
