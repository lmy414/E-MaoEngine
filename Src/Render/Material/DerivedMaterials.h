#pragma once
#include "Material.h"
#include "../ShaderManager.h"

// 默认材质（无需纹理）
class DefaultMaterial : public Material {
public:
    DefaultMaterial() 
        : Material(ShaderManager::Get("Default")) 
    {
        SetVector3("uColor", glm::vec3(1.0f));
    }
};

// 基础漫反射材质


/*// PBR材质
class PBRMaterial : public Material {
public:
    PBRMaterial()
        : Material(ShaderManager::Get("PBR")) 
    {
        SetFloat("uMetallic", 0.0f);
        SetFloat("uRoughness", 0.5f);
        
        SetTexture("uAlbedo", "default_white.png",
                  TextureLabel::BaseColor,
                  TextureType::SRGB);
        SetTexture("uNormal", "default_normal.png",
                  TextureLabel::Normal,
                  TextureType::RGB);
        SetTexture("uMetallicRoughness", "default_metal_rough.png",
                  TextureLabel::Metallic,
                  TextureType::RGB);
    }

    void SetAlbedo(const std::string& path) {
        SetTexture("uAlbedo", path,
                  TextureLabel::BaseColor,
                  TextureType::SRGB);
    }

    void SetNormalMap(const std::string& path) {
        SetTexture("uNormal", path,
                  TextureLabel::Normal,
                  TextureType::RGB);
    }
};*/
