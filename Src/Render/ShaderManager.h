#pragma once
#include "Shader.h"
#include <map>
#include <memory>
#include <string>
#include <mutex>

class ShaderManager {
public:
    static void Initialize();
    static std::shared_ptr<Shader> Get(const std::string& name);
    static void Reload(const std::string& name);
    static void LoadShader(const std::string& name,
                         const std::string& vertPath,
                         const std::string& fragPath);

private:
    inline static std::map<std::string, std::shared_ptr<Shader>> shaders;
    inline static std::mutex mutex;
    
    static std::string LoadShaderSource(const std::string& path);
    static std::shared_ptr<Shader> CreateShader(
        const std::string& vertSrc,
        const std::string& fragSrc
    );
};
