// Texture.cpp
#include "Texture.h"
#include "Resources/stb_image.h"
#include <iostream>
#include <stdexcept>

std::unordered_map<std::string, Texture> Texture::textureCache;

Texture::Texture(const std::string& path, TextureLabel label, TextureType type)
    : path(path), label(label), type(type) {}

Texture::Texture(Texture&& other) noexcept 
    : textureID(other.textureID),
      label(other.label),
      type(other.type),
      path(std::move(other.path)),
      width(other.width),
      height(other.height) {
    other.textureID = 0;
    other.width = 0;
    other.height = 0;
}

Texture& Texture::operator=(Texture&& other) noexcept {
    if (this != &other) {
        Release();
        textureID = other.textureID;
        label = other.label;
        type = other.type;
        path = std::move(other.path);
        width = other.width;
        height = other.height;
        other.textureID = 0;
        other.width = 0;
        other.height = 0;
    }
    return *this;
}

const Texture& Texture::Load(const std::string& path, 
                            TextureLabel label,
                            TextureType type) {
    auto it = textureCache.find(path);
    if (it != textureCache.end()) {
        return it->second;
    }
    
    Texture newTex(path, label, type);
    newTex.LoadFromFile();
    
    if (!newTex.IsValid()) {
        throw std::runtime_error("Failed to load texture: " + path);
    }
    
    auto [iter, success] = textureCache.emplace(path, std::move(newTex));
    return iter->second;
}

void Texture::LoadFromFile() {
    stbi_set_flip_vertically_on_load(true);
    
    int channels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    if (!data) {
        std::cerr << "Failed to load texture: " << path << std::endl;
        return;
    }

    GLenum format;
    switch (channels) {
        case 1: format = GL_RED; break;
        case 3: format = (type == TextureType::SRGB) ? GL_SRGB : GL_RGB; break;
        case 4: format = (type == TextureType::SRGBA) ? GL_SRGB_ALPHA : GL_RGBA; break;
        default: 
            stbi_image_free(data);
            throw std::runtime_error("Unsupported texture format: " + path);
    }

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 
                0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // 默认纹理参数（可根据需要扩展配置）
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
}

Texture::~Texture() {
    Release();
}

void Texture::Bind(unsigned int unit) const {
    if (!IsValid()) return;
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, textureID);
}

void Texture::Release() {
    if (textureID != 0) {
        glDeleteTextures(1, &textureID);
        textureID = 0;
    }
}

const Texture& Texture::Get(const std::string& path) {
    auto it = textureCache.find(path);
    if (it == textureCache.end()) {
        throw std::runtime_error("Texture not loaded: " + path);
    }
    return it->second;
}

void Texture::ClearCache() {
    for (auto& pair : textureCache) {
        pair.second.Release();
    }
    textureCache.clear();
}
