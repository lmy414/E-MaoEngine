#include "Shader.h"
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

// 构造函数和基础方法实现
Shader::Shader(const std::string& vertexSrc, const std::string& fragmentSrc) {
    const char* vShaderCode = vertexSrc.c_str();
    const char* fShaderCode = fragmentSrc.c_str();

    GLuint vertex, fragment;
    
    // 顶点着色器编译
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, nullptr);
    glCompileShader(vertex);
    CheckCompileErrors(vertex, "VERTEX");

    // 片段着色器编译
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, nullptr);
    glCompileShader(fragment);
    CheckCompileErrors(fragment, "FRAGMENT");

    // 链接着色器程序
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    CheckCompileErrors(ID, "PROGRAM");

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Shader::~Shader() {
    if (ID != 0) {
        glDeleteProgram(ID);
    }
}

void Shader::Use() const {
    glUseProgram(ID);
}

// Uniform 位置缓存
GLint Shader::GetLocation(const std::string& name) const {
    auto it = uniformCache.find(name);
    if (it != uniformCache.end()) {
        return it->second;
    }

    GLint location = glGetUniformLocation(ID, name.c_str());
    if (location == -1) {
        std::cerr << "[Shader Warning] Uniform '" << name 
                 << "' not found or optimized out\n";
    }

    uniformCache[name] = location;
    return location;
}

// 基础类型设置实现
void Shader::SetBool(const std::string& name, bool value) const {
    glUniform1i(GetLocation(name), (int)value);
}

void Shader::SetInt(const std::string& name, int value) const {
    glUniform1i(GetLocation(name), value);
}

void Shader::SetFloat(const std::string& name, float value) const {
    glUniform1f(GetLocation(name), value);
}

// GLM类型设置实现
void Shader::SetVec3(const std::string& name, const glm::vec3& value) const {
    glUniform3fv(GetLocation(name), 1, glm::value_ptr(value));
}

void Shader::SetMat4(const std::string& name, const glm::mat4& value) const {
    glUniformMatrix4fv(GetLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

// 编译错误检查
void Shader::CheckCompileErrors(GLuint shader, const std::string& type) const {
    GLint success;
    GLchar infoLog[1024];

    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
            std::cerr << "SHADER_COMPILATION_ERROR [" << type << "]\n"
                      << infoLog << "\n-----------------------------------------\n";
            throw std::runtime_error("Shader compilation failed");
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
            std::cerr << "PROGRAM_LINKING_ERROR\n"
                      << infoLog << "\n-----------------------------------------\n";
            throw std::runtime_error("Shader program linking failed");
        }
    }
}
