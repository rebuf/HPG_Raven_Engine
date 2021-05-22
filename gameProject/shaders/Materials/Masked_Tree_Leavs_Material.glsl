
#if STAGE_VERTEX_SHADER


vec3 ComputeMaterialVertex(vec3 inPos, vec3 inWorldPos, vec3 inNormal)
{
	return inWorldPos + inNormal * sin(inCommon.time * 0.5 + (inWorldPos.x + inWorldPos.y)) * inPos.y * 0.1;
}


#else

// Input Textures
uniform sampler2D LeavsTexture;
uniform sampler2D LeavsMaskedTexture;
uniform sampler2D LeavsNormalTexture;


void ComputeMaterial(in MaterialData inData, out MaterialOutput outParams)
{
	vec4 leavsTex = texture(LeavsTexture, inData.texCoord);
	vec4 mask = texture(LeavsMaskedTexture, inData.texCoord);

	outParams.color = sRGBToLinear(leavsTex.rgb);
	outParams.emission = vec3(0.0);
	outParams.roughness = 1.0;
	outParams.metallic = 0.0;
	outParams.specular = 1.0;
	outParams.alpha = mask.r;
	
	// This is normal in world coord.
	vec3 normal = SampleNormalMap(LeavsNormalTexture, inData.texCoord);
	outParams.normal = TangentToWorld(normal, inData.normal, inData.tangent);
}


#endif
