#version 450 core






// Vertex Shader Output.
in VertexOutput
{
	vec3 position;
	vec3 normal;
	vec3 tangent;
	vec2 texCoord;
} inFrag;







void main()
{

#if RENDER_SHADER_TYPE_MASKED || RENDER_SHADER_TYPE_MASKED_FOLIAGE
	// Normalize Input Normal
	vec3 normal = normalize(inFrag.normal);
	vec3 tangent = normalize(inFrag.tangent);

	
	// Material Input...
	MaterialData matIn;
	matIn.position = inFrag.position;
	matIn.normal = normal;
	matIn.tangent = inFrag.tangent;
	matIn.texCoord = inFrag.texCoord;
	
	
	// Material Compute Output.
	MaterialOutput matOut;
	
	// Compute Material Function...
	ComputeMaterial(matIn, matOut);
	
	// Masking...
	// TODO: Alpha Dithering for better transition.
	if (matOut.alpha < 0.5)
	{
		// discard fragment.
		discard;
	}
#endif

}






