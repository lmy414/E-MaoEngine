cmake_minimum_required(VERSION 3.15)
project(EMaoEengine)

set(CMAKE_CXX_STANDARD 20)

# 设置GLFW选项
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

# 添加GLFW子目录
add_subdirectory(${CMAKE_SOURCE_DIR}/ThirdParty/glfw-master)

# 收集所有源文件
file(GLOB_RECURSE SOURCE_FILES
    "src/*.cpp"
    "src/*/*.cpp"
    "src/*/*/*.cpp"
    "include/glad.c"  # 使用原始位置的glad.c
)

# 收集所有头文件
file(GLOB_RECURSE HEADER_FILES
    "include/*.h"
    "include/*.hpp"
    "include/*/*.h"
    "include/*/*.hpp"
    "include/*/*/*.h"
    "include/*/*/*.hpp"
    "ThirdParty/glfw-master/include/GLFW/*.h"  # 包含GLFW头文件
    "ThirdParty/glm/glm/*.hpp"  # 包含GLM头文件
    "ThirdParty/assimp/*.h"     # 包含Assimp头文件
)

# 添加可执行文件并包含所有源文件和头文件
add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})

# 包含目录
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/include/3Dtiles
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Core
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Gui
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Render
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Render/Light
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Render/Material
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Resources
    ${CMAKE_CURRENT_SOURCE_DIR}/include/TestResources
    ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty
    ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/glfw-master/include
)

# 链接库
target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw
    opengl32
)

# 添加shader目录
file(COPY ${CMAKE_SOURCE_DIR}/Shaders DESTINATION ${CMAKE_BINARY_DIR})

# 添加Assets目录
file(COPY ${CMAKE_SOURCE_DIR}/Assets DESTINATION ${CMAKE_BINARY_DIR})

# 修复Windows上的编译警告
if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

# 在Visual Studio中组织文件结构
source_group("Header Files" FILES ${HEADER_FILES})
source_group("Source Files" FILES ${SOURCE_FILES})
