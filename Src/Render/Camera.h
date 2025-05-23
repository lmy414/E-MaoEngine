// Camera.h
#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    enum class Movement {
        Forward,
        Backward,
        Left,
        Right,
        Up,       // 新增垂直移动
        Down
    };

    struct Configuration {
        float moveSpeed = 4.5f;
        float mouseSensitivity = 0.1f;
        float zoom = 45.0f;
        float zoomMin = 1.0f;
        float zoomMax = 90.0f;
        float pitchMin = -89.0f;
        float pitchMax = 89.0f;
        float panSensitivity = 0.002f;  // 平移灵敏度
    };

    explicit Camera(
        glm::vec3 position = {0.0f, 0.0f, 3.0f},
        glm::vec3 worldUp = {0.0f, 1.0f, 0.0f},
        float yaw = -90.0f,
        float pitch = 0.0f
    ) noexcept;

    [[nodiscard]] glm::mat4 getViewMatrix() const noexcept;
    [[nodiscard]] float getZoom() const noexcept { return config.zoom; }

    void processKeyboard(Movement direction, float deltaTime) noexcept;
    void processMouseMovement(float xOffset, float yOffset, bool constrainPitch = true) noexcept;
    void processMouseScroll(float yOffset) noexcept;
    void processPan(float xOffset, float yOffset) noexcept;
    void reset() noexcept;

    // 访问器方法
    [[nodiscard]] const glm::vec3& position() const noexcept { return position_; }
    [[nodiscard]] glm::vec3 front() const noexcept { return front_; }
    [[nodiscard]] const Configuration& getConfig() const noexcept { return config; }
    void setPosition(const glm::vec3& newPos) noexcept { position_ = newPos; }

private:
    void updateVectors() noexcept;
    Configuration config;
    glm::vec3 position_;
    glm::vec3 front_;
    glm::vec3 up_;
    glm::vec3 right_;
    glm::vec3 worldUp_;
    //默认参数
    glm::vec3 initialPosition_;
    float initialYaw_;
    float initialPitch_;
    float initialZoom_;

    
    float yaw_;
    float pitch_;
};
