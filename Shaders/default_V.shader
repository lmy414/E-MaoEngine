#version 330 core    // ← 版本声明必须是文件首行

// 输入顶点属性
layout (location = 0) in vec3 aPos;  
layout (location = 1) in vec3 aNormal;

// 传递到片段着色器的变量
out vec3 Normal;     // ← 输出法线数据

// 统一变量名统一为 model/view/projection （匹配你的应用代码）
uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

void main() {
    gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0);
    Normal = aNormal; // 直接传递法线（可能需要转换为世界空间）
}
