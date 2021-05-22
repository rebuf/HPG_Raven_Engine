// BRDF Look up table.
//
//  References:
//		- https://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf



#version 450 core






// Vertex Shader Output.
in VertexOutput
{
	vec2 rtCoord;
} inFrag;




// Output...
layout(location=0) out vec2 outFragColor;



// Geometry Function - GGX [Schlick model] / Smith
float ComputeGeomGGX(float roughness, float NDotV, float NDotL)
{
	float k = (roughness * roughness)  * 0.5;
	
	float G1 = NDotV / (NDotV * (1.0 - k) + k);
	float G2 = NDotL / (NDotL * (1.0 - k) + k);
	
	return G1 * G2;
}




// Compute Look up table values for precomputd roughness and NDotV.
vec2 IntegrateBRDF(float NDotV, float roughness)
{
	vec3 N = vec3(0.0, 0.0, 1.0);
	vec3 V;
	V.x = sqrt(1.0 - NDotV * NDotV);
	V.y = 0;
	V.z = NDotV;
	
	float A = 0;
	float B = 0;
	
	const uint numSamples = 1024;
	
	for (uint i = 0; i < numSamples; ++i)
	{
		vec2 xi = Hammersley(i, numSamples);
		vec3 H = ImportanceSampleGGX(xi, roughness, N);
		vec3 L = 2 * dot(V, H) * H - V;
		
		float NDotL = max(L.z, 0.0);
		float NDotH = max(H.z, 0.0);
		float VDotH = max(dot(V, H), 0.0);
		
		if (NDotV > 0.0)
		{
			float G = ComputeGeomGGX(roughness, NDotV, NDotL);
			
			float G_Vis = (G * VDotH) / (NDotH * NDotV);
			float Fc = pow( 1.0 - VDotH, 5.0 );
			A += (1 - Fc) * G_Vis;
			B += Fc * G_Vis;
		}
	}
	
	return vec2( A, B ) / float(numSamples);
}



void main()
{
	outFragColor = IntegrateBRDF(inFrag.rtCoord.x, inFrag.rtCoord.y);
}
