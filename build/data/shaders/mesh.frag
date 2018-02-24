#version 330

in vec2 v_texCoord;
in vec3 v_normal;

out vec4 fragColor;

void main()
{
    //vec4 color = texture(diffuseTexture, v_texCoord);    

    fragColor = vec4((vec3(1.0f) + normalize(v_normal)) * 0.5f, 0.0);
}