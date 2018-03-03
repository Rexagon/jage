#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in ivec2 chunkId;

out ivec2 v_chunkId;

void main()
{
    gl_Position = vec4(position.x, position.y, position.z, 1.0);
    v_chunkId = chunkId;
}