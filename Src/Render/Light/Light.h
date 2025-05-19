// 在Common.h中添加结构体定义
#pragma once
#include <glm/glm.hpp>

struct Light {
    glm::vec3 direction = glm::vec3(-0.2f, -1.0f, -0.3f);
    glm::vec3 color = glm::vec3(1.0f);
    float intensity = 1.0f;
};
