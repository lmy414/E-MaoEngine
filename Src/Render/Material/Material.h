// Material.h
#pragma once
#include "../Shader.h"
#include "../Texture.h"
#include <glm/glm.hpp>

// 基类声明
class Material {
public:
    virtual ~Material() = default;
    virtual void Apply() = 0;
};

// 默认材质声明
class DefaultMaterial : public Material {
public:
    glm::vec3 Color = glm::vec3(1.0f);
    DefaultMaterial();
    void Apply() override;
private:
    static Shader* shader;
};

// 基础材质声明
class BasicMaterial : public Material {
public:
    glm::vec3 Color = glm::vec3(1.0f);
    const Texture* DiffuseTexture = nullptr;
    BasicMaterial();
    void Apply() override;
private:
    static Shader* shader;
};

// PBR材质声明（示例）
class PBRMaterial : public Material {
public:
    // PBR参数声明...
    void Apply() override;
};
