#version 330 core
layout (location = 0) in vec3 aPos;

// 添加三个矩阵的uniform变量
uniform mat4 uModel;      // 模型矩阵（物体空间->世界空间）
uniform mat4 uView;       // 视图矩阵（世界空间->观察空间）
uniform mat4 uProjection; // 投影矩阵（观察空间->裁剪空间）

void main()
{
    // 矩阵应用顺序：Projection * View * Model * Position
    gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0);
}