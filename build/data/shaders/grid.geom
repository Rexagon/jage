#version 330

layout (points) in;
layout (line_strip, max_vertices = 256) out;

uniform mat4 u_cameraViewProjection;
uniform mat4 u_transformation;

out vec4 v_color;
out vec4 v_position;

void main() 
{
    float offset = 2.0f;

    int size = 20;

    mat4 MVP = u_cameraViewProjection * u_transformation;
    
    v_color = vec4(1.0f);
    for (int i = -size; i <= size; ++i) {
        if (i == 0) continue;

        v_position = vec4(offset * i, 0.0, -size * offset, 1.0);
        gl_Position = MVP * v_position; 
        EmitVertex();
        v_position = vec4(offset * i, 0.0, size * offset, 1.0);
        gl_Position = MVP * v_position;
        EmitVertex();
        EndPrimitive();

        v_position = vec4(-size * offset, 0.0, offset * i, 1.0);
        gl_Position = MVP * v_position; 
        EmitVertex();
        v_position = vec4(size * offset, 0.0, offset * i, 1.0);
        gl_Position = MVP * v_position;
        EmitVertex();
        EndPrimitive();
    }

    v_position = vec4(0.0f);

    v_color = vec4(1.0, 0.0, 0.0, 1.0);
    gl_Position = MVP * vec4(-size * offset, 0.0, 0.0, 1.0); 
    EmitVertex();
    gl_Position = MVP * vec4(size * offset, 0.0, 0.0, 1.0);
    EmitVertex();
    EndPrimitive();

    v_color = vec4(0.0, 1.0, 0.0, 1.0);
    gl_Position = MVP * vec4(0.0, 0.0, -size * offset, 1.0); 
    EmitVertex();
    gl_Position = MVP * vec4(0.0, 0.0, size * offset, 1.0);
    EmitVertex();
    EndPrimitive();

    v_color = vec4(0.0, 0.0, 1.0, 1.0);
    gl_Position = MVP * vec4(0.0, 0.0, 0.0, 1.0); 
    EmitVertex();
    gl_Position = MVP * vec4(0.0, 1.0, 0.0, 1.0);
    EmitVertex();
    EndPrimitive();
}  