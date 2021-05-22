

// Bin Data Uniform 
layout(std140) uniform TerrainBinBlock
{	
	// Scale of the entire terrain.
	vec2 inScale;
	
	// Min/Max height of the terrain.
	vec2 inHeight;

	// Terrain Bin Offset.
	vec2 inOffset;
	
	// Terrain UV Scaling.
	vec2 inUVScale;
	
};




// Input Textures
uniform sampler2D GrassTexture;
uniform sampler2D GrassNormalTexture;
uniform sampler2D GrassRoughnessTexture;

uniform sampler2D RockTexture;
uniform sampler2D RockNormalTexture;
uniform sampler2D RockRoughnessTexture;


uniform sampler2D SandTexture;
uniform sampler2D SandNormalTexture;
uniform sampler2D SandRoughnessTexture;








void ComputeMaterial(in MaterialData inData, out MaterialOutput outParams)
{
	float h = inData.position.y;
	
	vec3 color;
	vec3 normal = vec3(0.0, 1.0, 0.0);
	float roughness = 1.0;
	float metallic = 0.0;
	float specular = 1.0;
	
	float uvscale = 120.0;
	vec4 rockColor = sRGBToLinearSample(RockTexture, inData.texCoord * uvscale);
	float rockRoughness = texture(RockRoughnessTexture, inData.texCoord * uvscale).r;
	vec3 rockNormal = SampleNormalMap(RockNormalTexture, inData.texCoord * uvscale);
	float rockMetal = clamp(rockColor.r * 3.7 + 0.5, 0.0, 1.0) ;
	
	
	uvscale = 120.0;
	vec4 grassColor = sRGBToLinearSample(GrassTexture, inData.texCoord * uvscale);
	float grassRoughness = sRGBToLinearSample(GrassRoughnessTexture, inData.texCoord * uvscale).r;
	vec3 grassNormal = SampleNormalMap(GrassNormalTexture, inData.texCoord * uvscale);
	grassRoughness  = clamp(grassRoughness * 4.0, 0.0, 1.0);
	float grassMetal = 0.0;
	grassColor = grassColor * vec4(0.7, 1.0, 0.7, 1.0) * 0.7;
	
	uvscale = 120.0;
	vec4 sandColor = sRGBToLinearSample(SandTexture, inData.texCoord * uvscale);
	float sandRoughness = sRGBToLinearSample(SandRoughnessTexture, inData.texCoord * uvscale).r;
	vec3 sandNormal = SampleNormalMap(SandNormalTexture, inData.texCoord * uvscale);
	float sandMetal = 0.0;
	
	
	
	// Use red map0 as noise...
	float noise = sRGBToLinearSample(RockTexture, inData.texCoord * 1.5).r;
	noise = clamp(pow(noise * 2.0 + 0.1, 2.5) * 45.0, 0.0, 2.0);
	noise = smoothstep(0.0, 1.0, noise);
	
	// Mix Based on Height...
	float Max = inHeight.y;
	float H0 = 40.0;
	float H1 = 0.5;
	float H2 = -1.0;
	
	if (h > H0)
	{
		float a = clamp((h - H0) / 25.0, 0.0, 1.0);
		color = mix(grassColor.rgb, vec3(rockColor), a);
		roughness = mix(grassRoughness, rockRoughness, a);
		metallic = mix(grassMetal, rockMetal, a);
		normal = mix(grassNormal, rockNormal, a);
		
		color = color * noise;
	}
	else if (h > H1)
	{		
		float a = clamp((h - H1) / 5.0, 0.0, 1.0);
		color = mix(sandColor.rgb, grassColor.rgb * noise, a);
		roughness = mix(sandRoughness, grassRoughness, a);
		metallic = mix(sandMetal, grassMetal, a);
		normal = mix(sandNormal, grassNormal, a);
	}
	else
	{
		color = sandColor.rgb;
		roughness = sandRoughness;
		metallic = sandMetal;
		normal = sandNormal;
	}
	
	
	
	if (inData.normal.x < 0.3 && h > H1)
	{
		float a = clamp((1.0 - inData.normal.x) / 20.0, 0.0, 1.0);
		color = mix(color, vec3(rockColor), a);
		roughness = mix(roughness, rockRoughness, a);
		metallic = mix(metallic, rockMetal, a);
		normal = mix(normal, rockNormal, a);
		
		color = color * noise;
	}
	
	

	outParams.color = color;
	outParams.emission = vec3(0.0);
	outParams.roughness = roughness;
	outParams.metallic = metallic;
	outParams.specular = specular;
	
	
	
	// Tangent...
	vec3 Q1 = dFdx(inData.position);
	vec3 Q2 = dFdy(inData.position);
	vec2 st1 = dFdx(inData.texCoord);
	vec2 st2 = dFdy(inData.texCoord);
	vec3 T = normalize(Q1*st2.t - Q2*st1.t);
	
	
	// Normal Map...
	normal = normalize(normal);
	outParams.normal = TangentToWorld(normal, inData.normal, T);
	//outParams.normal = inData.normal;
}

