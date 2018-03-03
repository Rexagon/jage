#version 330

in vec2 v_texCoord;
in vec3 v_normal;

uniform vec3 u_sunDirection;

out vec4 fragColor;

void main()
{
    fragColor = (0.1 + clamp(dot(normalize(v_normal), u_sunDirection), 0.0, 1.0) * 0.9) * vec4(1.0);
    //fragColor = vec4((vec3(1.0f) + normalize(v_normal)) * 0.5f, 0.0);
}