#version 330 core    // �� �汾�����������ļ�����

// ���붥������
layout (location = 0) in vec3 aPos;  
layout (location = 1) in vec3 aNormal;

// ���ݵ�Ƭ����ɫ���ı���
out vec3 Normal;     // �� �����������

// ͳһ������ͳһΪ model/view/projection ��ƥ�����Ӧ�ô��룩
uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

void main() {
    gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0);
    Normal = aNormal; // ֱ�Ӵ��ݷ��ߣ�������Ҫת��Ϊ����ռ䣩
}
