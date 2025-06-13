#version 330 core    // �� �汾��������Ϊ����!

// �������Զ�����ɫ���ķ���
in vec3 Normal;      // �� �����붥����ɫ���� out ������һ��

// ���յ������ɫ
out vec4 FragColor;  // �� ��ȷ�����������

// ���ʺ͹��ղ���
uniform vec3 uColor;          // ���������ɫ
uniform vec3 lightColor;      // ��Դ��ɫ
uniform vec3 lightDir;        // ��Դ����Ӧ��һ����
uniform float lightIntensity; // ����ǿ��

void main() {
    // ���ռ��㣨��Ҫ��ȷ�����ߺ͹�Դ������Ԥ����
    vec3 norm = normalize(Normal);
    vec3 lightDirNormalized = normalize(-lightDir); // ע�ⷽ�����
    
    // ���������
    float diff = max(dot(norm, lightDirNormalized), 0.0);
    
    // �ϳ���ɫ
    vec3 result = lightIntensity * (diff * lightColor) * uColor;
    FragColor = vec4(result, 1.0); 
}
