// basic.frag
#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D uAlbedo;
uniform vec3 uColor;

void main()
{
    vec4 texColor = texture(uAlbedo, TexCoord);
    FragColor = vec4(uColor, 1.0) * texColor;
}
