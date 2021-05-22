
#if STAGE_VERTEX_SHADER


vec3 ComputeMaterialVertex(vec3 inPos, vec3 inWorldPos, vec3 inNormal)
{
	return inWorldPos + inNormal * abs(sin(inCommon.time * 2.0 + inPos.y * 0.5)) * 1.2;
}


#else


void ComputeMaterial(in MaterialData inData, out MaterialOutput outParams)
{
	vec3 vv = normalize(inCommon.viewPos - inData.position);
	float vf =  dot(vv, inData.normal);
	
	outParams.color = vec3(1.0);
	outParams.emission = vec3(vf);
	outParams.roughness = 0.0;
	outParams.metallic = 1.0;
	outParams.specular = 1.0;
	
	outParams.alpha = 1.0;
	
	// This is normal in world coord.
	outParams.normal = inData.normal;
}


#endif
