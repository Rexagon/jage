#version 330

in vec3 v_position;
in float v_sunFade;
in vec3 v_BetaR;
in vec3 v_BetaM;
in float v_sunE;

uniform vec3 u_sunDirection;
uniform float u_luminance;
uniform float u_mieDirectionalG;

const vec3 cameraPos = vec3( 0.0, 0.0, 0.0 );

// constants for atmospheric scattering
const float pi = 3.141592653589793238462643383279502884197169;

// refractive index of air
const float n = 1.0003;

// number of molecules per unit volume for air at
// 288.15K and 1013mb (sea level -45 celsius)
const float N = 2.545E25;

// optical length at zenith for molecules
const float rayleighZenithLength = 8.4E3;
const float mieZenithLength = 1.25E3;
const vec3 up = vec3( 0.0, 1.0, 0.0 );
// 66 arc seconds -> degrees, and the cosine of that
const float sunAngularDiameterCos = 0.999812627955556656903750820965829532378703448007194460804;

// 3.0 / ( 16.0 * pi )
const float THREE_OVER_SIXTEENPI = 0.05968310365946075;
// 1.0 / ( 4.0 * pi )
const float ONE_OVER_FOURPI = 0.07957747154594767;

float rayleighPhase(float cosTheta) {
	return THREE_OVER_SIXTEENPI * (1.0 + pow(cosTheta, 2.0));
}

float hgPhase(float cosTheta, float g) {
	float g2 = pow(g, 2.0);
	float inverse = 1.0 / pow(1.0 - 2.0 * g * cosTheta + g2, 1.5);
	return ONE_OVER_FOURPI * ((1.0 - g2) * inverse);
}

// Filmic ToneMapping http://filmicgames.com/archives/75
const float A = 0.15;
const float B = 0.50;
const float C = 0.10;
const float D = 0.20;
const float E = 0.02;
const float F = 0.30;

const float whiteScale = 1.0748724675633854; // 1.0 / Uncharted2Tonemap(1000.0)

vec3 Uncharted2Tonemap( vec3 x ) {
	return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}


void main() {
	// optical length
	// cutoff angle at 90 to avoid singularity in next formula.
	float zenithAngle = acos(max(0.0, dot(up, normalize(v_position - cameraPos))));
	float inverse = 1.0 / (cos(zenithAngle) + 0.15 * pow( 93.885 - ((zenithAngle * 180.0) / pi), -1.253));
	float sR = rayleighZenithLength * inverse;
	float sM = mieZenithLength * inverse;

	// combined extinction factor
	vec3 Fex = exp( -(v_BetaR * sR + v_BetaM * sM));

	// in scattering
	float cosTheta = dot(normalize(v_position - cameraPos), u_sunDirection);

	float rPhase = rayleighPhase(cosTheta * 0.5 + 0.5);
	vec3 betaRTheta = v_BetaR * rPhase;

	float mPhase = hgPhase(cosTheta, u_mieDirectionalG);
	vec3 betaMTheta = v_BetaM * mPhase;

	vec3 Lin = pow(v_sunE * ((betaRTheta + betaMTheta) / (v_BetaR + v_BetaM)) * (1.0 - Fex), vec3(1.5));
	Lin *= mix(vec3(1.0), pow(v_sunE * ((betaRTheta + betaMTheta) / (v_BetaR + v_BetaM)) * Fex, vec3(1.0 / 2.0)), clamp(pow(1.0 - dot(up, u_sunDirection), 5.0), 0.0, 1.0));

	// nightsky
	vec3 direction = normalize(v_position - cameraPos);
	float theta = acos(direction.y); // elevation --> y-axis, [-pi/2, pi/2]
	float phi = atan(direction.z, direction.x); // azimuth --> x-axis [-pi/2, pi/2]',
	vec2 uv = vec2(phi, theta) / vec2(2.0 * pi, pi) + vec2(0.5, 0.0);
	vec3 L0 = vec3(0.1) * Fex;

    // composition + solar disc
	float sundisk = smoothstep(sunAngularDiameterCos, sunAngularDiameterCos + 0.00002, cosTheta);
	L0 += (v_sunE * 19000.0 * Fex) * sundisk;

	vec3 texColor = (Lin + L0) * 0.04 + vec3(0.0, 0.0003, 0.00075);

	vec3 curr = Uncharted2Tonemap((log2(2.0 / pow(u_luminance, 4.0))) * texColor);
	vec3 color = curr * whiteScale;

	vec3 retColor = pow(color, vec3(1.0 / (1.2 + (1.2 * v_sunFade))));

	gl_FragColor = vec4(retColor, 1.0);
}