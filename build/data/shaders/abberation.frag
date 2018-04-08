#version 330

uniform sampler2D u_colorTexture;
uniform float u_chromaticAberration;

in vec2 v_texCoord;

vec2 barrelDistortion(vec2 coord, float amt) {
	vec2 cc = coord - 0.5;
	float dist = dot(cc, cc);
	return coord + cc * dist * amt;
}

float saturate(float t)
{
	return clamp(t, 0.0, 1.0);
}

float linterp(float t) {
	return saturate(1.0 - abs(2.0 * t - 1.0));
}

float remap(float t, float a, float b) {
	return saturate((t - a) / (b - a));
}

vec4 spectrumOffset(float t) {
	vec4 result;
	float lo = step(t, 0.5);
	float hi = 1.0 - lo;
	float w = linterp(remap(t, 1.0 / 6.0, 5.0 / 6.0));
	result = vec4(lo, 1.0, hi, 1.0) * vec4(1.0 - w, w, 1.0 - w, 1.0);

	return pow(result, vec4(1.0 / 2.2));
}

const int iteraionCount = 6;
const float iterationStep = 1.0 / float(iteraionCount);

void main()
{
	vec4 sumcol = vec4(0.0);
	vec4 sumw = vec4(0.0);	
	for (int i = 0; i < iteraionCount; ++i) {
		float t = float(i) * iterationStep;
		vec4 w = spectrumOffset( t );
		sumw += w;
		sumcol += w * texture2D(u_colorTexture, barrelDistortion(v_texCoord, 0.6 * u_chromaticAberration * t ));
	}
		
	gl_FragColor = sumcol / sumw;
}