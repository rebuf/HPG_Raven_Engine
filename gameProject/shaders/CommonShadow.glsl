#if RENDER_SHADER_TYPE_SHADOW

// Data used by shader in shadow pass
layout(std140) uniform ShadowBlock
{
	// Shadow View Projection Matrix.
	mat4 inShadowViewProj;
};


#else

// Enable Shadow Calculation while Lighting.
#define SHADOW_ENABLED


// Number of cascade should be enabled.
#ifndef MAX_SHADOW_CASCADE
#error MAX_SHADOW_CASCADE should be defined.
#endif



// Shadow information used while computing light.
layout(std140) uniform LightShadowBlock
{
	// View Projection matrix from sun light point of view.
	mat4 sunViewProj[MAX_SHADOW_CASCADE];
	vec4 cascadeRanges[MAX_SHADOW_CASCADE+1];
} inShadowData;


// Sun Shadow.
uniform sampler2DShadow inSunShadow[MAX_SHADOW_CASCADE];


// Calcuate Sun Shadow from sun shadow cascade.
float SunShadowCalculation(vec3 p, vec3 n, vec3 l)
{
	
	float viewDist = length(p - inCommon.viewPos);
	int ci = -1;
	float biasFactor = 0.00024;
	float biasMax = 0.0009;
	vec4 lsPos;
	
	// Compute Cascade...
	for (int i = 0; i < MAX_SHADOW_CASCADE; ++i)
	{
		if (viewDist < inShadowData.cascadeRanges[i+1].x)
		{
			lsPos = inShadowData.sunViewProj[i] * vec4(p, 1.0);
			
			// Normalize
			lsPos = lsPos * 0.5 + 0.5;
	
			if (lsPos.z > 0.9999)
				continue;
			
			ci = i;
			float cb = (i + 1) * 1.0025;
			biasFactor *= cb;
			biasMax *= cb;
			break;
		}
	}

	// Outside of shadow volumes?
	if (ci == -1)
		return 0.0;
	
	
    // get depth of current fragment from light's perspective
    float currentDepth = lsPos.z;
	

	
	
	float bias = max(biasFactor * (1.0 - dot(n, l)), biasMax);  
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(inSunShadow[ci], 0);
	
	// PCF...
	for(float x = -1.5; x <= 1.51; x+=1.0)
	{
		for(float y = -1.5; y <= 1.51; y+=1.0)
		{
			shadow += texture(inSunShadow[ci], lsPos.xyz + vec3(x * texelSize.x, y * texelSize.y, -bias) ).r;
		}    
	}
	

	shadow /= 16.0;


    return 1.0 - shadow;
}  





#endif




