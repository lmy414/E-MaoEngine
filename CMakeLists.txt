cmake_minimum_required(VERSION 3.15)
project(EMaoEengine)

set(CMAKE_CXX_STANDARD 20)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)

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

# 添加可执行文件
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# 包含目录
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty
    ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/glfw-master/include
    ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/glm
)

# 链接库
target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw
    opengl32
)

# 复制资源文件到构建目录
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Shaders $<TARGET_FILE_DIR:${PROJECT_NAME}>/Shaders
    COMMENT "复制Shader文件..."
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Assets $<TARGET_FILE_DIR:${PROJECT_NAME}>/Assets
    COMMENT "复制Assets文件..."
)

# Windows编译设置
if(MSVC)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        _CRT_SECURE_NO_WARNINGS
        NOMINMAX
    )
endif()

# 在Visual Studio中组织文件结构
file(GLOB_RECURSE HEADER_FILES "include/*.h*")
file(GLOB_RECURSE SOURCE_FILES_LIST "src/*.cpp")
source_group("Header Files" FILES ${HEADER_FILES})
source_group("Source Files" FILES ${SOURCE_FILES_LIST})

# 设置启动工作目录
set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
)
