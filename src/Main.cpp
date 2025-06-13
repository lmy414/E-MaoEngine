#include "Common.h"
namespace fs = std::filesystem;  // 在全局作用域添加

//修改场景初始化函数

/*SceneManager InitScene() {
    SceneManager scene;
    const std::string base_path = "C:/Users/Mirror/Desktop/3DData/测试用例/";
    const std::string tileset_path = base_path + "tileset.json";

    std::cout << "[InitScene] 开始初始化场景..." << std::endl;

    try {
        std::cout << "解析Tileset路径: " << tileset_path << std::endl;

        auto modelPaths = TilesetParser::GetB3DMPaths(tileset_path);
        std::cout << "获取到模型数量: " << modelPaths.size() << std::endl;

        for (const auto& modelPath : modelPaths) {
            std::cout << "---------------------------------------------" << std::endl;
            std::cout << "尝试加载模型文件: " << modelPath << std::endl;

            try {
                auto ext = fs::path(modelPath).extension().string();
                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

                std::shared_ptr<Mesh> mesh;

                if (ext == ".b3dm") {
                    std::cout << "识别为 B3DM 格式，调用 B3DMLoader 加载..." << std::endl;
                    mesh = std::make_shared<Mesh>(B3DMLoader::LoadFromFile(modelPath));
                } else if (ext == ".glb") {
                    std::cout << "识别为 GLB 格式，直接解析..." << std::endl;

                    std::ifstream file(modelPath, std::ios::binary);
                    if (!file) throw std::runtime_error("无法打开GLB文件: " + modelPath);

                    std::vector<uint8_t> glbData((std::istreambuf_iterator<char>(file)),
                                                 std::istreambuf_iterator<char>());

                    std::cout << "读取GLB数据完毕，大小: " << glbData.size() << " 字节" << std::endl;
                    mesh = std::make_shared<Mesh>(Mirror::GLTF::GLBParser::Parse(glbData).ToMesh());
                } else {
                    std::cerr << "[跳过] 不支持的模型格式: " << modelPath << std::endl;
                    continue;
                }

                auto entity = std::make_shared<Entity>();
                entity->mesh = mesh;
                entity->transform->position = glm::vec3(0.0f);
                entity->transform->scale = glm::vec3(1.0f);
                entity->material = std::make_shared<DefaultMaterial>();
                entity->name = fs::path(modelPath).stem().string();

                scene.AddEntity(entity);
                std::cout << "成功加载模型并添加到场景: " << modelPath << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "[模型加载失败] " << modelPath << " - " << e.what() << std::endl;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "[场景初始化失败] " << e.what() << std::endl;
    }

    //std::cout << "[InitScene] 场景初始化完成，实体数量: " << scene.GetEntities().size() << std::endl;
    return scene;
}*/

int main() {
    
    // 初始化OpenGL窗口
    GLFWwindow* window = OpenGLUtils::InitializeOpenGL(800, 600, "E_MaoEngine");
    if (!window) return -1;

    ShaderManager::Initialize(); // 必须先初始化

	// 设置OpenGL调试回调
    glfwSetFramebufferSizeCallback(window, OpenGLUtils::FramebufferSizeCallback);

    // 创建帧缓冲（匹配窗口初始尺寸）
    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    Framebuffer mainFramebuffer(fbWidth, fbHeight);

    Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
    CameraController cameraController(window, camera);
    
    
    // 初始化场景
    SceneManager scene;
    // 初始化ImGui
    ImGuiManager::Init(window);
    
    // 创建GUI控件并关联帧缓冲
    GUIControls guiControls;
    guiControls.SetSceneManager(&scene); // 关键关联
    guiControls.SetFramebufferInfo(mainFramebuffer.GetTexture(), 
                                  mainFramebuffer.Width(), 
                                  mainFramebuffer.Height());

    //guiControls.SetTargetEntity(mainModel);
    guiControls.AddResetCameraCallback([&camera]()
    {
        camera.reset(); // 调用Camera的reset方法
        });
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
        glViewport(0, 0, mainFramebuffer.Width(), mainFramebuffer.Height());
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
