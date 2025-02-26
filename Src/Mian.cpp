#include "Common.h"


int main() {
    // 初始化OpenGL窗口
    GLFWwindow* window = InitializeOpenGL(800, 600, "E_MaoEngine");
    if (!window) return -1;
    
    ImGuiManager::Init(window);
    
    // 创建GUI控件实例
    GUIControls guiControls;
    
    // 创建默认材质并设置颜色
    DefaultMaterial defaultMat;
    defaultMat.Color = guiControls.triangleColor;

    // 顶点数据
    std::vector<Vertex> vertices = {
        {glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f)},
        {glm::vec3(0.5f, -0.5f, 0.0f),  glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f)},
        {glm::vec3(0.0f,  0.5f, 0.0f),  glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.5f, 1.0f)}
    };

    std::vector<unsigned int> indices = {0, 1, 2};
    Mesh triangle(vertices, indices);

    // 主渲染循环
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // 开始ImGui帧
        ImGuiManager::BeginFrame();
        
        // 清除缓冲
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glClearColor(guiControls.clearColor.r, 
                   guiControls.clearColor.g, 
                   guiControls.clearColor.b, 
                   1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // 应用材质并绘制
        defaultMat.Color = guiControls.triangleColor;
        defaultMat.Apply();
        triangle.Draw();

        // 渲染GUI控件
        guiControls.Render();
        
        // 结束ImGui帧
        ImGuiManager::EndFrame();

        // 交换缓冲
        glfwSwapBuffers(window);
    }

    // 清理资源
    glfwTerminate();
    return 0;
} 
