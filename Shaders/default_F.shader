#version 330 core    // ← 版本声明必须为首行!

// 输入来自顶点着色器的法线
in vec3 Normal;      // ← 必须与顶点着色器的 out 变量名一致

// 最终的输出颜色
out vec4 FragColor;  // ← 明确声明输出变量

// 材质和光照参数
uniform vec3 uColor;          // 物体基础颜色
uniform vec3 lightColor;      // 光源颜色
uniform vec3 lightDir;        // 光源方向（应归一化）
uniform float lightIntensity; // 光照强度

void main() {
    // 光照计算（重要：确保法线和光源方向已预处理）
    vec3 norm = normalize(Normal);
    vec3 lightDirNormalized = normalize(-lightDir); // 注意方向符号
    
    // 漫反射分量
    float diff = max(dot(norm, lightDirNormalized), 0.0);
    
    // 合成颜色
    vec3 result = lightIntensity * (diff * lightColor) * uColor;
    FragColor = vec4(result, 1.0); 
}
