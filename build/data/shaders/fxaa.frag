#version 330

uniform sampler2D u_screenTexture;
uniform vec2 u_screenSizeInverted;

in vec2 v_texCoord;

void main()
{
    const float R_fxaaReduceMin = 1.0 / 128;
    const float R_fxaaReduceMul = 1.0 / 8.0;
    const float R_fxaaSpanMax = 8.0;
	
	vec3 luma = vec3(0.299, 0.587, 0.114);	
	float lumaTL = dot(luma, texture2D(u_screenTexture, v_texCoord.xy + (vec2(-1.0, -1.0) * u_screenSizeInverted)).xyz);
	float lumaTR = dot(luma, texture2D(u_screenTexture, v_texCoord.xy + (vec2(1.0, -1.0) * u_screenSizeInverted)).xyz);
	float lumaBL = dot(luma, texture2D(u_screenTexture, v_texCoord.xy + (vec2(-1.0, 1.0) * u_screenSizeInverted)).xyz);
	float lumaBR = dot(luma, texture2D(u_screenTexture, v_texCoord.xy + (vec2(1.0, 1.0) * u_screenSizeInverted)).xyz);
	float lumaM  = dot(luma, texture2D(u_screenTexture, v_texCoord.xy).xyz);

	vec2 dir;
	dir.x = -((lumaTL + lumaTR) - (lumaBL + lumaBR));
	dir.y = ((lumaTL + lumaBL) - (lumaTR + lumaBR));
	
	float dirReduce = max((lumaTL + lumaTR + lumaBL + lumaBR) * (R_fxaaReduceMul * 0.25), R_fxaaReduceMin);
	float inverseDirAdjustment = 1.0/(min(abs(dir.x), abs(dir.y)) + dirReduce);
	
	dir = min(vec2(R_fxaaSpanMax, R_fxaaSpanMax), 
		max(vec2(-R_fxaaSpanMax, -R_fxaaSpanMax), dir * inverseDirAdjustment));
	
	dir.x = dir.x * step(1.0, abs(dir.x));
	dir.y = dir.y * step(1.0, abs(dir.y));
	
	//float dirStep = max(step(1.0, abs(dir.x)), step(1.0, abs(dir.y)));
	//dir.x = dir.x * dirStep;
	//dir.y = dir.y * dirStep;
	
	dir = dir * u_screenSizeInverted;

	vec3 result1 = (1.0/2.0) * (
		texture2D(u_screenTexture, v_texCoord.xy + (dir * vec2(1.0/3.0 - 0.5))).xyz +
		texture2D(u_screenTexture, v_texCoord.xy + (dir * vec2(2.0/3.0 - 0.5))).xyz);

	vec3 result2 = result1 * (1.0/2.0) + (1.0/4.0) * (
		texture2D(u_screenTexture, v_texCoord.xy + (dir * vec2(0.0/3.0 - 0.5))).xyz +
		texture2D(u_screenTexture, v_texCoord.xy + (dir * vec2(3.0/3.0 - 0.5))).xyz);

	float lumaMin = min(lumaM, min(min(lumaTL, lumaTR), min(lumaBL, lumaBR)));
	float lumaMax = max(lumaM, max(max(lumaTL, lumaTR), max(lumaBL, lumaBR)));
	float lumaResult2 = dot(luma, result2);
	
	if(lumaResult2 < lumaMin || lumaResult2 > lumaMax)
		gl_FragColor = vec4(result1, 1.0);
	else
		gl_FragColor = vec4(result2, 1.0);
}