#version 450 core



#ifndef NUM_SAMPLES
#error You Must Define NUM_SAMPLES
#endif




// Vertex Shader Output.
in VertexOutput
{
	vec3 position;
} inFrag;



//The Envrionment map to filter.
uniform samplerCube inEnvTexture;

// Roughness Value to filter.
uniform float inRoughness;


// Output...
out vec4 outFragColor;






// Pefilter Specular:
//  - https://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
//
vec3 FilterSpecularIBL(float roughness, vec3 normal)
{
	vec3 filterColor = vec3(0.0);
	
	const uint numSamples = NUM_SAMPLES;
	float weights = 0.0;
	
	for (uint i = 0; i < numSamples; ++i)
	{
		vec2 xi = Hammersley(i, numSamples);
		vec3 H = ImportanceSampleGGX(xi, roughness, normal);
		vec3 L = 2 * dot(normal, H) * H - normal;
		float NDotL = max(dot(normal, L), 0.0);
		
		if (NDotL > 0.0)
		{
			filterColor += texture(inEnvTexture, H).rgb * NDotL;
			weights += NDotL;
		}
	}
	
	return filterColor / weights;
}





void main()
{
	vec3 normal = normalize(inFrag.position);
	
	outFragColor.rgb = FilterSpecularIBL(inRoughness, normal);
}
