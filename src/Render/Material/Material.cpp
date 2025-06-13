#include "Material.h"
#include <iostream>

Material::Material(std::shared_ptr<Shader> shader)
    : shader(std::move(shader)) {}

void Material::Apply() {
    if (!shader || !shader->IsValid() || !dirty) return;
    
    shader->Use();
    
    // ���ñ�������
    for (const auto& [name, value] : floatParams) {
        shader->SetUniform(name, value);
    }
    
    // ������������
    for (const auto& [name, value] : vec3Params) {
        shader->SetUniform(name, value);
    }
    
    // ���þ������
    for (const auto& [name, value] : mat4Params) {
        shader->SetUniform(name, value);
    }
    
    // ������
    int unit = 0;
    for (const auto& [name, texture] : textureSlots) {
        if (texture && texture->IsValid()) {
            texture->Bind(unit);
            shader->SetUniform(name, unit++);
        }
    }
    
    dirty = false; // �������
}
