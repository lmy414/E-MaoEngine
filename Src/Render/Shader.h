#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>
#include <type_traits>

class Shader {
public:
    Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
    ~Shader();

    bool IsValid() const { return ID != 0; }

    void Use() const;
    
    template<typename T>
    void SetUniform(const std::string& name, const T& value) const {
        if constexpr (std::is_same_v<T, bool>) {
            SetBool(name, value);
        } else if constexpr (std::is_same_v<T, int>) {
            SetInt(name, value);
        } else if constexpr (std::is_same_v<T, float>) {
            SetFloat(name, value);
        } else if constexpr (std::is_same_v<T, glm::vec3>) {
            SetVec3(name, value);
        } else if constexpr (std::is_same_v<T, glm::mat4>) {
            SetMat4(name, value);
        } else {
            static_assert(sizeof(T) == 0, "Unsupported uniform type");
        }
    }

private:
    GLuint ID = 0;
    mutable std::unordered_map<std::string, GLint> uniformCache;

    // 主模板声明
    void SetBool(const std::string& name, bool value) const;
    void SetInt(const std::string& name, int value) const;
    void SetFloat(const std::string& name, float value) const;
    // GLM向量/矩阵设置
    void SetVec3(const std::string& name, const glm::vec3& value) const;
    void SetMat4(const std::string& name, const glm::mat4& value) const;
    
    GLint GetLocation(const std::string& name) const;
    void CheckCompileErrors(GLuint shader, const std::string& type) const;
};
