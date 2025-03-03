#pragma once
#include <memory>
#include "../Shader.h"
#include "../Texture.h"
#include <glm/glm.hpp>
#include <unordered_map>

class Material {
public:
    using Ptr = std::shared_ptr<Material>;

    int GetRenderQueue() const { return renderQueue; }
    bool IsTransparent() const { return renderQueue > 2500; }
    void SetRenderQueue(int queue) { renderQueue = queue; }
    
    explicit Material(std::shared_ptr<Shader> shader);
    virtual ~Material() = default;
    
    // 支持拷贝构造
    Material(const Material&) = default;
    Material& operator=(const Material&) = default;

    bool IsValid() const { 
        return shader != nullptr && shader->IsValid(); 
    }
    
    virtual void Apply();

    // 参数设置接口
    void SetFloat(const std::string& name, float value) { 
        floatParams[name] = value; 
        dirty = true;
    }
    
    void SetVector3(const std::string& name, const glm::vec3& value) { 
        vec3Params[name] = value;
        dirty = true;
    }
    
    void SetMatrix4(const std::string& name, const glm::mat4& value) { 
        mat4Params[name] = value;
        dirty = true;
    }
    
    void SetTexture(const std::string& uniformName,
                   const std::shared_ptr<Texture>& texture) {
        textureSlots[uniformName] = texture;
        dirty = true;
    }

protected:
    std::shared_ptr<Shader> shader;
    bool dirty = true; // 优化标记
    int renderQueue = 2000;
    
    // 参数存储
    std::unordered_map<std::string, float> floatParams;
    std::unordered_map<std::string, glm::vec3> vec3Params;
    std::unordered_map<std::string, glm::mat4> mat4Params;
    
    // 使用智能指针管理纹理
    std::unordered_map<std::string, std::shared_ptr<Texture>> textureSlots;
};
