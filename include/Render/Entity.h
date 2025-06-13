#pragma once
#include "Transform.h"
#include "Mesh.h"
#include "Render/Material/Material.h"
#include "ProgressiveLOD.h"

class Entity {
public:
    std::string name; // 新增名字
    std::shared_ptr<Transform> transform = std::make_shared<Transform>();
    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Material> material;
    std::shared_ptr<ProgressiveLOD> lodController;  // 新增LOD控制器指针

    void Render(const glm::mat4& view, const glm::mat4& projection) const {
        if (!IsRenderable()) return;

        // 计算模型矩阵
        const glm::mat4 model = transform->GetGlobalMatrix();
        
        // 设置材质参数
        material->SetMatrix4("uModel", model);
        material->SetMatrix4("uView", view);
        material->SetMatrix4("uProjection", projection);
        
        // 可选：传递世界位置
        //material->SetVector3("u_WorldPos", transform->position);

        // 应用材质并绘制
        material->Apply();
        mesh->Draw();
    }

    bool IsRenderable() const {
        return mesh && mesh->IsReady() && 
               material && material->IsValid();
    }


    int GetRenderQueue() const { 
        return material ? material->GetRenderQueue() : 2000; 
    }
    bool IsTransparent() const { 
        return material && material->IsTransparent(); 
    }

    float GetDepth(const glm::mat4& view) const {
        glm::vec4 pos = view * transform->GetGlobalMatrix()[3];
        return pos.z / pos.w;
    }
    
    template<typename T>
    std::shared_ptr<T> GetMaterial() const {
        return std::dynamic_pointer_cast<T>(material);
    }


private:
    // 保持与你的材质类参数命名一致
    static constexpr const char* MODEL_MATRIX = "uModel";
    static constexpr const char* VIEW_PROJ_MATRIX = "uProjection";
};
