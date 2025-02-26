#include "Texture.h"
#include "../Resources/stb_image.h"
#include <iostream>

std::unordered_map<std::string, Texture> Texture::textureCache;

Texture::Texture(const std::string& path, TextureLabel label, TextureType type)
    : path(path), label(label), type(type), textureID(0), width(0), height(0) {}

Texture Texture::Load(const std::string& path, TextureLabel label, TextureType type) {
    auto it = textureCache.find(path);
    if (it != textureCache.end()) {
        return it->second;
    }
    
    Texture newTexture(path, label, type);
    newTexture.LoadFromFile();
    textureCache[path] = newTexture;
    return newTexture;
}

void Texture::LoadFromFile() {
    stbi_set_flip_vertically_on_load(true);
    
    int nrChannels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (!data) {
        std::cerr << "Failed to load texture: " << path << std::endl;
        return;
    }

    GLenum format;
    switch (nrChannels) {
        case 1: format = GL_RED; break;
        case 3: format = (type == TextureType::SRGB) ? GL_SRGB : GL_RGB; break;
        case 4: format = (type == TextureType::SRGBA) ? GL_SRGB_ALPHA : GL_RGBA; break;
        default: format = GL_RGB; break;
    }

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // 默认纹理参数
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
}

Texture::~Texture() {
    if (textureID != 0) {
        glDeleteTextures(1, &textureID);
    }
}

void Texture::Bind(unsigned int unit) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, textureID);
}

const Texture& Texture::Get(const std::string& path) {
    auto it = textureCache.find(path);
    if (it == textureCache.end()) {
        throw std::runtime_error("Texture not loaded: " + path);
    }
    return it->second;
}

void Texture::ClearCache() {
    textureCache.clear();
}
