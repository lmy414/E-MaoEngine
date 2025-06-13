#pragma once
#include "Geometry.h" 
#include "Render/Mesh.h"
#include <utility> // 包含std::move

class Cube : public Geometry {
    
    //  Cube() = default; // 添加默认构造函数
    
    Cube(Cube&&) = default;
    
    Cube(const Cube&) = delete;
    
public:
    Cube() = default; // 添加默认构造函数

    
    Mesh CreateMesh() const override {
        // 使用constexpr确保编译期计算
        constexpr float half = 0.5f;
        
        // 预计算顶点数据（符合右手坐标系）
        std::vector<Vertex> vertices = {
            // 前面 (Z+)
            {{-half, -half,  half}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
            {{ half, -half,  half}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
            {{ half,  half,  half}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
            {{-half,  half,  half}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},

            // 后面 (Z-)
            {{ half, -half, -half}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}},
            {{-half, -half, -half}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}},
            {{-half,  half, -half}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},
            {{ half,  half, -half}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}},

            // 左面 (X-)
            {{-half, -half, -half}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
            {{-half, -half,  half}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
            {{-half,  half,  half}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
            {{-half,  half, -half}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},

            // 右面 (X+)
            {{ half, -half,  half}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
            {{ half, -half, -half}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
            {{ half,  half, -half}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
            {{ half,  half,  half}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},

            // 顶面 (Y+)
            {{-half,  half,  half}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
            {{ half,  half,  half}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
            {{ half,  half, -half}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
            {{-half,  half, -half}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},

            // 底面 (Y-)
            {{-half, -half, -half}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},
            {{ half, -half, -half}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},
            {{ half, -half,  half}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}},
            {{-half, -half,  half}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}}
        };

        // 索引数据优化：使用三角形带模式减少索引数量
        std::vector<unsigned int> indices = {
            // 前面
            0, 1, 2, 0, 2, 3,
            // 后面
            4, 5, 6, 4, 6, 7,
            // 左面
            8, 9, 10, 8, 10, 11,
            // 右面
            12, 13, 14, 12, 14, 15,
            // 顶面
            16, 17, 18, 16, 18, 19,
            // 底面
            20, 21, 22, 20, 22, 23
        };

        // 使用完美转发避免拷贝
        return Mesh(std::move(vertices), std::move(indices));
    }
    
    Cube& operator=(const Cube&) = delete;
    
};
