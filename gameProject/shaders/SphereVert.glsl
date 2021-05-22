#version 450 core


// Input Attributes...
layout(location=0) in vec3 inPosition;


#if !defined(DRAW_SKY) && !defined(DRAW_SKY_CUBE_MAP)
uniform mat4 inViewProjMatrix;
#endif


#if !defined(DRAW_SKY_CUBE_MAP)

// Vertex Shader Output.
out VertexOutput
{
	vec3 position;
} outVertex;

#endif



// -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- 
// -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- 





void main()
{
#if defined(DRAW_SKY)
	gl_Position = inCommon.viewProjMatrix * vec4(inPosition * 30000.0, 1.0);
	outVertex.position = inPosition;
#elif defined(DRAW_SKY_CUBE_MAP)
	gl_Position = vec4(inPosition, 1.0);
#else
	gl_Position = inViewProjMatrix * vec4(inPosition, 1.0);
	outVertex.position = inPosition;
#endif

}

