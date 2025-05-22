#include "Common.h"
#include <windows.h> 
#include "../imgui/dirent.h"
#include"Render/Camera.h"
#include"../TestResources/OBJLoader.h"
#include "../3Dtiles/B3DMLoader.h"
#include "../Src/Render/CameraController.h"
#include "../3Dtiles/TilesetParser.h"
#include <filesystem>  // 需要C++17或更高版本

namespace fs = std::filesystem;  // 在全局作用域添加

// Main.cpp 修改场景初始化函数
SceneManager InitScene() {
    SceneManager scene;
    const std::string base_path = "C:/Users/Mirror/Desktop/3DData/";

    // 修改为加载3D Tiles
    try {
        // 步骤1：解析Tileset
        auto b3dmPaths = TilesetParser::GetB3DMPaths(base_path + "tileset.json");
        
        // 步骤2：加载所有B3DM模型
        for (const auto& b3dmPath : b3dmPaths) {
            try {
                auto entity = std::make_shared<Entity>();
                entity->mesh = std::make_shared<Mesh>(B3DMLoader::LoadFromFile(b3dmPath));
                
                // 初始化变换参数
                entity->transform->position = glm::vec3(0.0f);
                entity->transform->scale = glm::vec3(1.0f);
                
                // 关联默认材质
                entity->material = std::make_shared<DefaultMaterial>();
                entity->name = fs::path(b3dmPath).stem().string();
                
                scene.AddEntity(entity);
                std::cout << "成功加载B3DM: " << b3dmPath << std::endl;
            }
            catch (const std::exception& e) {
                std::cerr << "[B3DM加载失败] " << b3dmPath 
                          << " - " << e.what() << std::endl;
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "场景初始化异常: " << e.what() << std::endl;
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
    auto mainModel = scene.GetFirstEntity();
    // 初始化ImGui
    ImGuiManager::Init(window);
    
    // 创建GUI控件并关联帧缓冲
    GUIControls guiControls;
    guiControls.SetFramebufferInfo(mainFramebuffer.GetTexture(), 
                                  mainFramebuffer.width, 
                                  mainFramebuffer.height);
    guiControls.SetTargetEntity(mainModel);
    // 确保材质颜色初始化同步
    if (auto entity = guiControls.GetTargetEntity().lock()) {
        if (auto mat = entity->GetMaterial<DefaultMaterial>()) {
            guiControls.triangleColor = mat->GetColor(); // 双向同步
        }
    }
    
    guiControls.SetLight(&scene.light); // << 关联场景的灯光对象
    
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
