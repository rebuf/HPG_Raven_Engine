#version 450 core


// Input Attributes...
layout(location=0) in vec3 inPosition;





// Vertex Shader Output.
out VertexOutput
{
	vec3 position;
} outVertex;



// -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- 
// -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- 





void main()
{
	//
	gl_Position = inCommon.viewProjMatrix * vec4(inPosition, 1.0);
	outVertex.position = inPosition;
}

