#version 450 core



// Vertex Shader Output.
in VertexOutput
{
	//
	vec2 rtCoord;
} inFrag;




// Input G-Buffer...
uniform sampler2D inSSAO;




// Output.
layout(location = 0) out vec4 outFinalColor;




// --- --- ---- --- --- ---- --- --- ---- --- --- ---- --- --- ---- 




void main()
{
	vec2 screenCoord = ComputeScreenCoord();
	
    float blurSSAO = 0.0;
	
    for (int x = -2; x < 2; ++x) 
    {
        for (int y = -2; y < 2; ++y) 
        {
			blurSSAO += textureOffset(inSSAO, inFrag.rtCoord, ivec2(x, y)).r;
        }
    }
	
		
	blurSSAO /= 16.0;
	//blurSSAO = smoothstep(0.0, 0.8, blurSSAO);
	
    outFinalColor.rgb = vec3(blurSSAO);
}



