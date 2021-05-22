#version 450 core


layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;


// Vertex Shader Output.
out VertexOutput
{
	vec3 position;
} outVertex;



// View & Proj matrix one for each face.
uniform mat4 inLayerViewProjMatrix[6];


// -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- 





void main()
{
	
	for (int f = 0; f < 6; ++f)
	{
		gl_Layer = f;
		
		for (int v = 0; v < 3; ++v)
		{
			outVertex.position = gl_in[v].gl_Position.xyz;
			gl_Position = inLayerViewProjMatrix[f] * gl_in[v].gl_Position;
			EmitVertex();
		}
		
		EndPrimitive();
	}
}

