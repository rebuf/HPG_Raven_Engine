#version 450 core



layout(location=0) in vec3 inPosition;



void main()
{
	gl_Position = inCommon.viewProjMatrix * inModelMatrix * vec4(inPosition, 1.0);
}

