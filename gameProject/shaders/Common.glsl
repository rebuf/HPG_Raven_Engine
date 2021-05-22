// Stage Macros:
//		- Macro used to determine which stage we are in, used when you have a file that is shared between multiple stages.
//		- if the macro is not defined the define its default to zero.
#ifndef STAGE_VERTEX_SHADER
#define STAGE_VERTEX_SHADER 0
#endif

#ifndef STAGE_FRAGMENT_SHADER
#define STAGE_FRAGMENT_SHADER 0
#endif

#ifndef STAGE_GEOMETRY_SHADER
#define STAGE_GEOMETRY_SHADER 0
#endif

#ifndef STAGE_TESS_CONTROL_SHADER
#define STAGE_TESS_CONTROL_SHADER 0
#endif

#ifndef STAGE_TESS_EVALUATION_SHADER
#define STAGE_TESS_EVALUATION_SHADER 0
#endif



// Render Pass Macros:
//		- Macro used to determine which render pass we are currently in.
#ifndef RENDER_PASS_DEFERRED
#define RENDER_PASS_DEFERRED 0
#endif


#ifndef RENDER_PASS_FORWARD
#define RENDER_PASS_FORWARD 0
#endif




// Shader Type Macros:
//		- Macro used to determine what is the shader type.
#ifndef RENDER_SHADER_TYPE_OPAQUE
#define RENDER_SHADER_TYPE_OPAQUE 0
#endif


#ifndef RENDER_SHADER_TYPE_MASKED
#define RENDER_SHADER_TYPE_MASKED 0
#endif


#ifndef RENDER_SHADER_TYPE_MASKED_FOLIAGE
#define RENDER_SHADER_TYPE_MASKED_FOLIAGE 0
#endif	


#ifndef RENDER_SHADER_TYPE_TRANSLUCENT
#define RENDER_SHADER_TYPE_TRANSLUCENT 0
#endif


#ifndef RENDER_SHADER_TYPE_POSTPROCESSING
#define RENDER_SHADER_TYPE_POSTPROCESSING 0
#endif


#ifndef RENDER_SHADER_TYPE_SHADOW
#define RENDER_SHADER_TYPE_SHADOW 0
#endif


#ifndef RENDER_SHADER_CUSTOM
#define RENDER_SHADER_CUSTOM 0
#endif


#ifndef RENDER_SHADER_MESH_INSTANCE
#define RENDER_SHADER_MESH_INSTANCE 0
#endif






// --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- 








// General uniform data that can be used by all shaders
layout(std140) uniform CommonBlock
{
	// The View & Projection Matrix of the scene.
	mat4 viewProjMatrix;
	
	// The Inverse of View & Projection Matrix of the scene.
	mat4 viewProjMatrixInverse;
	
	// The View Direction of the scene.
	vec3 viewDir;
	
	// The View Position of the scene.
	vec3 viewPos;
	
	// The render viewport postion and size, X,Y: Position, and Z,W: Size.
	//     - Values Ranges [0, width], [0, Height]
	vec4 viewport;
	
	// The environment sun direction.
	vec4 sunDir;
	
	// The environment sun color(RGB) & power(A).
	vec4 sunColorAndPower;
	
	// X: Near Plane, Y: Far Plane.
	vec2 nearFar;
	
	// Application Time.
	float time;
	
} inCommon;


// --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- 


#define NEAR_VALUE inCommon.nearFar.x
#define FAR_VALUE inCommon.nearFar.y


#define PI 3.14159265
#define TWO_PI 6.2831853
#define HALF_PI 1.5707963
#define ONE_OVER_PI 0.3183099
#define SMALL_NUM 0.000001



// --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- 





// Compute world position from depth.
vec3 ComputeWorldPos(vec2 coord, float depth)
{
	depth = depth * 2.0 - 1.0;
	coord = coord * 2.0 - 1.0;
	
	vec4 wpos = inCommon.viewProjMatrixInverse * vec4(coord, depth, 1.0);
	return wpos.xyz / wpos.w;
}


// Convert Depth map value to a linear depth in eye-space.
float DepthToLinaer(float depth) 
{
    depth = depth * 2.0 - 1.0;
    return (2.0 * NEAR_VALUE * FAR_VALUE) / (FAR_VALUE + NEAR_VALUE - depth * (FAR_VALUE - NEAR_VALUE));	
}



#if STAGE_FRAGMENT_SHADER

// Return the coordinate [0, 1] for the currrent fragment.
vec2 ComputeScreenCoord()
{
	return gl_FragCoord.xy / inCommon.viewport.zw;
}

#endif


// Compute Direction from polar coordinate in OpenGL Cooridnate.
// @param theta: Horziotnal Angle.
// @param phi: Vertical Angle.
vec3 ComputePolarDir(float theta, float phi)
{
	float cosPhi = cos(phi);	
	return vec3(cosPhi * sin(theta), sin(phi), cosPhi * cos(theta));
}


// Convert Equirectangular Texture Coordinate from direction.
vec2 ComputeEquiCoord(vec3 normal)
{
	vec2 uv;
	
	// Convert to polar angles and divide by 2PI, PI respectively
	uv.x = atan(normal.z, normal.x) * 0.1591549;
	uv.y = asin(normal.y) * 0.3183099;
	
	return uv + 0.5;
}


// Convert Linear to sRGB color space.
vec3 LinearTosRGB(vec3 val)
{
	return pow(val, vec3(0.4545456));
}


// Convert sRGB to Linear color space.
vec3 sRGBToLinear(vec3 val)
{
	return pow(val, vec3(2.2));
}


// Convert Linear Scalar to sRGB color space.
float LinearTosRGB(float val)
{
	return pow(val, 0.4545456);
}


// Convert sRGB Scalar to Linear color space.
float sRGBToLinear(float val)
{
	return pow(val, 2.2);
}


// Convert sample sRGB texture and convert it to Linear color space.
vec4 sRGBToLinearSample(sampler2D tex, vec2 coord)
{
	vec4 color = texture(tex, coord);
	color.rgb = pow(color.rgb, vec3(2.2));
	return color;
}


// Compute Tangnet Matrix to World Matrix.
mat3 ComputeTBNMatrix(vec3 N, vec3 T)
{
	// Orthogonalize.
	T = normalize(T - dot(T, N) * N);
	
	// Bi-Tangent.
    vec3 B = normalize( cross(T, N) );
	
	return mat3(
		vec3(T.x, B.x, N.x),
		vec3(T.y, B.y, N.y),
		vec3(T.z, B.z, N.z)
	);
}





