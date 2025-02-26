// DefaultMaterial.cpp
#include "Material.h"

Shader* DefaultMaterial::shader = nullptr;

DefaultMaterial::DefaultMaterial() {
    if (!shader) {
        static Shader instance("E:/MirrorEngine/MirrorEngine2/Shaders/default_V.shader", "E:/MirrorEngine/MirrorEngine2/Shaders/default_F.shader");
        shader = &instance;
    }
}

void DefaultMaterial::Apply() {
    shader->use();
    shader->setVec3("uColor", Color);
}
