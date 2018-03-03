#version 330

layout (points) in;
layout (line_strip, max_vertices = 48) out;

in ivec2 v_chunkId[];

uniform mat4 u_cameraViewProjection;
uniform mat4 u_transformation;

out vec4 v_color;
out vec4 v_position;

void main() 
{
    float offset = 2.0f;

    int size = 5;
    
    for (int i = -size; i <= size; ++i) {
        if (i == 0 && v_chunkId[0].x == 0) {
            v_color = vec4(1.0, 0.0, 0.0, 1.0);
        }
        else {
            v_color = vec4(1.0f);
        }

        v_position = u_transformation * (gl_in[0].gl_Position + vec4(offset * i, 0.0, -size * offset, 0.0));
        gl_Position = u_cameraViewProjection * v_position; 
        EmitVertex();
        v_position = u_transformation * (gl_in[0].gl_Position + vec4(offset * i, 0.0, size * offset, 0.0));
        gl_Position = u_cameraViewProjection * v_position;
        EmitVertex();
        EndPrimitive();

        if (i == 0 && v_chunkId[0].y == 0) {
            v_color = vec4(0.0, 1.0, 0.0, 1.0);
        }
        else {
            v_color = vec4(1.0f);
        }

        v_position = u_transformation * (gl_in[0].gl_Position + vec4(-size * offset, 0.0, offset * i, 0.0));
        gl_Position = u_cameraViewProjection * v_position; 
        EmitVertex();
        v_position = u_transformation * (gl_in[0].gl_Position + vec4(size * offset, 0.0, offset * i, 0.0));
        gl_Position = u_cameraViewProjection * v_position;
        EmitVertex();
        EndPrimitive();
    }

    if (v_chunkId[0].x == 0 && v_chunkId[0].y == 0) {
        v_color = vec4(0.0, 0.0, 1.0, 1.0);

        v_position = u_transformation * (gl_in[0].gl_Position + vec4(0.0, 0.0, 0.0, 0.0));
        gl_Position = u_cameraViewProjection * v_position; 
        EmitVertex();
        v_position = u_transformation * (gl_in[0].gl_Position + vec4(0.0, 1.0, 0.0, 0.0));
        gl_Position = u_cameraViewProjection * v_position;
        EmitVertex();
        EndPrimitive();
    }
}  