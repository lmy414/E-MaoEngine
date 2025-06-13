#include "ShaderManager.h"
#include <fstream>
#include <sstream>
#include <iostream>

//std::map<std::string, std::shared_ptr<Shader>> ShaderManager::shaders;
//std::mutex ShaderManager::mutex;

void ShaderManager::Initialize() {
    LoadShader("Default", "E:/MirrorEngine/MirrorEngine2/Shaders/default_V.shader", "E:/MirrorEngine/MirrorEngine2/Shaders/default_F.shader");
    //LoadShader("Basic", "Shaders/basic.vert", "Shaders/basic.frag");
    //LoadShader("PBR", "Shaders/pbr.vert", "Shaders/pbr.frag");
}

std::shared_ptr<Shader> ShaderManager::Get(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex);
    if (auto it = shaders.find(name); it != shaders.end()) {
        return it->second;
    }
    throw std::runtime_error("Shader not found: " + name);
}

void ShaderManager::Reload(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex);
    if (auto it = shaders.find(name); it != shaders.end()) {
        try {
            // 获取原路径（需在初次加载时存储）
            auto vertPath = "Shaders/" + name + ".vert"; // 实际应根据存储的路径
            auto fragPath = "Shaders/" + name + ".frag";
            
            auto newShader = CreateShader(
                LoadShaderSource(vertPath),
                LoadShaderSource(fragPath)
            );
            
            // 原子替换
            it->second.swap(newShader);
            std::cout << "Reloaded shader: " << name << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Reload failed: " << e.what() << std::endl;
        }
    }
}

void ShaderManager::LoadShader(const std::string& name,
                              const std::string& vertPath,
                              const std::string& fragPath) {
    std::lock_guard<std::mutex> lock(mutex);
    try {
        auto vertSrc = LoadShaderSource(vertPath);
        auto fragSrc = LoadShaderSource(fragPath);
        shaders[name] = CreateShader(vertSrc, fragSrc);
        std::cout << "Loaded shader: " << name << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Load failed: " << e.what() << std::endl;
        throw;
    }
}

std::string ShaderManager::LoadShaderSource(const std::string& path) {
    std::ifstream file(path, std::ios::in);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open: " + path);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::shared_ptr<Shader> ShaderManager::CreateShader(
    const std::string& vertSrc,
    const std::string& fragSrc
) {
    try {
        return std::make_shared<Shader>(vertSrc, fragSrc);
    } catch (const std::exception& e) {
        throw std::runtime_error("Shader creation failed: " + std::string(e.what()));
    }
}
