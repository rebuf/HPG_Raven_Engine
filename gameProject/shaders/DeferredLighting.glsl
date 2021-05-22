#version 450 core



// Vertex Shader Output.
in VertexOutput
{
	//
	vec2 rtCoord;
} inFrag;



// Input G-Buffer...
uniform sampler2D inAlbedo;
uniform sampler2D inNormal;
uniform sampler2D inBRDF;
uniform sampler2D inDepth;
uniform sampler2D inAO;


layout(location=0) out vec4 outFinalColor;




// --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- 




void main()
{	
	vec2 screenCoord = ComputeScreenCoord();
	
	// G-Buffer.
	vec4 gAlbedo = texture(inAlbedo, inFrag.rtCoord);
	vec4 gBRDF = texture(inBRDF, inFrag.rtCoord);
	float gDepth = texture(inDepth, inFrag.rtCoord).r;
	float gAO = texture(inAO, inFrag.rtCoord).r;
	
	// Surface Data from G-Buffer used for lighting the surface.
	LightSurfaceData surface;
	surface.n = normalize(texture(inNormal, inFrag.rtCoord).xyz);
	surface.albedo = gAlbedo.rgb;
	surface.specular = gBRDF.b;
	surface.roughness = max(gBRDF.r, 0.01);
	surface.metallic = gBRDF.g;
	surface.type = int(gAlbedo.a * 255.0);
	surface.AO = gAO;
	
	// Reconstruct World Position from depth.
	surface.p = ComputeWorldPos(screenCoord, gDepth);
	
	// View Direction Vector.
	surface.v = normalize(inCommon.viewPos - surface.p);
	surface.NDotV = max(dot(surface.n, surface.v), 0.0001);
	
	// Base Reflective Index, 0.04 similar to UE4
	surface.f0 = mix(vec3(0.04), surface.albedo, surface.metallic); 
	
	
	// Compute Lighting on the surface
	vec3 lighting = ComputeLighting(surface);
	
	// Final Output Color.
	outFinalColor.rgb = lighting;
	
	// Temp Sky...
	if (gDepth == 1.0)
	{
		outFinalColor.rgb = textureLod(inSkyEnvironment, -surface.v, NUM_ENV_MAP_LOD - 1.0).rgb;
	}
	

}


