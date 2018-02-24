#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normal;

uniform mat4 u_transformation;
uniform mat4 u_cameraViewProjection;

out vec2 v_texCoord;
out vec3 v_normal;

void main()
{
    gl_Position = u_cameraViewProjection * u_transformation * vec4(position, 1.0);

    v_texCoord = texCoord;
    v_normal = (transpose(inverse(u_transformation)) * vec4(normal, 1.0)).xyz;
}