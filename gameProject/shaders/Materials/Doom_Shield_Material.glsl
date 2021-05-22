


// Input Textures
uniform sampler2D MetallicHexTexture;
uniform sampler2D NormalHexTexture;



// Hexagon Materail:
//
void ComputeMaterial(in MaterialData inData, out MaterialOutput outParams)
{
	vec3 vv = normalize(inCommon.viewPos - inData.position);
	float vf =  dot(vv, inData.normal);
	
	outParams.color = vec3(0.0, 0.0, 1.0);
	outParams.emission = vec3(0.0);
	

	
	vec2 uv = inData.texCoord * vec2(2.5, 2.0);
	float metal = texture(MetallicHexTexture, uv).r;
	float hex = 0.0;
	
	if (pow(metal, 0.6) > 0.73)
	{
		float dist = length(inData.position * 0.1);
		hex = 1.0;
	}

	
	
	outParams.roughness = 0.15;
	outParams.metallic = 1.0;
	outParams.specular = 1.0;
	
	

	outParams.alpha = mix(pow((1.0 - vf) + 0.2, 4.2), 0.7, abs(sin(inCommon.time * 2.0 + vf)) );
	
	// Normal Map...
	vec3 normal = SampleNormalMap(NormalHexTexture, uv);
	outParams.normal = TangentToWorld(normal, inData.normal, inData.tangent);
}

