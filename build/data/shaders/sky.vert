#version 330

layout (location = 0) in vec3 position;

uniform mat4 u_cameraProjection;
uniform mat4 u_cameraViewRotation;

uniform vec3 u_sunDirection;
uniform float u_rayleigh;
uniform float u_turbidity;
uniform float u_mieCoefficient;

out vec3 v_position;
out float v_sunFade;
out vec3 v_BetaR;
out vec3 v_BetaM;
out float v_sunE;

const vec3 up = vec3(0.0, 1.0, 0.0);

// constants for atmospheric scattering
const float e = 2.71828182845904523536028747135266249775724709369995957;
const float pi = 3.141592653589793238462643383279502884197169;

// wavelength of used primaries, according to preetham
const vec3 lambda = vec3(680E-9, 550E-9, 450E-9);
// this pre-calcuation replaces older TotalRayleigh(vec3 lambda) function:
// (8.0 * pow(pi, 3.0) * pow(pow(n, 2.0) - 1.0, 2.0) * (6.0 + 3.0 * pn)) / (3.0 * N * pow(lambda, vec3(4.0)) * (6.0 - 7.0 * pn))
const vec3 totalRayleigh = vec3(5.804542996261093E-6, 1.3562911419845635E-5, 3.0265902468824876E-5);

// mie stuff
// K coefficient for the primaries
const float v = 4.0;
const vec3 K = vec3( 0.686, 0.678, 0.666 );
// MieConst = pi * pow( ( 2.0 * pi ) / lambda, vec3( v - 2.0 ) ) * K
const vec3 MieConst = vec3(1.8399918514433978E14, 2.7798023919660528E14, 4.0790479543861094E14);

// earth shadow hack
// cutoffAngle = pi / 1.95;
const float cutoffAngle = 1.6110731556870734;
const float steepness = 1.5;
const float EE = 1000.0;

float sunIntensity(float zenithAngleCos) {
    zenithAngleCos = clamp( zenithAngleCos, -1.0, 1.0);
    return EE * max(0.0, 1.0 - pow(e, -((cutoffAngle - acos(zenithAngleCos)) / steepness )));
}

vec3 totalMie(float T) {
    float c = (0.2 * T) * 10E-18;
    return 0.434 * c * MieConst;
}

void main() {

    v_position = position;

    gl_Position = u_cameraProjection * u_cameraViewRotation * vec4(position, 1.0f);
    gl_Position.z = 0.0f;

    v_sunE = sunIntensity(dot(u_sunDirection, up));

    v_sunFade = 1.0 - clamp(1.0 - exp((u_sunDirection.y / 450000.0)), 0.0, 1.0);

    float rayleighCoefficient = u_rayleigh - (1.0 * (1.0 - v_sunFade));

    // extinction (absorbtion + out scattering)
    // u_rayleigh coefficients
    v_BetaR = totalRayleigh * rayleighCoefficient;

    // mie coefficients
    v_BetaM = totalMie(u_turbidity) * u_mieCoefficient;
}