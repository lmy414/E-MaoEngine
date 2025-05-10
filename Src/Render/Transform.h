// Transform.h
#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <vector>
#include <algorithm> // 用于 std::remove

class Transform {
public:
    glm::vec3 position = {0, 0, 0};
    glm::quat rotation = glm::quat(1, 0, 0, 0);
    glm::vec3 scale = {1, 1, 1};
    Transform* parent = nullptr;

    // 获取局部空间矩阵（带缓存）
    glm::mat4 GetLocalMatrix() const {
        if (dirty) {
            // 重新计算并更新缓存
            glm::mat4 translation = glm::translate(glm::mat4(1.0f), position);
            glm::mat4 rotationMat = glm::toMat4(rotation);
            glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), scale);
            cachedMatrix = translation * rotationMat * scaleMat;
            dirty = false;
        }
        return cachedMatrix;
    }

    // 获取全局空间矩阵（带层级计算）
    glm::mat4 GetGlobalMatrix() const {
        if (parent) {
            return parent->GetGlobalMatrix() * GetLocalMatrix();
        }
        return GetLocalMatrix();
    }

    // 标记脏状态并传播到子节点
    void MarkDirty() {
        if (!dirty) {
            dirty = true;
            for (auto& child : children) {
                child->MarkDirty();
            }
        }
    }

    // 维护父子关系
    void AddChild(Transform* child) {
        if (child && child != this) {
            children.push_back(child);
            child->parent = this;
            child->MarkDirty(); // 父子关系变化时标记脏状态
        }
    }

    void RemoveChild(Transform* child) {
        if (child) {
            children.erase(std::remove(children.begin(), children.end(), child), children.end());
            child->parent = nullptr;
            child->MarkDirty(); // 父子关系变化时标记脏状态
        }
    }

private:
    mutable bool dirty = true;
    mutable glm::mat4 cachedMatrix = glm::mat4(1.0f); // 显式初始化
    std::vector<Transform*> children;
};
