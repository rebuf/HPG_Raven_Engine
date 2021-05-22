#version 450 core




// Vertex Shader Output.
in VertexOutput
{
	//
	vec2 rtCoord;
} inFrag;




// Post-Processing Input.
uniform sampler2D inPPInput;

// This must be from a constant/uniform.
// {x_} = 1.0/screenWidthInPixels
// {_y} = 1.0/screenHeightInPixels
uniform vec2 fxaaQualityRcpFrame;


// Output.
layout(location = 0) out vec4 outFinalColor;




void main()
{

	
	
    // This used to be the FXAA_QUALITY__SUBPIX define.
    // It is here now to allow easier tuning.
    // Choose the amount of sub-pixel aliasing removal.
    // This can effect sharpness.
    //   1.00 - upper limit (softer)
    //   0.75 - default amount of filtering
    //   0.50 - lower limit (sharper, less sub-pixel aliasing removal)
    //   0.25 - almost off
    //   0.00 - completely off
    float fxaaQualitySubpix = 0.75;
	
	
    // This used to be the FXAA_QUALITY__EDGE_THRESHOLD define.
    // It is here now to allow easier tuning.
    // The minimum amount of local contrast required to apply algorithm.
    //   0.333 - too little (faster)
    //   0.250 - low quality
    //   0.166 - default
    //   0.125 - high quality 
    //   0.063 - overkill (slower)
    float fxaaQualityEdgeThreshold = 0.166;
	
	
    // This used to be the FXAA_QUALITY__EDGE_THRESHOLD_MIN define.
    // It is here now to allow easier tuning.
    // Trims the algorithm from processing darks.
    //   0.0833 - upper limit (default, the start of visible unfiltered edges)
    //   0.0625 - high quality (faster)
    //   0.0312 - visible limit (slower)
    // Special notes when using FXAA_GREEN_AS_LUMA,
    //   Likely want to set this to zero.
    //   As colors that are mostly not-green
    //   will appear very dark in the green channel!
    //   Tune by looking at mostly non-green content,
    //   then start at zero and increase until aliasing is a problem.
    float fxaaQualityEdgeThresholdMin = 0.0833;



	// FXAA...
	outFinalColor = FxaaPixelShader(
		inFrag.rtCoord,                        // pos {xy} = center of pixel
		vec4(0.0),                             // fxaaConsolePosPos.
		inPPInput,                             // Input color texture.
		inPPInput,                             // fxaaConsole360TexExpBiasNegOne
		inPPInput,                             // fxaaConsole360TexExpBiasNegTwo
		fxaaQualityRcpFrame,
		vec4(0.0),                             // fxaaConsoleRcpFrameOpt
		vec4(0.0),                             // fxaaConsoleRcpFrameOpt2
		vec4(0.0),                             // fxaaConsole360RcpFrameOpt2,
		fxaaQualitySubpix,
		fxaaQualityEdgeThreshold,
		fxaaQualityEdgeThresholdMin,
		0.0,                                   // fxaaConsoleEdgeSharpness,
		0.0,                                   // fxaaConsoleEdgeThreshold,
		0.0,                                   // fxaaConsoleEdgeThresholdMin,
		vec4(0.0)                              // fxaaConsole360ConstDir
	);
	
	
	// Reset Alpha to 1.0.
	outFinalColor.a = 1.0;
}



