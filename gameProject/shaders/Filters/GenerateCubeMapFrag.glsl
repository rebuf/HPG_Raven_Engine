#version 450 core


// Vertex Shader Output.
in VertexOutput
{
	vec3 position;
} inFrag;



// Equirectangular Texture that we want to transfrom to CubeMap.
uniform sampler2D inEquiTexture;


// Output...
out vec4 outFragColor;


void main()
{
	vec3 normal = normalize(-inFrag.position);
	vec2 texCoord = ComputeEquiCoord(normal); 

	outFragColor = pow(texture(inEquiTexture, texCoord), vec4(2.2));
}
