#include "Material.h"
#include <iostream>

Material::Material(std::shared_ptr<Shader> shader)
    : shader(std::move(shader)) {}

void Material::Apply() {
    if (!shader || !shader->IsValid() || !dirty) return;
    
    shader->Use();
    
    // 设置标量参数
    for (const auto& [name, value] : floatParams) {
        shader->SetUniform(name, value);
    }
    
    // 设置向量参数
    for (const auto& [name, value] : vec3Params) {
        shader->SetUniform(name, value);
    }
    
    // 设置矩阵参数
    for (const auto& [name, value] : mat4Params) {
        shader->SetUniform(name, value);
    }
    
    // 绑定纹理
    int unit = 0;
    for (const auto& [name, texture] : textureSlots) {
        if (texture && texture->IsValid()) {
            texture->Bind(unit);
            shader->SetUniform(name, unit++);
        }
    }
    
    dirty = false; // 清除脏标记
}
