/**
 * @file Shader.h
 * @brief OpenGL着色器程序封装类
 * @author MirrorEngine Team
 * @date 2024
 */
#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>
#include <type_traits>

/**
 * @class Shader
 * @brief OpenGL着色器程序的高级封装
 * 
 * 提供着色器程序的创建、编译、链接和uniform变量设置等功能。
 * 支持类型安全的uniform设置，并实现uniform位置的缓存优化。
 */
class Shader {
public:
    /**
     * @brief 构造新的着色器程序
     * @param vertexSrc 顶点着色器源代码
     * @param fragmentSrc 片段着色器源代码
     */
    Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
    ~Shader();

    // 禁止拷贝
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    /**
     * @brief 检查着色器程序是否有效
     * @return 如果着色器程序已成功编译和链接则返回true
     */
    bool IsValid() const { return ID != 0; }

    /**
     * @brief 激活着色器程序
     */
    void Use() const;
    
    /**
     * @brief 设置uniform变量的值
     * @tparam T uniform变量的类型
     * @param name uniform变量的名称
     * @param value 要设置的值
     * @throws static_assert 如果T是不支持的类型
     */
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
    GLuint ID{0};  // 着色器程序ID
    mutable std::unordered_map<std::string, GLint> uniformCache;  // uniform位置缓存

    /**
     * @brief 设置bool类型的uniform变量
     */
    void SetBool(const std::string& name, bool value) const;

    /**
     * @brief 设置int类型的uniform变量
     */
    void SetInt(const std::string& name, int value) const;

    /**
     * @brief 设置float类型的uniform变量
     */
    void SetFloat(const std::string& name, float value) const;

    /**
     * @brief 设置vec3类型的uniform变量
     */
    void SetVec3(const std::string& name, const glm::vec3& value) const;

    /**
     * @brief 设置mat4类型的uniform变量
     */
    void SetMat4(const std::string& name, const glm::mat4& value) const;
    
    /**
     * @brief 获取uniform变量的位置
     * @param name uniform变量的名称
     * @return uniform变量的位置
     */
    GLint GetLocation(const std::string& name) const;

    /**
     * @brief 检查着色器编译错误
     * @param shader 着色器对象ID
     * @param type 着色器类型描述字符串
     */
    void CheckCompileErrors(GLuint shader, const std::string& type) const;
};
