#version 330

in vec4 v_color;
in vec4 v_position;

uniform vec3 u_cameraPosition;

out vec4 fragColor;

void main(void)
{
    float dist = length(u_cameraPosition - v_position.xyz);
    fragColor = vec4(v_color.xyz, clamp(10 / dist + 0.001, 0.0, 1.0));
}