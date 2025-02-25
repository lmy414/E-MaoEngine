#include "Common.h"

int main() {
    // 初始化OpenGL窗口
    GLFWwindow* window = InitializeOpenGL(800, 600, "Simple OpenGL Window");
    if (!window) return -1;
    
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
        
        // 设置视口和清除颜色
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        triangle.Draw();

        // 交换缓冲
        glfwSwapBuffers(window);

        
    }

    // 清理资源
    glfwTerminate();
    return 0;
}


