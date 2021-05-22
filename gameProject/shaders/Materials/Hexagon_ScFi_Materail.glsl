


// Input Textures
uniform sampler2D HeightTexture;
uniform sampler2D MetallicTexture;
uniform sampler2D RoughnessTexture;
uniform sampler2D NormalTexture;



// Hexagon Materail:
//
void ComputeMaterial(in MaterialData inData, out MaterialOutput outParams)
{
	outParams.color = vec3(1.0);
	outParams.emission = vec3(0.0);
	
	vec2 uv = inData.texCoord * 5.0;
	float metal = texture(MetallicTexture, uv).r;

	if (pow(metal, 0.6) > 0.73)
	{
		float dist = length(inData.position * 0.1);
		outParams.emission = mix(vec3(2.0, 0.0, 0.0), vec3(0.0, 0.0, 2.0), abs(sin(dist - inCommon.time * 2.0)));
	}

	
	
	outParams.roughness = pow(texture(RoughnessTexture, uv).r, 2.2);
	outParams.metallic = clamp(metal + 0.5, 0.0, 1.0);
	outParams.specular = 1.0;
	
	
	
#if RENDER_SHADER_TYPE_TRANSLUCENT
	outParams.alpha = inMaterial.Alpha;
#else
	outParams.alpha = 1.0;
#endif
	
	// Normal Map...
	vec3 normal = SampleNormalMap(NormalTexture, uv) * vec3(4.0, 1.0, 4.0);
	outParams.normal = TangentToWorld(normal, inData.normal, inData.tangent);
}

