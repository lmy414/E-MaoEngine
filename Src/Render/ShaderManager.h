/**
 * @file ShaderManager.h
 * @brief 着色器资源管理器
 * @author MirrorEngine Team
 * @date 2024
 */
#pragma once
#include "Shader.h"
#include <map>
#include <memory>
#include <string>
#include <mutex>

/**
 * @class ShaderManager
 * @brief 管理着色器程序的创建、缓存和重载
 * 
 * 该类使用单例模式实现，提供着色器资源的集中管理。
 * 支持着色器的懒加载、重载和资源共享。
 */
class ShaderManager {
public:
    /**
     * @brief 初始化着色器管理器
     */
    static void Initialize();

    /**
     * @brief 获取指定名称的着色器
     * @param name 着色器名称
     * @return 着色器的共享指针
     */
    static std::shared_ptr<Shader> Get(const std::string& name);

    /**
     * @brief 重新加载指定的着色器
     * @param name 要重载的着色器名称
     */
    static void Reload(const std::string& name);

    /**
     * @brief 加载新的着色器
     * @param name 着色器名称
     * @param vertPath 顶点着色器路径
     * @param fragPath 片段着色器路径
     */
    static void LoadShader(const std::string& name,
                          const std::string& vertPath,
                          const std::string& fragPath);

private:
    inline static std::map<std::string, std::shared_ptr<Shader>> shaders;
    inline static std::mutex mutex;
    
    /**
     * @brief 从文件加载着色器源代码
     * @param path 着色器文件路径
     * @return 着色器源代码字符串
     */
    static std::string LoadShaderSource(const std::string& path);

    /**
     * @brief 创建新的着色器程序
     * @param vertSrc 顶点着色器源代码
     * @param fragSrc 片段着色器源代码
     * @return 创建的着色器程序
     */
    static std::shared_ptr<Shader> CreateShader(
        const std::string& vertSrc,
        const std::string& fragSrc
    );

    // 禁止实例化
    ShaderManager() = delete;
    ~ShaderManager() = delete;
    ShaderManager(const ShaderManager&) = delete;
    ShaderManager& operator=(const ShaderManager&) = delete;
};
