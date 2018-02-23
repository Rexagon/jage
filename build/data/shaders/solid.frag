#version 330

uniform vec4 u_color;

in vec2 v_texCoord;

out vec4 fragColor;

void main(void)
{
    fragColor = u_color;
}