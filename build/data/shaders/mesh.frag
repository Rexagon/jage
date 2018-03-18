#version 330

uniform sampler2D u_albedoTexture;
uniform sampler2D u_normalsTexture;

in vec2 v_texCoord;
in vec3 v_normal;

layout(location = 0) out vec4 f_albedo;
layout(location = 1) out vec4 f_normals;
layout(location = 2) out vec4 f_position;

void main()
{
    f_albedo = texture(u_albedoTexture, v_texCoord);
    f_normals = vec4((vec3(1.0f) + normalize(v_normal)) * 0.5f, 0.0);
    f_position = vec4(0.0f, 0.0, 0.0f, 0.0f);
}