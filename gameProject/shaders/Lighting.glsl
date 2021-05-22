//	Lighting.glsl:
//		- shared between multiple shader types and domains to perform lighting.
//		- contain functionality to help with lighting, deferred render, etc...
//
//  References Used for PBR & IBL:
//		- https://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
//		- https://www.trentreed.net/blog/physically-based-shading-and-image-based-lighting/
//




// Make sure to define MAX_LIGHTS
#ifndef MAX_LIGHTS
#error MAX_LIGHTS should be defined.
#endif



#define TYPE_INVALID_LIGHT 0
#define TYPE_DIR_LIGHT 1
#define TYPE_SPOT_LIGHT 2
#define TYPE_POINT_LIGHT 3


#define NUM_ENV_MAP_LOD 5.0




 
// Lighting Input...
layout(std140) uniform LightingBlock
{
	// Direction (XYZ).
	vec4 lightDir[MAX_LIGHTS];
	
	// Position (XYZ).
	vec4 lightPos[MAX_LIGHTS];
	
	// Light Color(RGB), Power(A).
	vec4 lightPower[MAX_LIGHTS];
	
	// Type(X), Inner Angle(Y), Outter Angle(Z), Radius(W).
	vec4 lightData[MAX_LIGHTS];
	
} inLighting;



// Sky Environment Map.
uniform samplerCube inSkyEnvironment;
uniform sampler2D inEnvBRDF;



// Data used to light a surface.
struct LightSurfaceData
{
	// The Surface Position in World Space.
	vec3 p;
	
	// The Surface Normal in World Space.
	vec3 n;
	
	// The direction from the surface to the view.
	vec3 v;
	
	// Base Reflective Index
	vec3 f0;
	
	// The Surface Albedo(Color)
	vec3 albedo;
	
	// The roughness factor [0, 1] 0:Rough - 1:Smooth
	float roughness;
	
	// The metallic factor [0, 1] 0:Plastic - 1:Metallic
	float metallic;
	
	// Specular Factor used to control overall specular reflection.
	float specular;
	
	// Dot product between surface normal and view vector.
	float NDotV;

	// The surface type.
	// 0: Default Surface.
	// 1: Masked Foliage.
	int type;
	
	// Ambient Occlusion.
	float AO;
};





// --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- 


// Normal Distribution Function - GGX [Trowbridge-Reitz]
float ComputeDistributionGGX(float roughness, float NDotH)
{
	float alpha = roughness * roughness;
	float d = (NDotH * NDotH) * (alpha - 1.0) + 1.0;
	return alpha / (PI * d * d);
}


// Geometry Function - GGX [Schlick model] / Smith
float ComputeGeomGGX(float roughness, float NDotV, float NDotL)
{
	float k = (roughness + 1.0);
	k = (k * k) * 0.125;
	
	float G1 = NDotV / (NDotV * (1.0 - k) + k);
	float G2 = NDotL / (NDotL * (1.0 - k) + k);
	
	return G1 * G2;
}


// Fresnel - Schlick approximation + Spherical Gaussian approximation
vec3 ComputeFresnel(vec3 f0, float VDotH)
{
	return f0 + (1.0 - f0) * pow(2, (-5.55473 * VDotH - 6.98316) * VDotH);
}


vec3 ComputeFresnelRoughness(vec3 f0, float VDotH, float roughness)
{
	return f0 + (max(vec3(1.0 - roughness), f0) - f0) * pow(2, (-5.55473 * VDotH - 6.98316) * VDotH);
}  




// Compute Physically Based Rendering BRDF:
// 		- Cook-Torrance microfacet specular shading model.
//
vec3 ComputeBRDFLighting(in LightSurfaceData surface, float NDotH, float NDotV, float NDotL, float VDotH)
{
	// BRDF specular funtion DGF.
	float Dggx = ComputeDistributionGGX(surface.roughness, NDotH);
	float Gggx = ComputeGeomGGX(surface.roughness, NDotV, NDotL);
	vec3 Fr = ComputeFresnel(surface.f0, VDotH);
	
	// Specular & Diffuse Terms
	vec3 Ks = (Dggx * Gggx * Fr) / (4.0 * NDotL * NDotV);
	Ks *= surface.specular;
	
	vec3 Kd = surface.albedo * ONE_OVER_PI;
	
	// Incoming Light == Outgoing Light
	Kd *= (1.0 - Ks);
	
	// Non-Dielectric Surface - No Diffuse.
	Kd *= (1.0 - surface.metallic);
	
	return (Kd + Ks) * NDotL;
}


// Compute the specular form Env IBL.
vec3 ComputeIBL(in LightSurfaceData surface)
{
	vec3 Fr = ComputeFresnelRoughness(surface.f0, surface.NDotV, surface.roughness);

	// Approximate Specular IBL...
	vec3 r = reflect(-surface.v, surface.n);
	
	vec3 prefiltered = textureLod(inSkyEnvironment, r, surface.roughness * (NUM_ENV_MAP_LOD - 1.0)).rgb;
	vec2 brdf = texture(inEnvBRDF, vec2(surface.roughness, surface.NDotV) ).xy;
	vec3 Ks = prefiltered * ( Fr * brdf.x + brdf.y );
	Ks *= surface.specular;
	
	
	// Trying to compute diffuse from environemnt pre-filtered specular roughness 1.0.
	vec3 Kd = textureLod(inSkyEnvironment, surface.n, NUM_ENV_MAP_LOD - 1.0).rgb;
	Kd *= (1.0 - Fr);
	Kd *= (1.0 - surface.metallic);
	
	return (Kd * surface.albedo + Ks) * surface.AO;
}



// Compute The light from the environemnt sun.
vec3 ComputeSunLight(in LightSurfaceData surface)
{
	// ...
	vec3 multiColor = vec3(1.0);
	
	// The Light Data
	vec3 l = -inCommon.sunDir.xyz;
	vec3 h = normalize(l + surface.v);
	float NDotL = dot(surface.n, l);
	
	
	// Is Masked Foliage?
	if (surface.type == 1)
	{
		if (NDotL < 0.0)
		{
			NDotL *= -1.0;
			surface.n *= -1.0;
			multiColor = mix(surface.albedo, vec3(0.7), 0.24);
		}
	}
	
	
	NDotL = max(NDotL, 0.0001);
	float NDotH = max(dot(surface.n, h), 0.0001);
	float VDotH = max(dot(surface.v, h), 0.00001);
	
	// BRDF
	vec3 brdf = ComputeBRDFLighting(surface, NDotH, surface.NDotV, NDotL, VDotH);
	
	// SUN SHADOW.
#ifdef SHADOW_ENABLED
	float shadow = SunShadowCalculation(surface.p, surface.n, l);
#else
	float shadow = 0.0;
#endif
	
	
	// Return the sun lighting
	return brdf * inCommon.sunColorAndPower.rgb * multiColor * inCommon.sunColorAndPower.a * (1.0 - shadow);
}




// Compute a light from light data at light index.
vec3 ComputeLight(in LightSurfaceData surface, int lightIdx)
{
	// General Light Data...
	vec4 lightPower = inLighting.lightPower[lightIdx];
	vec4 lightData = inLighting.lightData[lightIdx];
	int lightType = int(lightData.x);
	
	
	// Data
	vec3 l;
	vec3 h;
	float dist;
	float attenuation;

	
	
	// Compute Data fore each light type...
	switch (lightType)
	{
	// No Light in this slot.
	case TYPE_INVALID_LIGHT: return vec3(0.0);
	
	//
	case TYPE_DIR_LIGHT:
		l = -inLighting.lightDir[lightIdx].xyz;
		dist = 0.0; // Infinity.
		attenuation = 1.0; // No Attenuation.
		break;
		
	//
	case TYPE_SPOT_LIGHT:
	case TYPE_POINT_LIGHT:
	{
		l = inLighting.lightPos[lightIdx].xyz - surface.p; 
		dist = length(l);
		
		float attenBase = pow(max(1.0 - pow(dist / lightData.w, 4), 0.0), 2);
		attenuation = attenBase / (dist * dist + 1.0);
		
		l /= dist;
	}
		break;
		
	};
	
	
	// Spot Light Angles
	if (lightType == TYPE_SPOT_LIGHT)
	{
		vec2 angles = lightData.yz;
		vec3 dir = -inLighting.lightDir[lightIdx].xyz;
		
		float a = dot(l, dir);
		lightPower.a *= clamp((a - angles.y) / (angles.x - angles.y), 0.0, 1.0); 
	}
	
	float NDotL = dot(surface.n, l);
	
	// Is Masked Foliage?
	if (surface.type == 1)
	{
		if (NDotL < 0.0)
		{
			NDotL *= -1.0;
			surface.n *= -1.0;
		}
	}
	
	
	NDotL = max(NDotL, 0.0001);
	h = normalize(l + surface.v);
	float NDotH = max(dot(surface.n, h), 0.0001);
	float VDotH = max(dot(surface.v, h), 0.00001);
	
	// BRDF
	vec3 brdf = ComputeBRDFLighting(surface, NDotH, surface.NDotV, NDotL, VDotH);
	brdf *= lightPower.rgb * lightPower.a; // Color & Power
	brdf *= attenuation;
		
	return brdf;
}






//	Compute Scene Lighting form sun and light data.
vec3 ComputeLighting(in LightSurfaceData surface)
{
	// Compute Environment Sun
	vec3 finalLighting = ComputeSunLight(surface);
	
	// Environment IBL
	finalLighting += ComputeIBL(surface);
	

	// TODO: This should only be done for forward pass, deferred pass we need to implement light proxies
	// Many Lights.
	for (int lightIndex = 0; lightIndex < MAX_LIGHTS; ++lightIndex)
	{
		finalLighting += ComputeLight(surface, lightIndex);
	}


	return finalLighting;
}






