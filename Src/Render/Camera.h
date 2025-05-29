/**
 * @file Camera.h
 * @brief 3D相机组件
 * @author MirrorEngine Team
 * @date 2024
 */
#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

/**
 * @class Camera
 * @brief 3D场景相机控制器
 * 
 * 提供第一人称/自由视角相机的控制功能，
 * 支持键盘移动、鼠标旋转、缩放和平移操作。
 */
class Camera {
public:
    /**
     * @enum Movement
     * @brief 相机移动方向枚举
     */
    enum class Movement {
        Forward,    ///< 向前移动
        Backward,   ///< 向后移动
        Left,       ///< 向左移动
        Right,      ///< 向右移动
        Up,         ///< 向上移动
        Down        ///< 向下移动
    };

    /**
     * @struct Configuration
     * @brief 相机配置参数
     */
    struct Configuration {
        float moveSpeed{4.5f};          ///< 移动速度
        float mouseSensitivity{0.1f};   ///< 鼠标灵敏度
        float zoom{45.0f};              ///< 当前视野角度
        float zoomMin{1.0f};            ///< 最小视野角度
        float zoomMax{90.0f};           ///< 最大视野角度
        float pitchMin{-89.0f};         ///< 最小俯仰角度
        float pitchMax{89.0f};          ///< 最大俯仰角度
        float panSensitivity{0.002f};   ///< 平移灵敏度
    };

    /**
     * @brief 构造相机对象
     * @param position 初始位置
     * @param worldUp 世界空间的上方向
     * @param yaw 初始偏航角（度）
     * @param pitch 初始俯仰角（度）
     */
    explicit Camera(
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f),
        glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f),
        float yaw = -90.0f,
        float pitch = 0.0f
    ) noexcept;

    /**
     * @brief 获取观察矩阵
     * @return 4x4观察矩阵
     */
    [[nodiscard]] glm::mat4 GetViewMatrix() const noexcept;

    /**
     * @brief 获取当前视野角度
     * @return 视野角度（度）
     */
    [[nodiscard]] float GetZoom() const noexcept { return config.zoom; }

    /**
     * @brief 处理键盘输入
     * @param direction 移动方向
     * @param deltaTime 帧时间间隔
     */
    void ProcessKeyboard(Movement direction, float deltaTime) noexcept;

    /**
     * @brief 处理鼠标移动
     * @param xOffset X轴偏移量
     * @param yOffset Y轴偏移量
     * @param constrainPitch 是否限制俯仰角度
     */
    void ProcessMouseMovement(float xOffset, float yOffset, bool constrainPitch = true) noexcept;

    /**
     * @brief 处理鼠标滚轮
     * @param yOffset Y轴偏移量
     */
    void ProcessMouseScroll(float yOffset) noexcept;

    /**
     * @brief 处理相机平移
     * @param xOffset X轴偏移量
     * @param yOffset Y轴偏移量
     */
    void ProcessPan(float xOffset, float yOffset) noexcept;

    /**
     * @brief 重置相机到初始状态
     */
    void Reset() noexcept;

    // Getters
    [[nodiscard]] const glm::vec3& GetPosition() const noexcept { return position_; }
    [[nodiscard]] glm::vec3 GetFront() const noexcept { return front_; }
    [[nodiscard]] const Configuration& GetConfig() const noexcept { return config; }
    
    // Setters
    void SetPosition(const glm::vec3& newPos) noexcept { position_ = newPos; }

private:
    /**
     * @brief 更新相机向量
     */
    void UpdateVectors() noexcept;

    Configuration config;            ///< 相机配置
    
    // 相机状态
    glm::vec3 position_;            ///< 当前位置
    glm::vec3 front_;              ///< 前方向向量
    glm::vec3 up_;                 ///< 上方向向量
    glm::vec3 right_;              ///< 右方向向量
    glm::vec3 worldUp_;            ///< 世界空间上方向
    
    // 初始状态
    glm::vec3 initialPosition_;     ///< 初始位置
    float initialYaw_;              ///< 初始偏航角
    float initialPitch_;            ///< 初始俯仰角
    float initialZoom_;             ///< 初始视野角度
    
    // 当前欧拉角
    float yaw_;                     ///< 当前偏航角
    float pitch_;                   ///< 当前俯仰角
};
