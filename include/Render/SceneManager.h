#pragma once
#include <vector>
#include <memory>
#include <algorithm>
#include "Render/Light/Light.h"
#include "Entity.h"

class SceneManager {
public:
    void AddEntity(std::shared_ptr<Entity> entity) {
        entities.push_back(entity);
    }

    // 安全获取第一个实体（需null检查）
    std::shared_ptr<Entity> GetFirstEntity() const {
        return entities.empty() ? nullptr : entities.front();
    }

    Light light; // << 新增成员变量

    // 实现ClearEntities (与声明严格一致)
    void ClearEntities(){ // [!++ 新增实现]
        entities.clear();
        //std::cout << "已清除所有场景实体\n"; // 调试输出
    }

    
    void RenderScene(const glm::mat4& view, const glm::mat4& projection) {
        // 计算公共矩阵
        //const glm::mat4 viewProj = projection * view;
        
        // 排序优化
        SortEntities(view);
        
        // 统一渲染流程
        for(const auto& entity : entities) {
            if(entity->IsRenderable()) {
                // 增加这三个参数设置步骤 ▶ 核心添加部分
                entity->material->SetVector3("lightColor", light.color);
                entity->material->SetVector3("lightDir", light.direction);
                entity->material->SetFloat("lightIntensity", light.intensity);
                entity->Render(view,projection);
            }
        }
    }

private:
    std::vector<std::shared_ptr<Entity>> entities;
    
    void SortEntities(const glm::mat4& view) {
        // 按渲染队列排序（不透明物体优先）
        std::sort(entities.begin(), entities.end(),
            [](const auto& a, const auto& b) {
                return a->GetRenderQueue() < b->GetRenderQueue();
            });

        // 透明物体按深度排序（从后到前）
        auto transparentStart = std::partition(entities.begin(), entities.end(),
            [](const auto& e) { return !e->IsTransparent(); });
        
        std::sort(transparentStart, entities.end(),
            [&view](const auto& a, const auto& b) {
                return a->GetDepth(view) > b->GetDepth(view);
            });
    }
};
