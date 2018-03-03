#version 330

uniform sampler2D u_colorTexture;
uniform float u_chromaticAberration;

in vec2 v_texCoord;

void main()
{
    float dist = pow(length(vec2(0.5, 0.5) - v_texCoord), 2);

    float mov = u_chromaticAberration * dist;
    vec2 uvR = v_texCoord - vec2(mov, 0.0);
    vec2 uvG = v_texCoord + vec2(mov, 0.0);
    vec2 uvB = v_texCoord - vec2(0.0, mov);
    vec4 colR = texture2D(u_colorTexture, uvR);
    vec4 colG = texture2D(u_colorTexture, uvG);
    vec4 colB = texture2D(u_colorTexture, uvB);

    gl_FragColor = texture2D(u_colorTexture, v_texCoord) * 0.2 + 0.8 * vec4(colR.r, colG.g, colB.b, 1.0);
}