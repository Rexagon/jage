#version 330
layout (location = 0) in vec3 position;

uniform mat4 u_transformation;
uniform mat4 u_cameraViewProjection;

void main()
{
    gl_Position = u_cameraViewProjection * u_transformation * vec4(position, 1.0);
}