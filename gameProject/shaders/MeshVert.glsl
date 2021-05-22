#version 450 core


// Input Attributes...
layout(location=0) in vec3 inPosition;
layout(location=1) in vec3 inNormal;
layout(location=2) in vec3 inTangent;
layout(location=3) in vec2 inTexCoord;

#if RENDER_SHADER_MESH_INSTANCE
layout(location=4) in mat4 inInstanceTransform;
//     location=5 for mat4.  
//     location=6 for mat4.
//     location=7 for mat4.
#endif






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





void main()
{
#if RENDER_SHADER_MESH_INSTANCE
	// Transform to world space.
	vec4 worldPos = inInstanceTransform * vec4(inPosition, 1.0);
	vec4 wolrdNormal = inInstanceTransform * vec4(inNormal, 0.0);
	vec4 wolrdTangent = inInstanceTransform * vec4(inTangent, 0.0);

#else
	// Transform to world space.
	vec4 worldPos = inModelMatrix * vec4(inPosition, 1.0);
	vec4 wolrdNormal = inNormalMatrix * vec4(inNormal, 0.0);
	vec4 wolrdTangent = inNormalMatrix * vec4(inTangent, 0.0);
#endif

	
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

