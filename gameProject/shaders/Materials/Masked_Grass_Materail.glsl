
#if STAGE_VERTEX_SHADER


vec3 ComputeMaterialVertex(vec3 inPos, vec3 inWorldPos, vec3 inNormal)
{
	return inWorldPos + inNormal * sin(inCommon.time * 6.0 + (inWorldPos.x + inWorldPos.y)) * inPos.y * 0.1;
}


#else

// Input Textures
uniform sampler2D GrassTexture;



void ComputeMaterial(in MaterialData inData, out MaterialOutput outParams)
{
	vec4 grassTex = texture(GrassTexture, inData.texCoord);

	outParams.color = sRGBToLinear(grassTex.rgb) * vec3(0.7, 1.0, 0.0);
	outParams.emission = vec3(0.0);
	outParams.roughness = 1.0;
	outParams.metallic = 0.0;
	outParams.specular = 1.0;
	outParams.alpha = grassTex.a;
	
	// This is normal in world coord.
	outParams.normal = inData.normal;
}


#endif
