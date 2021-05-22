#version 330 core



layout(location=0) in vec3 inPosition;
layout(location=1) in vec3 inNormal;
layout(location=2) in vec3 inTangent;
layout(location=3) in vec2 inTexCoord;

layout(location=4) in vec4 inWeight;
layout(location=5) in vec4 inIndices;


// Vertex Shader Output.
out VertexOutput
{
	vec3 position;
	vec3 normal;
	vec3 tangent;
	vec2 texCoord;
} outVertex;




// -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- 
// -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- 



mat4 getSkinMat()
{

	int index_0 = int(inIndices.x);
	int index_1 = int(inIndices.y);
	int index_2 = int(inIndices.z);
	int index_3 = int(inIndices.w);


	float weights_0 = inWeight.x;
	float weights_1 = inWeight.y;
	float weights_2 = inWeight.z;
	float weights_3 = inWeight.w;


	mat4 bone_0 = bones[index_0];
	mat4 bone_1 = bones[index_1];
	mat4 bone_2 = bones[index_2];
	mat4 bone_3 = bones[index_3];
	
	return bone_0 * weights_0 + bone_1 * weights_1 + bone_2 * weights_2 + bone_3 * weights_3;
}


void main()
{
	mat4 skinMatrix = getSkinMat();
	
	// Transform to world space.
	vec4 worldPos = inModelMatrix * skinMatrix * vec4(inPosition, 1.0);
	vec4 wolrdNormal = inNormalMatrix * skinMatrix * vec4(inNormal, 0.0);
	vec4 wolrdTangent = inNormalMatrix * skinMatrix * vec4(inTangent, 0.0);
	
	
#if MATERIAL_VERTEX_OVERRIDE
	worldPos.xyz = ComputeMaterialVertex(inPosition, worldPos.xyz, wolrdNormal.xyz);
#endif

#if RENDER_SHADER_TYPE_SHADOW
	gl_Position = inShadowViewProj * worldPos;
#else	
	gl_Position = inCommon.viewProjMatrix * worldPos;
#endif

	
	// Set Vertex-Output...
	outVertex.position = worldPos.xyz;
	outVertex.normal = wolrdNormal.xyz;
	outVertex.tangent = wolrdTangent.xyz;
	outVertex.texCoord = inTexCoord;
	
}

