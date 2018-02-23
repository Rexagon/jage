#version 330
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;

out vec2 v_texCoord;

void main()
{
    gl_Position = vec4(position.x, position.y, position.z, 1.0);
    v_texCoord = texCoord;
}