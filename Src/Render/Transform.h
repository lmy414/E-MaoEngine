/**
 * @file Transform.h
 * @brief 3D变换组件
 * @author MirrorEngine Team
 * @date 2024
 */
#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <vector>
#include <algorithm> // 用于 std::remove

/**
 * @class Transform
 * @brief 表示3D空间中的变换组件
 * 
 * 管理实体在3D空间中的位置、旋转和缩放。
 * 支持层级结构，实现局部和全局空间的变换计算。
 * 使用脏标记机制优化矩阵计算。
 */
class Transform {
public:
    /**
     * @brief 默认构造函数
     */
    Transform() = default;

    /**
     * @brief 构造具有指定变换的组件
     * @param pos 初始位置
     * @param rot 初始旋转（四元数）
     * @param s 初始缩放
     */
    Transform(const glm::vec3& pos, const glm::quat& rot = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
             const glm::vec3& s = glm::vec3(1.0f))
        : position(pos), rotation(rot), scale(s) {}

    // 变换属性
    glm::vec3 position{0.0f};     ///< 局部空间位置
    glm::quat rotation{1.0f, 0.0f, 0.0f, 0.0f};  ///< 局部空间旋转（四元数）
    glm::vec3 scale{1.0f};        ///< 局部空间缩放
    Transform* parent{nullptr};    ///< 父节点指针

    /**
     * @brief 获取局部空间变换矩阵
     * @return 4x4变换矩阵
     */
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

    /**
     * @brief 获取全局空间变换矩阵
     * @return 4x4变换矩阵
     */
    glm::mat4 GetGlobalMatrix() const {
        if (parent) {
            return parent->GetGlobalMatrix() * GetLocalMatrix();
        }
        return GetLocalMatrix();
    }

    /**
     * @brief 标记变换状态为脏
     * 
     * 当变换属性发生改变时调用此函数，
     * 会递归标记所有子节点的变换状态为脏。
     */
    void MarkDirty() {
        if (!dirty) {
            dirty = true;
            for (auto& child : children) {
                child->MarkDirty();
            }
        }
    }

    /**
     * @brief 添加子节点
     * @param child 要添加的子节点
     */
    void AddChild(Transform* child) {
        if (child && child != this) {
            children.push_back(child);
            child->parent = this;
            child->MarkDirty(); // 父子关系变化时标记脏状态
        }
    }

    /**
     * @brief 移除子节点
     * @param child 要移除的子节点
     */
    void RemoveChild(Transform* child) {
        if (child) {
            children.erase(std::remove(children.begin(), children.end(), child), children.end());
            child->parent = nullptr;
            child->MarkDirty(); // 父子关系变化时标记脏状态
        }
    }

    /**
     * @brief 获取所有子节点
     * @return 子节点指针数组的常量引用
     */
    const std::vector<Transform*>& GetChildren() const { return children; }

private:
    mutable bool dirty = true;
    mutable glm::mat4 cachedMatrix = glm::mat4(1.0f); // 显式初始化
    std::vector<Transform*> children;
};
