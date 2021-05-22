#version 450 core


// Reference:
//   - https://learnopengl.com/Advanced-Lighting/SSAO
//   - http://john-chapman-graphics.blogspot.com/2013/01/ssao-tutorial.html
//


// Vertex Shader Output.
in VertexOutput
{
	//
	vec2 rtCoord;
} inFrag;



// Number of SSAO smaples to do. 
#ifndef NUM_SAMPLES
#error Number of samples must be defiend.
#endif



// Input G-Buffer...
uniform sampler2D inNormal;
uniform sampler2D inDepth;
uniform vec2 inRTSize;

// Noise for sampling.
uniform sampler2D inNoise;
uniform vec3 inSamples[NUM_SAMPLES];




// Output.
layout(location = 0) out vec4 outFinalColor;




// --- --- ---- --- --- ---- --- --- ---- --- --- ---- --- --- ---- 




void main()
{
	vec2 screenCoord = ComputeScreenCoord();
	vec2 uvScale = (inCommon.viewport.zw / inRTSize);
	
	// G-Buffer.
	float gDepth = texture(inDepth, inFrag.rtCoord).r;
	
		
	// Reconstruct World Position from depth.
	vec3 P = ComputeWorldPos(screenCoord, gDepth);
	float gDepthLinear = DepthToLinaer(gDepth);
	
	
	// Normal
	vec3 N = texture(inNormal, inFrag.rtCoord).xyz;
	
	// Noise
	vec2 noiseScale = inCommon.viewport.zw / 4.0;
	vec3 random = texture(inNoise, screenCoord * noiseScale).xyz;
	
	// Tangent Space -> World.
	random = normalize(random - dot(random, N) * N);
    vec3 B = normalize( cross(random, N) );
	mat3 TBN = mat3(random, B, N);
	
	//
	float SSAO = 0.0;
	float gds = 1.0;// smoothstep(0.05, 1.0, gDepth * 0.5 + 0.25);
	float radius = 1.2;
	float bias = 0.025;
	
	
	for(int i = 0; i < NUM_SAMPLES; ++i)
	{
		// Sample Tangent->World
		vec3 V = TBN * inSamples[i];
		V = P + V * radius * gds; 
		
		// Sample to screen space...
		vec4 sampleOffset = vec4(V, 1.0);
		sampleOffset = inCommon.viewProjMatrix * sampleOffset;
		sampleOffset.xyz /= sampleOffset.w;
		sampleOffset.xyz  = sampleOffset.xyz * 0.5 + 0.5;
		
		// Range & Test Depth...
		float gDepthSample = texture(inDepth, sampleOffset.xy * uvScale).r;
		vec3 gN = texture(inNormal,  sampleOffset.xy * uvScale).xyz;
		float gBias = bias * dot(gN, N) + 0.0005;
		
		float gDepthSampleLinaer = DepthToLinaer(gDepthSample);
		
		float range = smoothstep(1.0, 0.0, (radius * 0.5) / abs(gDepthLinear - gDepthSampleLinaer)); // Scale based on distance
		
		SSAO += (gDepthSample >= gDepth - gBias ? 1.0 : range);
	}  
	

	SSAO = SSAO / NUM_SAMPLES;
	
	// Remove Far from Depth...
	if (gDepth == 1.0)
	{	
		SSAO = 1.0;
	}
	
	outFinalColor.r = smoothstep(0.2, 1.2, SSAO * SSAO);
}



