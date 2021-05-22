#version 450 core



#ifdef GEOEMTRY_SHADER_LINES
// Geometry Shader Output.
in GeomOutput
{
	vec3 position;
	vec2 texCoord;
} inFrag;
#else
// Vertex Shader Output.
in VertexOutput
{
	vec3 position;
} inFrag;
#endif




layout(location=0) out vec4 outFragColor;



void main()
{
	vec3 v = inFrag.position - inCommon.viewPos;
	float d2 = dot(v, v);
	
	outFragColor.rgb = vec3(1.0, 1.0, 1.0);
	outFragColor.a = smoothstep(100000.0, 0.0, d2 * 9.0);
	
	if (abs(inFrag.position.x) < SMALL_NUM * 10.0)
		outFragColor.rgb = vec3(1.0, 0.0, 0.0);
		
	if (abs(inFrag.position.z) < SMALL_NUM * 10.0)
		outFragColor.rgb = vec3(0.0, 0.0, 1.0);
	
}

