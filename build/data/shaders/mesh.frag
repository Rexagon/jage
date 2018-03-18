#version 330

in vec2 v_texCoord;
in vec3 v_normal;

uniform vec3 u_sunDirection;

out vec4 fragColor;

layout(location = 0) out vec4 f_albedo;
layout(location = 1) out vec4 f_normals;
layout(location = 2) out vec4 f_position;
layout(location = 3) out vec4 f_specular;

void main()
{
    vec3 diffuse = vec3(1.0);

    vec3 ambient = vec3(0.1);
    vec3 light = clamp(dot(normalize(v_normal), u_sunDirection), 0.0, 1.0) * vec3(1.0, 1.0, 1.0);

    vec3 color = (ambient + light) * diffuse;

    f_albedo = vec4(color, 1.0);
    f_normals = vec4((vec3(1.0f) + normalize(v_normal)) * 0.5f, 0.0);
    f_position = vec4(1.0f, 0.0, 0.0f, 1.0f);
    f_specular = vec4(0.0f, 1.0, 0.0f, 1.0f);
}