

// Input Basic Materail Paramters
layout(std140) uniform MaterialParamtersBlock
{
	float Emission;
	float UVScale;
} inMaterial;




// Input Textures
uniform sampler2D ColorTexture;
uniform sampler2D EmissiveTexture;
uniform sampler2D NormalTexture;
uniform sampler2D RoughnessTexture;



// Baisc Materail:
// 		- is just a simple pass through material.
//
void ComputeMaterial(in MaterialData inData, out MaterialOutput outParams)
{
	vec2 uv = inData.texCoord;
	uv *= inMaterial.UVScale;
	uv += inCommon.time * 0.1;
	
	//
	float lem = mix(0.5, 2.0, abs(sin(inCommon.time * 0.1)));

	outParams.color = sRGBToLinearSample(ColorTexture, uv).rgb;
	outParams.emission = inMaterial.Emission * texture(EmissiveTexture, uv).rgb * lem;
	outParams.roughness = texture(RoughnessTexture, uv).r * 0.2;
	outParams.metallic = 1.0;
	outParams.specular = 1.0;
	
	
	// Normal Map...
	vec3 normal = SampleNormalMap(NormalTexture, uv);
	normal.xz *= lem * 2.0; 
	outParams.normal = TangentToWorld(normal, inData.normal, inData.tangent);
}

