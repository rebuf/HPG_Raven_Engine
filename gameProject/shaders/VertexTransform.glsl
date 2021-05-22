


#ifdef RENDER_BONE_TRANSFORM

#ifndef RENDER_SKINNED_MAX_BONES
#error you must provide RENDER_SKINNED_MAX_BONES.
#endif


// Transform Unifrom Block.
layout(std140) uniform TransformBoneBlock
{
	// The Model/Object matrix.
	mat4 inModelMatrix;
	
	// The Model/Object normals transform matrix.
	mat4 inNormalMatrix;
	
	// Bones Transformation
	mat4 bones[RENDER_SKINNED_MAX_BONES];
};


#else

// Transform Unifrom Block.
layout(std140) uniform TransformBlock
{
	// The Model/Object matrix.
	mat4 inModelMatrix;
	
	// The Model/Object normals transform matrix.
	mat4 inNormalMatrix;
};


#endif




// --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- 




