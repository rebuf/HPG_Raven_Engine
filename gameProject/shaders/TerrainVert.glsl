#version 450 core



layout(location=0) in vec3 inPosition;





// Vertex Shader Output.
out VertexOutput
{
	vec3 position;
} outVertex;



// Bin Data Uniform 
layout(std140) uniform TerrainBinBlock
{	
	// Scale of the entire terrain.
	vec2 inScale;
	
	// Min/Max height of the terrain.
	vec2 inHeight;

	// Terrain Bin Offset.
	vec2 inOffset;
	
	// Terrain UV Scaling.
	vec2 inUVScale;
	
};





// Uniforms...
uniform sampler2D inHeightMap;



// -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- 






void main()
{
	vec3 worldPos =  vec3(inPosition.x + inOffset.x, 0.0, inPosition.z + inOffset.y);

	vec2 uv = vec2(worldPos.x / inScale.x, worldPos.z / inScale.y);
	worldPos.y = texture(inHeightMap, uv).r * (inHeight.y - inHeight.x) + inHeight.x; 
	
#if MATERIAL_VERTEX_OVERRIDE
#error Terrain does not support materail vertex function.
#endif
	
	gl_Position = vec4(worldPos, 1.0);
	
	outVertex.position = worldPos;

}





