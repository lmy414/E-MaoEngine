// BasicMaterial.cpp
#include "Material.h"

Shader* BasicMaterial::shader = nullptr;

BasicMaterial::BasicMaterial() {
    if (!shader) {
        static Shader instance("shaders/basic.vert", "shaders/basic.frag");
        shader = &instance;
    }
}

void BasicMaterial::Apply() {
    shader->use();
    if (DiffuseTexture) {
        DiffuseTexture->Bind(0);
        shader->setInt("uTexture", 0);
        shader->setBool("uUseTexture", true);
    } else {
        shader->setBool("uUseTexture", false);
    }
    shader->setVec3("uColor", Color);
}

