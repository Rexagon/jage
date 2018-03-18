#version 330

uniform sampler2D u_albedoTexture;
uniform sampler2D u_normalsTexture;
uniform sampler2D u_positionsTexture;
uniform sampler2D u_depthTexture;
uniform sampler2D u_lightDepthTexture;
uniform int u_isShadowsEnabled;
uniform vec3 u_direction;
uniform vec3 u_color;

uniform mat4 u_inversedViewProjection;
uniform mat4 u_lightViewProjection;

in vec2 v_texCoord;

void main()
{
    float fragmentDepth = texture(u_depthTexture, v_texCoord).r;
    if (fragmentDepth == 0.0) {
        discard;
    }

    vec3 normal = texture(u_normalsTexture, v_texCoord).xyz;
    normal = normal * 2.0 - 1.0;

    vec4 position = u_inversedViewProjection * vec4(v_texCoord * 2.0 - 1.0, fragmentDepth, 1.0);
    position /= position.w;

    vec3 result = u_color * dot(normal, u_direction);

    vec4 fragmentLightPosition = u_lightViewProjection * position;
    fragmentLightPosition.xyz /= fragmentLightPosition.w;

    float bias = max(0.009 * (1.0 - dot(normal, normalize(u_direction))), 0.005);
    float fragmentLightDepth = texture(u_lightDepthTexture, fragmentLightPosition.xy * 0.5 + 0.5).r;

    float shadow = 0.0;
    if (fragmentLightPosition.z + bias < fragmentLightDepth) {
        shadow = 0.9;
    }
    if (fragmentLightPosition.z < 0.0) {
        shadow = 0.0;
    }

    gl_FragColor = vec4(result * (1.0 - shadow), 1.0);
    //gl_FragColor = texture(u_lightDepthTexture, v_texCoord);
}