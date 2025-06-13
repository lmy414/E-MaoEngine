// DefaultMaterial.h
#pragma once
#include "Material.h"
#include "../ShaderManager.h"

class DefaultMaterial : public Material {
    glm::vec3 m_ColorCache{ 1.0f, 1.0f, 1.0f };
    
    static constexpr const char* COLOR_PARAM_NAME = "uColor";
public:
    DefaultMaterial() : Material(ShaderManager::Get("Default")) {
        SetColor(m_ColorCache);
    }
    
    // 保持与SetVector3一致的参数传递风格
    void SetColor(const glm::vec3& value) {
        m_ColorCache = value;
        
        // 复用基类逻辑（参考你的现有代码结构）
        // ↓↓↓ 与SetVector3相同的存储和标记方式 ↓↓↓
        vec3Params[COLOR_PARAM_NAME] = value;
        dirty = true;
        
        // 或者直接调用基类方法（如果继承可见性允许）
        // Material::SetVector3(COLOR_PARAM_NAME, value);
    }
    // 保持原有通用参数接口的访问性
    using Material::SetVector3;
    // 保持与SetVector3风格一致的获取方法
    const glm::vec3& GetColor() const { 
        return m_ColorCache; 
    }

};
