# EMaoEengine

## ��Ŀ����
����һ������OpenGL��С��3D��Ⱦ����֧��3D Tiles���غͻ�����Ⱦ���ܡ�

## ����ָ��

### ǰ������
1. **CMake 3.15+** - [���ص�ַ](https://cmake.org/download/)
2. **C++������**��
    - Windows: Visual Studio 2019+ (�Ƽ�ʹ��MSVC)
    - Linux: GCC 9+

### ���벽��

#### Windows (ʹ��Visual Studio)
```bash
# ��¡�ֿ�
git clone https://github.com/yourusername/EMaoEengine.git
cd EMaoEengine

# ��������Ŀ¼
mkdir build
cd build

# ���ɽ������
cmake .. -G "Visual Studio 17 2022" -A x64

# ������Ŀ
cmake --build . --config Release
