#pragma once
#include <vector>
#include <memory>
#include <algorithm>
#include "Entity.h"

class SceneManager {
public:
    void AddEntity(std::shared_ptr<Entity> entity) {
        entities.push_back(entity);
    }

    void RenderScene(const glm::mat4& view, const glm::mat4& projection) {
        // 计算公共矩阵
        //const glm::mat4 viewProj = projection * view;
        
        // 排序优化
        SortEntities(view);
        
        // 统一渲染流程
        for(const auto& entity : entities) {
            if(entity->IsRenderable()) {
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
