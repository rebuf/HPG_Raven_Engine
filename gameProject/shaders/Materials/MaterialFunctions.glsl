// Material Functions:
//      - are functions we use to override the materail computation, for example when running the fragment shader we call
//        the materail function to provide us with the materail parameters, these parameters can be computed in any way we
//        want in these functions.
//
//      - Material functions can have their own uniform input that is used to compute the paramters, a simple example is using 
//        texture sampler2D and assign the sampled value to the material output color.



// if defined will override the default vertex function.
#ifndef MATERIAL_FUNCTION_OVERRIDE
#define MATERIAL_FUNCTION_OVERRIDE 0
#endif

// if defined will override the default materail function.
#ifndef MATERIAL_VERTEX_OVERRIDE
#define MATERIAL_VERTEX_OVERRIDE 0
#endif




#if STAGE_VERTEX_SHADER

// If no materail function provided use default.
#if !MATERIAL_VERTEX_OVERRIDE
// Used by vertex shader to compute 
vec3 ComputeMaterialVertex(vec3 inPos, vec3 inWorldPos, vec3 inNormal)
{
	return inWorldPos;
}
#endif

#endif  // End of STAGE_VERTEX_SHADER




#if STAGE_FRAGMENT_SHADER


// Input data that can be used by the material function.
struct MaterialData
{
	// The position in world coordinate.
	vec3 position;
	
	// The normal in world coordinate.
	vec3 normal;
	
	// The tangent in world coordinate.
	vec3 tangent;
	
	// The texture coordinate.
	vec2 texCoord;
};



// The Output of the material function used to define materail
//     attributes.
struct MaterialOutput
{
	// The base color value of the materail.
	vec3 color;
	
	// The emission value.
	vec3 emission;
	
	// The normal in world coordinate.
	vec3 normal;
	
	// Material Roughness -> [0.0, 1.0].
	float roughness;
	
	// Material Plastic & Metallic -> [0.0, 1.0].
	float metallic;
	
	// Specular value, controls the overall specularity.
	float specular;
	
	// Alpha value, used for translucent and masked materials.
	float alpha;
};



// If no materail function provided use default.
#if !MATERIAL_FUNCTION_OVERRIDE
// The Function to override for custom materails.
void ComputeMaterial(in MaterialData inData, out MaterialOutput outParams)
{
	outParams.color = vec3(1.0, 0.0, 0.0);
	outParams.emission = vec3(0.0);
	outParams.roughness = 0.0;
	outParams.metallic = 0.0;
	outParams.specular = 1.0;
	outParams.alpha = 1.0;
	
	// This is normal in world coord.
	outParams.normal = inData.normal;
}
#endif





// Sample normal from normal map.
vec3 SampleNormalMap(sampler2D tex, vec2 coord)
{
	return texture(tex, coord).xyz * 2.0 - 1.0;
}


// Convert [-1, 1] Normal Texture values to World Normal.
vec3 TangentToWorld(vec3 NV, vec3 N, vec3 T)
{	
	// Tangent Matrix
	mat3 TBN = ComputeTBNMatrix(N, T);
	
	return NV * TBN;
}




#endif // End of STAGE_FRAGMENT_SHADER




