#version 330

in vec2 v_texCoord;
in vec3 v_normal;

uniform vec3 u_sunDirection;

out vec4 fragColor;

void main()
{
    vec3 diffuse = vec3(1.0);

    vec3 ambient = vec3(0.1);
    vec3 light = clamp(dot(normalize(v_normal), u_sunDirection), 0.0, 1.0) * vec3(1.0, 1.0, 0.95);

    vec3 color = (ambient + light) * diffuse;

    fragColor = vec4(color, 1.0);
    //fragColor = vec4((vec3(1.0f) + normalize(v_normal)) * 0.5f, 0.0);
}