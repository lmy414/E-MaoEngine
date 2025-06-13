# EMaoEengine

## 项目概述
这是一个基于OpenGL的小型3D渲染器，支持3D Tiles加载和基本渲染功能。

## 编译指南

### 前提条件
1. **CMake 3.15+** - [下载地址](https://cmake.org/download/)
2. **C++编译器**：
    - Windows: Visual Studio 2019+ (推荐使用MSVC)
    - Linux: GCC 9+

### 编译步骤

#### Windows (使用Visual Studio)
```bash
# 克隆仓库
git clone https://github.com/yourusername/EMaoEengine.git
cd EMaoEengine

# 创建构建目录
mkdir build
cd build

# 生成解决方案
cmake .. -G "Visual Studio 17 2022" -A x64

# 编译项目
cmake --build . --config Release
