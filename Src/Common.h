#ifndef COMMON_H
#define COMMON_H

#include "../Src/Render/Framebuffer.h"//帧缓冲
#include "../imgui/ImGuiFileDialog.h"
#include "Gui/ImGuiManager.h"
#include "Gui/GUIControls.h"
#include "../Src/Render/Opengl_Utils.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "Render/Mesh.h"
#include "../Src/Render/Material/Material.h"  // 包含材质头文件
#include "../Src/Render/Camera.h"
#include "../Src/Render/SceneManager.h"
#include "../Src/Render/ShaderManager.h"
#include "../Src/Render/Framebuffer.h"
#include "../Src/Render/Texture.h"
#include "../Src/Render/Shader.h"
#include "../Src/Render/Mesh.h"
#include "../Src/Render/Material/DerivedMaterials.h"
#include <windows.h> 
#include "../imgui/dirent.h"
#include"Render/Camera.h"
#include"../TestResources/OBJLoader.h"
#include "../3Dtiles/B3DMLoader.h"
#include "../Src/Render/CameraController.h"
#include "../3Dtiles/TilesetParser.h"
#include <filesystem>  // 需要C++17或更高版本

//测试用资源
#include "../TestResources/Cube.h"
#include "../TestResources/Geometry.h"
//#include "../TestResources/Triangle.h"

#endif // COMMON_H




















