// CommonLight.glsl
//		- contain common light functions used by mutliple shaders that does lighting calculation.
//  References Used for PBR & IBL:
//		- https://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
//		- https://www.trentreed.net/blog/physically-based-shading-and-image-based-lighting/
//



//	Hammersley Points on the Hemisphere:
//	   - http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html

float RadicalInverse_VdC(uint bits) 
{
     bits = (bits << 16u) | (bits >> 16u);
     bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
     bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
     bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
     bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
     return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}
 
 
 
 // Points on the Hemisphere
vec2 Hammersley(uint i, uint N)
{
	return vec2(float(i)/float(N), RadicalInverse_VdC(i));
}




// Importance Sampling - skew the samples in the direction of the normal based on roughness.
//		- https://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
//
vec3 ImportanceSampleGGX(vec2 xi, float roughness, vec3 normal)
{
	float alpha = roughness * roughness;
	

	float phi = TWO_PI * xi.x;
	float cosTheta = sqrt( (1.0 - xi.y) / (1.0 + (alpha * alpha - 1.0) * xi.y)  );
	float sinTheta = sqrt( 1.0 - cosTheta * cosTheta );
	

	vec3 H;
	H.x = sinTheta * cos(phi);
	H.y = sinTheta * sin(phi);
	H.z = cosTheta;
	

	vec3 up = abs(normal.y) > 0.001 ? vec3(0.0, 0.0, 1.0) : vec3(0.0, 1.0, 0.0);
	vec3 Tx = normalize( cross(up, normal) );
	vec3 Ty = normalize( cross(normal, Tx) );
	

	return Tx * H.x + Ty * H.y + H.z * normal;
}


