#version 330

uniform sampler2D u_colorTexture; 

uniform vec2 u_resolution;

uniform int u_showEdges;
uniform int u_fxaaOn;

uniform float u_lumaThreshold; // 0.5
uniform float u_mulReduce; // 1.0 / 8.0
uniform float u_minReduce; // 1.0 / 128
uniform float u_maxSpan; // 8.0

in vec2 v_texCoord;

out vec4 fragColor;

void main(void)
{
    vec3 rgbM = texture(u_colorTexture, v_texCoord).rgb;

	// Possibility to toggle FXAA on and off.
	if (u_fxaaOn == 0)
	{
		fragColor = vec4(rgbM, 1.0);
		
		return;
	}

	vec3 rgbNW = textureOffset(u_colorTexture, v_texCoord, ivec2(-1, 1)).rgb;
    vec3 rgbNE = textureOffset(u_colorTexture, v_texCoord, ivec2(1, 1)).rgb;
    vec3 rgbSW = textureOffset(u_colorTexture, v_texCoord, ivec2(-1, -1)).rgb;
    vec3 rgbSE = textureOffset(u_colorTexture, v_texCoord, ivec2(1, -1)).rgb;

	const vec3 toLuma = vec3(0.299, 0.587, 0.114);
	
	float lumaNW = dot(rgbNW, toLuma);
	float lumaNE = dot(rgbNE, toLuma);
	float lumaSW = dot(rgbSW, toLuma);
	float lumaSE = dot(rgbSE, toLuma);
	float lumaM = dot(rgbM, toLuma);

	// gather minimum and maximum luma.
	float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
	float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));
	
	// If contrast is lower than a maximum threshold ...
	if (lumaMax - lumaMin < lumaMax * u_lumaThreshold)
	{
		// ... do no AA and return.
		fragColor = vec4(rgbM, 1.0);
		
		return;
	}  
	
	// Sampling is done along the gradient.
	vec2 samplingDirection;	
	samplingDirection.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
    samplingDirection.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));
    
    // Sampling step distance depends on the luma: The brighter the sampled texels, the smaller the final sampling step direction.
    // This results, that brighter areas are less blurred/more sharper than dark areas.  
    float samplingDirectionReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * 0.25 * u_mulReduce, u_minReduce);

	// Factor for norming the sampling direction plus adding the brightness influence. 
	float minSamplingDirectionFactor = 1.0 / (min(abs(samplingDirection.x), abs(samplingDirection.y)) + samplingDirectionReduce);
    
    // Calculate final sampling direction vector by reducing, clamping to a range and finally adapting to the texture size. 
    samplingDirection = clamp(samplingDirection * minSamplingDirectionFactor, vec2(-u_maxSpan, -u_maxSpan), vec2(u_maxSpan, u_maxSpan)) * (1.0 / u_resolution.xy);
	
	// Inner samples on the tab.
	vec3 rgbSampleNeg = texture(u_colorTexture, v_texCoord + samplingDirection * (1.0/3.0 - 0.5)).rgb;
	vec3 rgbSamplePos = texture(u_colorTexture, v_texCoord + samplingDirection * (2.0/3.0 - 0.5)).rgb;

	vec3 rgbTwoTab = (rgbSamplePos + rgbSampleNeg) * 0.5;  

	// Outer samples on the tab.
	vec3 rgbSampleNegOuter = texture(u_colorTexture, v_texCoord + samplingDirection * (0.0/3.0 - 0.5)).rgb;
	vec3 rgbSamplePosOuter = texture(u_colorTexture, v_texCoord + samplingDirection * (3.0/3.0 - 0.5)).rgb;
	
	vec3 rgbFourTab = (rgbSamplePosOuter + rgbSampleNegOuter) * 0.25 + rgbTwoTab * 0.5;   
	
	// Calculate luma for checking against the minimum and maximum value.
	float lumaFourTab = dot(rgbFourTab, toLuma);
	
	// Are outer samples of the tab beyond the edge ... 
	if (lumaFourTab < lumaMin || lumaFourTab > lumaMax)
	{
		fragColor = vec4(rgbTwoTab, 1.0); 
	}
	else
	{
		fragColor = vec4(rgbFourTab, 1.0);
	}

	// Show edges for debug purposes.	
	if (u_showEdges != 0)
	{
		fragColor.r = 1.0;
	}
}