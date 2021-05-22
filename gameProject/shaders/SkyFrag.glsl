#version 450 core


// References:
//     - Sean O'Neil, Chapter 16. Accurate Atmospheric Scattering, GPU Gems 2
//       https://developer.nvidia.com/gpugems/gpugems2/part-ii-shading-lighting-and-shadows/chapter-16-accurate-atmospheric-scattering
// Note:
//     - This is not the optimized version it implement everything in this shader.
//





// Vertex Shader Output.
in VertexOutput
{
	vec3 position;
} inFrag;




out vec4 outFragColor;



// --- -- - -- --- -------
//  Atmosphere Variables
// --- -- - -- --- -------

// Radius in Meters
#define RAtmo  6416000
#define REarth 6371000

// Rayleigh & Mie constant -> Wavelength k, H:Height at H0. 
#define Kr vec3(3.8e-6, 13.5e-6, 22.1e-6)
#define Km vec3(27.0e-6)
#define Hr 7800.0
#define Hm 1000.0 


// Number of samples to do.
#define NUM_EYE_SAMPLES 8
#define NUM_SUN_SAMPLES 4




// -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- -- --- -- 




// PhaseFunction:
// @param ct: cos theta.
//
float PhaseFunction(float ct)
{
	const float g = 0.9;
	const float g2 = g * g;
	
	float n = 1.5 * (1.0 - g2) * (1.0 + ct * ct);
	float d = (2.0 + g2) * pow(1.0 + g2 - 2 * g * ct, 1.5);
	
	return n / d;
}


// RayleighPhaseFunction:
//     - same as PhaseFunction but g = 0.
//
// @param ct: cos theta.
//
float RayleighPhaseFunction(float ct)
{
	return 0.75 * (1.0 + ct * ct);
}


// RayAtmosphereIntersect
//   - Return the distance a ray intersect with atmosphere.
//
bool RayAtmosphereIntersect(vec3 ro, vec3 rd, inout float t)
{
	const float ratmo = RAtmo;

    float B = 2.0 * dot(ro, rd);
    float C = dot(ro, ro) - (ratmo * ratmo);
    t = (-B + sqrt(B*B - 4.0*C)) / 2.0 ;

	return true;
}



// RenderAtmosphere:
// @param ro: ray origin
// @param rd: ray direction 
vec3 RenderAtmosphere(vec3 ro, vec3 rd)
{
	// The Sun Direction...
	vec3 sunDir = -inCommon.sunDir.xyz;
	
	// Rayleigh & Mie scattering.
	vec3 Ir = vec3(0.0), Im = vec3(0.0);
	
	
	// Compute Phase Functions
    float ct = dot(rd, sunDir);
    float Fr = RayleighPhaseFunction(ct);
    float Fm = PhaseFunction(ct);
	
	
	// -- --- --- --- --- ---- ---- --- ---- ---- --- ---- ---- --- --
	//  Compute Atmosphere by ray marching and integrating along ray.
	// -- --- --- --- --- ---- ---- --- ---- ---- --- ---- ---- --- --
	
	
	 // Intersection distance EyeRay x Atmosphere.
	float eye_intersect;
	RayAtmosphereIntersect(ro, rd, eye_intersect);
	
	float eye_t = 0.0;
	float eye_density_r = 0.0, eye_density_m = 0.0;
	float eye_step = eye_intersect / float(NUM_EYE_SAMPLES);
	
	// In-Scattering -> Outer Integral.
	for(int ei = 0; ei < NUM_EYE_SAMPLES; ++ei)
    {
        // Ray march step along eye ray.
        vec3 eye_p = ro + rd * (eye_t + eye_step * 0.5);
        float eye_h = length(eye_p) - REarth;
		
		// Density for the eye ray
		float ei_eye_density_r = exp( -eye_h / Hr ) * eye_step;
		float ei_eye_density_m = exp( -eye_h / Hm ) * eye_step;
        eye_density_r += ei_eye_density_r;
        eye_density_m += ei_eye_density_m;
		eye_t += eye_step;
		
		
		// Intersection distance SunRay x Atmosphere.
		float sun_intersect;
		RayAtmosphereIntersect(eye_p, sunDir, sun_intersect); 
		
		float sun_t = 0.0;
		float sun_density_r = 0.0, sun_density_m = 0.0;
		float sun_step = sun_intersect / float(NUM_SUN_SAMPLES);
		
		
		//  Out-Scattering -> Inner Integral.
		for (int si = 0; si < NUM_SUN_SAMPLES; ++si)
		{
			// Ray march step along sun ray.
			vec3 sun_p = eye_p + sunDir * (sun_t + sun_step * 0.5);
			float sun_h = length(sun_p) - REarth;

			
			// Density for the sun ray
			sun_density_r += exp( -sun_h / Hr ) * sun_step;
			sun_density_m += exp( -sun_h / Hm ) * sun_step;
			sun_t += sun_step;
		}
		
		
        // In-Scattering -> Scattering along eye and sun ray.	
        vec3 te = -Kr * (eye_density_r + sun_density_r);
        vec3 ts = -Km * (eye_density_m + sun_density_m); 
        vec3 t_exp  = exp(te+ts);

        //Total scattering from both view and sun ray direction up to this point
        Ir += ei_eye_density_r * t_exp;
        Im += ei_eye_density_m * t_exp;
		
	}
	
	
	// Final multiply by phase functions, Rayleigh + Mie.
    return (Ir * Fr * Kr + Im * Fm * Km);
}




void main()
{
	vec3 eye = vec3(0, REarth + 1.0 , 0.0);
	vec3 dir = inFrag.position;
	float blend = 0.0;
	vec3 groundColor = vec3(0.01);
	
	
	if (dir.y < 0.0)
	{
		dir = reflect(dir, vec3(0.0, 1.0, 0.0));
	
#if defined(DRAW_SKY_CUBE_MAP)
		blend = 1.0;
#else
		blend = smoothstep(0.9, 1.0, dir.y);
		dir = vec3(dir.x * 100.0, dir.y * 0.01, dir.z * 100.0);
#endif
	}
	
	dir = normalize(dir);
	outFragColor.rgb = RenderAtmosphere(eye, dir);	
	outFragColor.rgb = mix(outFragColor.rgb, groundColor, blend);
}


