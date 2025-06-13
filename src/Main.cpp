#include "Common.h"
namespace fs = std::filesystem;  // ��ȫ�����������

//�޸ĳ�����ʼ������

/*SceneManager InitScene() {
    SceneManager scene;
    const std::string base_path = "C:/Users/Mirror/Desktop/3DData/��������/";
    const std::string tileset_path = base_path + "tileset.json";

    std::cout << "[InitScene] ��ʼ��ʼ������..." << std::endl;

    try {
        std::cout << "����Tileset·��: " << tileset_path << std::endl;

        auto modelPaths = TilesetParser::GetB3DMPaths(tileset_path);
        std::cout << "��ȡ��ģ������: " << modelPaths.size() << std::endl;

        for (const auto& modelPath : modelPaths) {
            std::cout << "---------------------------------------------" << std::endl;
            std::cout << "���Լ���ģ���ļ�: " << modelPath << std::endl;

            try {
                auto ext = fs::path(modelPath).extension().string();
                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

                std::shared_ptr<Mesh> mesh;

                if (ext == ".b3dm") {
                    std::cout << "ʶ��Ϊ B3DM ��ʽ������ B3DMLoader ����..." << std::endl;
                    mesh = std::make_shared<Mesh>(B3DMLoader::LoadFromFile(modelPath));
                } else if (ext == ".glb") {
                    std::cout << "ʶ��Ϊ GLB ��ʽ��ֱ�ӽ���..." << std::endl;

                    std::ifstream file(modelPath, std::ios::binary);
                    if (!file) throw std::runtime_error("�޷���GLB�ļ�: " + modelPath);

                    std::vector<uint8_t> glbData((std::istreambuf_iterator<char>(file)),
                                                 std::istreambuf_iterator<char>());

                    std::cout << "��ȡGLB������ϣ���С: " << glbData.size() << " �ֽ�" << std::endl;
                    mesh = std::make_shared<Mesh>(Mirror::GLTF::GLBParser::Parse(glbData).ToMesh());
                } else {
                    std::cerr << "[����] ��֧�ֵ�ģ�͸�ʽ: " << modelPath << std::endl;
                    continue;
                }

                auto entity = std::make_shared<Entity>();
                entity->mesh = mesh;
                entity->transform->position = glm::vec3(0.0f);
                entity->transform->scale = glm::vec3(1.0f);
                entity->material = std::make_shared<DefaultMaterial>();
                entity->name = fs::path(modelPath).stem().string();

                scene.AddEntity(entity);
                std::cout << "�ɹ�����ģ�Ͳ���ӵ�����: " << modelPath << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "[ģ�ͼ���ʧ��] " << modelPath << " - " << e.what() << std::endl;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "[������ʼ��ʧ��] " << e.what() << std::endl;
    }

    //std::cout << "[InitScene] ������ʼ����ɣ�ʵ������: " << scene.GetEntities().size() << std::endl;
    return scene;
}*/

int main() {
    
    // ��ʼ��OpenGL����
    GLFWwindow* window = OpenGLUtils::InitializeOpenGL(800, 600, "E_MaoEngine");
    if (!window) return -1;

    ShaderManager::Initialize(); // �����ȳ�ʼ��

	// ����OpenGL���Իص�
    glfwSetFramebufferSizeCallback(window, OpenGLUtils::FramebufferSizeCallback);

    // ����֡���壨ƥ�䴰�ڳ�ʼ�ߴ磩
    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    Framebuffer mainFramebuffer(fbWidth, fbHeight);

    Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
    CameraController cameraController(window, camera);
    
    
    // ��ʼ������
    SceneManager scene;
    // ��ʼ��ImGui
    ImGuiManager::Init(window);
    
    // ����GUI�ؼ�������֡����
    GUIControls guiControls;
    guiControls.SetSceneManager(&scene); // �ؼ�����
    guiControls.SetFramebufferInfo(mainFramebuffer.GetTexture(), 
                                  mainFramebuffer.Width(), 
                                  mainFramebuffer.Height());

    //guiControls.SetTargetEntity(mainModel);
    guiControls.AddResetCameraCallback([&camera]()
    {
        camera.reset(); // ����Camera��reset����
        });
    // ȷ��������ɫ��ʼ��ͬ��
    if (auto entity = guiControls.GetTargetEntity().lock()) {
        if (auto mat = entity->GetMaterial<DefaultMaterial>()) {
            guiControls.triangleColor = mat->GetColor(); // ˫��ͬ��
        }
    }
    guiControls.SetLight(&scene.light); // << ���������ĵƹ����
    
    
    // ����Ⱦѭ��
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        float currentFrame = glfwGetTime();
        static float lastFrame = 0.0f;
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        cameraController.update(deltaTime);

        // === ��һ�׶Σ���Ⱦ��֡���� ===
        mainFramebuffer.Bind();
        glEnable(GL_DEPTH_TEST); // ������Ȳ���
        
        // �����ӿں������ɫ
        glViewport(0, 0, mainFramebuffer.Width(), mainFramebuffer.Height());
        glClearColor(guiControls.clearColor.r, 
                    guiControls.clearColor.g, 
                    guiControls.clearColor.b, 
                    1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //glEnable(GL_DEPTH_TEST); // ��������״̬
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = glm::perspective(
            glm::radians(camera.getZoom()), 
            (float)fbWidth/(float)fbHeight, 0.1f, 100.0f);

        // ��Ⱦ����
        scene.RenderScene(view, projection);
        
        mainFramebuffer.Unbind();

        // === �ڶ��׶Σ���Ⱦ��Ĭ�ϻ��� ===
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        int displayWidth, displayHeight;
        glfwGetFramebufferSize(window, &displayWidth, &displayHeight);
        glViewport(0, 0, displayWidth, displayHeight);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // ��ɫ����
        glClear(GL_COLOR_BUFFER_BIT);

        // === ��ȾImGui���� ===
        ImGuiManager::BeginFrame();
        guiControls.Render(); // ����֡������ʾ�������ؼ�
        ImGuiManager::EndFrame();

        // ��������
        glfwSwapBuffers(window);
    }

    // ������Դ
    glfwTerminate();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
    
}
