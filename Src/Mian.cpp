#include "Common.h"
#include "../imgui/dirent.h"
#include"Render/Camera.h"
#include"../TestResources/OBJLoader.h"
#include "../Src/Render/CameraController.h"

// 场景初始化函数
SceneManager InitScene() {
    SceneManager scene;
    try {
        // 加载OBJ模型（代替原来的立方体）
        auto modelEntity = std::make_shared<Entity>();
        
        // 使用OBJLoader加载模型
        Mesh objMesh = OBJLoader::LoadFromFile("E:/MirrorEngine/MirrorEngine2/Assets/m/011obj.obj");
        
        // 通过移动语义创建网格
        modelEntity->mesh = std::make_shared<Mesh>(std::move(objMesh));
        
        // 应用默认材质
        modelEntity->material = std::make_shared<DefaultMaterial>();
        
        // 设置模型变换
        modelEntity->transform->position = {0.0f, -1.0f, 0.0f}; // Y向下移动1单位
        modelEntity->transform->scale = {0.5f, 0.5f, 0.5f};     // 缩放调整模型大小
        modelEntity->transform->MarkDirty();

        scene.AddEntity(modelEntity);
    }
    catch (const std::exception& e) {
        std::cerr << "初始化场景失败: " << e.what() << std::endl;
        // 可在此创建默认立方体作为fallback
        auto fallbackCube = std::make_shared<Entity>();
        fallbackCube->mesh = std::make_shared<Mesh>(Cube().CreateMesh());
        fallbackCube->material = std::make_shared<DefaultMaterial>();
        scene.AddEntity(fallbackCube);
    }
    
    return scene;
}

int main() {
    

    // 初始化OpenGL窗口
    GLFWwindow* window = InitializeOpenGL(800, 600, "E_MaoEngine");
    if (!window) return -1;

    ShaderManager::Initialize(); // 必须先初始化
    
    // 创建帧缓冲（匹配窗口初始尺寸）
    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    Framebuffer mainFramebuffer(fbWidth, fbHeight);

    Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
    CameraController cameraController(window, camera);
    
    // 初始化场景
    SceneManager scene = InitScene();
    auto mainModel = scene.GetFirstEntity(); // 假设你的SceneManager有这个接口
    // 初始化ImGui
    ImGuiManager::Init(window);
    
    // 创建GUI控件并关联帧缓冲
    GUIControls guiControls;
    guiControls.SetFramebufferInfo(mainFramebuffer.GetTexture(), 
                                  mainFramebuffer.width, 
                                  mainFramebuffer.height);
    guiControls.SetTargetEntity(mainModel); 

    // 主渲染循环
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        float currentFrame = glfwGetTime();
        static float lastFrame = 0.0f;
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        cameraController.update(deltaTime);

        // === 第一阶段：渲染到帧缓冲 ===
        mainFramebuffer.Bind();
        glEnable(GL_DEPTH_TEST); // 启用深度测试
        
        // 设置视口和清除颜色
        glViewport(0, 0, mainFramebuffer.width, mainFramebuffer.height);
        glClearColor(guiControls.clearColor.r, 
                    guiControls.clearColor.g, 
                    guiControls.clearColor.b, 
                    1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //glEnable(GL_DEPTH_TEST); // 保持启用状态

        
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = glm::perspective(
            glm::radians(camera.getZoom()), 
            (float)fbWidth/(float)fbHeight, 0.1f, 100.0f);

        // 渲染场景
        scene.RenderScene(view, projection);
        
        mainFramebuffer.Unbind();

        // === 第二阶段：渲染到默认缓冲 ===
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        int displayWidth, displayHeight;
        glfwGetFramebufferSize(window, &displayWidth, &displayHeight);
        glViewport(0, 0, displayWidth, displayHeight);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // 黑色背景
        glClear(GL_COLOR_BUFFER_BIT);

        // === 渲染ImGui界面 ===
        ImGuiManager::BeginFrame();
        guiControls.Render(); // 包含帧缓冲显示和其他控件
        ImGuiManager::EndFrame();

        // 交换缓冲
        glfwSwapBuffers(window);
    }

    // 清理资源
    glfwTerminate();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
    
}
