// Camera.cpp
#include "Camera.h"
#include <cmath>

Camera::Camera(glm::vec3 position, glm::vec3 worldUp, float yaw, float pitch) noexcept
    : position_(position),
      worldUp_(glm::normalize(worldUp)),
      yaw_(yaw),
      pitch_(pitch),
      initialPosition_(position),
      initialYaw_(yaw),
      initialPitch_(pitch),
      initialZoom_(config.zoom) // 自动捕获初始zoom值
{
    updateVectors();
}

void Camera::reset() noexcept {
    position_ = initialPosition_;
    yaw_ = initialYaw_;
    pitch_ = initialPitch_;
    config.zoom = initialZoom_;
    updateVectors();
}


glm::mat4 Camera::getViewMatrix() const noexcept {
    return glm::lookAt(position_, position_ + front_, up_);
}

void Camera::processKeyboard(Movement direction, float deltaTime) noexcept {
    const float velocity = config.moveSpeed * deltaTime;
    
    switch (direction) {
    case Movement::Forward:  position_ += front_ * velocity; break;
    case Movement::Backward: position_ -= front_ * velocity; break;
    case Movement::Left:     position_ -= right_ * velocity; break;
    case Movement::Right:    position_ += right_ * velocity; break;
    case Movement::Up:       position_ += up_ * velocity;    break;
    case Movement::Down:     position_ -= up_ * velocity;    break;
    }
}

void Camera::processMouseMovement(float xOffset, float yOffset, bool constrainPitch) noexcept {
    xOffset *= config.mouseSensitivity;
    yOffset *= config.mouseSensitivity;

    yaw_   = std::fmod(yaw_ + xOffset, 360.0f);
    pitch_ += yOffset;

    if (constrainPitch) {
        pitch_ = glm::clamp(pitch_, config.pitchMin, config.pitchMax);
    }

    updateVectors();
}

void Camera::processMouseScroll(float yOffset) noexcept {
    config.zoom = glm::clamp(config.zoom - yOffset, 
                           config.zoomMin, 
                           config.zoomMax);
}

void Camera::updateVectors() noexcept {
    front_.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    front_.y = sin(glm::radians(pitch_));
    front_.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    
    front_ = glm::normalize(front_);
    right_ = glm::normalize(glm::cross(front_, worldUp_));
    up_    = glm::normalize(glm::cross(right_, front_));
}

void Camera::processPan(float xOffset, float yOffset) noexcept {
    position_ += right_ * xOffset;
    position_ += up_ * yOffset;
    updateVectors();
}
