// FinalPostProcessing:
//		- here we do gamma correction, tone mapping, exposure, and color grading


#version 450 core






// Vertex Shader Output.
in VertexOutput
{
	//
	vec2 rtCoord;
} inFrag;




// Post-Processing Input.
uniform sampler2D inPPInput0;



// Output.
layout(location = 0) out vec4 outFinalColor;



#define LUMA_FACTORS vec3(0.299, 0.587, 0.114)



// Tone Mapping Uncharted2:
//    - John Hable, Uncharted 2: HDR Lighting, GDC, https://www.gdcvault.com/play/1012351/Uncharted-2-HDR
vec3 Tonemap(vec3 color) 
{

// Approxiate Filmic
#define UNCHARTED_TONE_OP(x) ( ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F )

	float A = 0.15;
	float B = 0.50;
	float C = 0.10;
	float D = 0.20;
	float E = 0.02;
	float F = 0.30;
	float W = 11.2;
	
	
	// Exposure Bias.
	float Exp = 2.5;
	
	A = 0.25;
	C = 0.08;
	
	
	// Apply Tone Mapping...
	vec3 tone = UNCHARTED_TONE_OP(Exp * color);
	tone = tone / UNCHARTED_TONE_OP(W);
	
	// Gamma.
	return LinearTosRGB(tone);
}



//
vec3 Exposure(vec3 color, float ex)
{
	return exp2(ex) * color.rgb;
}


//
vec3 Saturation(vec3 color, float sat)
{
	float luma = dot(LUMA_FACTORS, color);
	return luma + sat * (color - luma);
}



uniform sampler2D temp;



void main()
{
	vec4 color = texture(inPPInput0, inFrag.rtCoord);
	
	
	// TODO Color Gradding...
	// ~
	
	
	// Tone Mapping
	outFinalColor.rgb = Tonemap(color.rgb);
	
	// Store luma in alpha channel.
	outFinalColor.a = dot(outFinalColor.rgb, LUMA_FACTORS); 
	
	
	vec2 screenCoord = ComputeScreenCoord();
	

	//outFinalColor.rgb = texture(temp, inFrag.rtCoord).rrr;
	//outFinalColor.a = outFinalColor.r;

	
}



