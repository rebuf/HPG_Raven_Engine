#version 450 core





// Vertex Shader Output.
in VertexOutput
{
	vec3 position;
} inFrag;



// 
uniform samplerCube inEnvTexture;



// Output...
out vec4 outFragColor;




// Pre-fitler Irradiance:
//    -
//
// References:
//    - http://www.codinglabs.net/article_physically_based_rendering.aspx
//
vec3 ComputeIrradiance(vec3 normal)
{
    vec3 irradiance = vec3(0.0);
    float numSamples = 0.0;
	
	// Compute Basis 
	vec3 up = abs(normal.y) > 0.001 ? vec3(0.0, 0.0, sign(-normal.y)) : vec3(0.0, 1.0, 0.0);
	vec3 right = normalize( cross(up, normal) );
	up = normalize( cross(normal, right) );
	
	// Delta
	float phidlt = 0.1;
	float thetadlt = 0.025;
	
	// Compute Irradiance from by sampling Environment Texture Hemisphere...
	for (float phi = 0; phi < TWO_PI; phi += phidlt)
	{
		for (float theta = 0; theta < HALF_PI; theta += thetadlt)
		{
			float sinTheta = sin(theta);
			float cosTheta = cos(theta);
			
			vec3 base0 = cos(phi) * right + sin(phi) * up;
			vec3 sv = cosTheta * normal + sinTheta * base0; // Sample Vector in the hemisphere
			
			irradiance += texture(inEnvTexture, sv).rgb * cosTheta * sinTheta;
			numSamples += 1.0;
		}
	}
	
	
	return PI * irradiance / numSamples;
	
}



void main()
{
	vec3 normal = normalize(inFrag.position);
	outFragColor.rgb = ComputeIrradiance(normal);
	
}
