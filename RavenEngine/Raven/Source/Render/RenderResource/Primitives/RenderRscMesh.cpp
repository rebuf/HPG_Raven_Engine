/*
 * Developed by Raven Group at the University  of Leeds
 * Copyright (C) 2021 Ammar Herzallah, Ben Husle, Thomas Moreno Cooper, Sulagna Sinha & Tian Zeng
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * THIS PROGRAM IS DISTRIBUTED IN THE HOPE THAT IT WILL BE USEFUL,
 * BUT WITHOUT ANY WARRANTY; WITHOUT EVEN THE IMPLIED WARRANTY OF
 * MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  SEE THE
 * GNU GENERAL PUBLIC LICENSE FOR MORE DETAILS.
 */
#include "RenderRscMesh.h"



#include "Render/OpenGL/GLVertexArray.h"
#include "Render/OpenGL/GLBuffer.h"



namespace Raven {




RenderRscMesh::RenderRscMesh()
	: vxarray(nullptr)
	, positionBuffer(nullptr)
	, normalBuffer(nullptr)
	, texCoordBuffer(nullptr)
	, indexBuffer(nullptr)
{

}


RenderRscMesh::~RenderRscMesh()
{
	delete vxarray;
	delete positionBuffer;
	delete normalBuffer;
	delete texCoordBuffer;
	delete indexBuffer;
}


void RenderRscMesh::Load(const std::vector<glm::vec3>& positions,
	const std::vector<glm::vec3>& normals, const std::vector<glm::vec3>& tangents,
	const std::vector<glm::vec2>& texCoord, const std::vector<unsigned int>& indices)
{
	// Create/Update Position Buffer.
	positionBuffer = GLBuffer::Create(
		EGLBufferType::Array, 
		(int)(positions.size() * sizeof(glm::vec3)),
		positions.data(),
		EGLBufferUsage::StaticDraw
	);

	// Create/Update Normal Buffer.
	normalBuffer = GLBuffer::Create(
		EGLBufferType::Array,
		(int)(normals.size() * sizeof(glm::vec3)),
		normals.data(),
		EGLBufferUsage::StaticDraw
	);

	// Create/Update Tangent Buffer.
	tangentBuffer = GLBuffer::Create(
		EGLBufferType::Array,
		(int)(tangents.size() * sizeof(glm::vec3)),
		tangents.data(),
		EGLBufferUsage::StaticDraw
	);

	// Create/Update TexCoordinate Buffer.
	texCoordBuffer = GLBuffer::Create(
		EGLBufferType::Array,
		(int)(texCoord.size() * sizeof(glm::vec2)),
		texCoord.data(),
		EGLBufferUsage::StaticDraw
	);

	// Create/Update Index Buffer.
	numIndices = indices.size();
	indexBuffer = GLBuffer::Create(
		EGLBufferType::Element,
		(int)(indices.size() * sizeof(unsigned int)),
		indices.data(),
		EGLBufferUsage::StaticDraw
	);



	// Build Vertex Input Description...
	std::vector<GLVABuildAttribData> attributes{
			// Attribute 0 - Positions
			GLVABuildAttribData(
				positionBuffer,    // Buffer
				0,                 // Index
				3,                 // Type-Size
				EGLTypes::Float,   // Type
				sizeof(glm::vec3), // Stride
				0                  // offset
			),

			// Attribute 1 - Normals
			GLVABuildAttribData(
				normalBuffer,      // Buffer
				1,                 // Index
				3,                 // Type-Size
				EGLTypes::Float,   // Type
				sizeof(glm::vec3), // Stride
				0                  // offset
			),

			// Attribute 2 - Tangents
			GLVABuildAttribData(
				tangentBuffer,     // Buffer
				2,                 // Index
				3,                 // Type-Size
				EGLTypes::Float,   // Type
				sizeof(glm::vec3), // Stride
				0                  // offset
			),

			// Attribute 3 - TexCoords
			GLVABuildAttribData(
				texCoordBuffer,		 // Buffer
				3,                 // Index
				2,                 // Type-Size
				EGLTypes::Float,   // Type
				sizeof(glm::vec2), // Stride
				0                  // offset
			)
	};


	vxarray = GLVertexArray::Create();
	vxarray->Build(attributes, indexBuffer);
}






// -- - -- -- - -- -- - -- -- - -- -- - -- -- - -- -- - -- -- - -- -- - -- -- - -- -- - -- 
// -- - -- -- - -- -- - -- -- - -- -- - -- -- - -- -- - -- -- - -- -- - -- -- - -- -- - -- 







RenderRscSkinnedMesh::RenderRscSkinnedMesh()
	: vxarray(nullptr)
	, positionBuffer(nullptr)
	, normalBuffer(nullptr)
	, texCoordBuffer(nullptr)
	, indexBuffer(nullptr)
	, weightBuffer(nullptr)
	, boneIndicesBuffer(nullptr)
{

}


RenderRscSkinnedMesh::~RenderRscSkinnedMesh()
{
	delete vxarray;
	delete positionBuffer;
	delete normalBuffer;
	delete texCoordBuffer;
	delete indexBuffer;
	delete weightBuffer;
	delete boneIndicesBuffer;
}


void RenderRscSkinnedMesh::Load(const std::vector<glm::vec3>& positions,
	const std::vector<glm::vec3>& normals, const std::vector<glm::vec3>& tangents,
	const std::vector<glm::vec2>& texCoord, const std::vector<unsigned int>& indices,
	const std::vector<glm::vec4>& weight, const std::vector<glm::ivec4>& blendIndices)
{
	// Create/Update Position Buffer.
	positionBuffer = GLBuffer::Create(
		EGLBufferType::Array,
		(int)(positions.size() * sizeof(glm::vec3)),
		positions.data(),
		EGLBufferUsage::StaticDraw
	);

	// Create/Update Normal Buffer.
	normalBuffer = GLBuffer::Create(
		EGLBufferType::Array,
		(int)(normals.size() * sizeof(glm::vec3)),
		normals.data(),
		EGLBufferUsage::StaticDraw
	);

	// Create/Update Tangent Buffer.
	tangentBuffer = GLBuffer::Create(
		EGLBufferType::Array,
		(int)(tangents.size() * sizeof(glm::vec3)),
		tangents.data(),
		EGLBufferUsage::StaticDraw
	);

	// Create/Update TexCoordinate Buffer.
	texCoordBuffer = GLBuffer::Create(
		EGLBufferType::Array,
		(int)(texCoord.size() * sizeof(glm::vec2)),
		texCoord.data(),
		EGLBufferUsage::StaticDraw
	);

	// Create/Update Weight Buffer.
	weightBuffer = GLBuffer::Create(
		EGLBufferType::Array,
		(int)(weight.size() * sizeof(glm::vec4)),
		weight.data(),
		EGLBufferUsage::StaticDraw
	);

	// Create/Update Bone Indices Buffer.
	boneIndicesBuffer = GLBuffer::Create(
		EGLBufferType::Array,
		(int)(blendIndices.size() * sizeof(glm::ivec4)),
		blendIndices.data(),
		EGLBufferUsage::StaticDraw
	);



	// Create/Update Index Buffer.
	numIndices = indices.size();
	indexBuffer = GLBuffer::Create(
		EGLBufferType::Element,
		(int)(indices.size() * sizeof(unsigned int)),
		indices.data(),
		EGLBufferUsage::StaticDraw
	);

	// Build Vertex Input Description...
	std::vector<GLVABuildAttribData> attributes{
		// Attribute 0 - Positions
		{
			positionBuffer,    // Buffer
			0,                 // Index
			3,                 // Type-Size
			EGLTypes::Float,   // Type
			sizeof(glm::vec3), // Stride
			0                  // offset
		},

		// Attribute 1 - Normals
		{
			normalBuffer,      // Buffer
			1,                 // Index
			3,                 // Type-Size
			EGLTypes::Float,   // Type
			sizeof(glm::vec3), // Stride
			0                  // offset
		},

		// Attribute 2 - Tangent
		{
			tangentBuffer,      // Buffer
			2,                 // Index
			3,                 // Type-Size
			EGLTypes::Float,   // Type
			sizeof(glm::vec3), // Stride
			0                  // offset
		},

		// Attribute 3 - TexCoords
		{
			texCoordBuffer,		 // Buffer
			3,                 // Index
			2,                 // Type-Size
			EGLTypes::Float,   // Type
			sizeof(glm::vec2), // Stride
			0                  // offset
		},

		// Attribute 4 - Weights
		{
			weightBuffer,		 // Buffer
			4,                 // Index
			4,                 // Type-Size
			EGLTypes::Float,   // Type
			sizeof(glm::vec4), // Stride
			0                  // offset
		}
		,

		// Attribute 5 - Bone Indices
		{
			boneIndicesBuffer,   // Buffer
			5,                   // Index
			4,                   // Type-Size
			EGLTypes::Int,       // Type
			sizeof(glm::ivec4),  // Stride
			0                    // offset
		}
	};


	vxarray = GLVertexArray::Create();
	vxarray->Build(attributes, indexBuffer);
}



RenderRscMeshInstance::RenderRscMeshInstance()
	: vxarray(nullptr)
	, mesh(nullptr)
	, instanceTransform(nullptr)
	, instanceBufferOwner(false)
{

}


RenderRscMeshInstance::~RenderRscMeshInstance()
{
	if (instanceBufferOwner)
	{
		delete instanceTransform;
	}

	delete vxarray;
}


void RenderRscMeshInstance::Load(GLBuffer* instanceBuffer, int32_t iniSize, RenderRscMesh* rscMesh)
{
	mesh = rscMesh;

	// Create Transform Buffers.
	if (instanceBuffer != nullptr)
	{
		instanceTransform = instanceBuffer;
	}
	else
	{
		instanceTransform = GLBuffer::Create(
			EGLBufferType::Array,
			(int)(iniSize * sizeof(glm::mat4)),
			nullptr,
			EGLBufferUsage::DynamicDraw
		);

		instanceBufferOwner = true;
	}



	// Build Vertex Input Description...
	std::vector<GLVABuildAttribData> attributes{
		// Attribute 0 - Positions
		GLVABuildAttribData(
			mesh->positionBuffer,   // Buffer
			0,                      // Index
			3,                      // Type-Size
			EGLTypes::Float,        // Type
			sizeof(glm::vec3),      // Stride
			0                       // offset
		),

		// Attribute 1 - Normals
		GLVABuildAttribData(
			mesh->normalBuffer,   // Buffer
			1,                    // Index
			3,                    // Type-Size
			EGLTypes::Float,      // Type
			sizeof(glm::vec3),    // Stride
			0                     // offset
		),

		// Attribute 2 - Tangents
		GLVABuildAttribData(
			mesh->tangentBuffer,  // Buffer
			2,                    // Index
			3,                    // Type-Size
			EGLTypes::Float,      // Type
			sizeof(glm::vec3),    // Stride
			0                     // offset
		),

		// Attribute 3 - TexCoords
		GLVABuildAttribData(
			mesh->texCoordBuffer,	 // Buffer
			3,                     // Index
			2,                     // Type-Size
			EGLTypes::Float,       // Type
			sizeof(glm::vec2),     // Stride
			0                      // offset
		),

		// -- -- -- -- -- -- -- -- -- -- --
		// Attribute 4,5,6,7 - Instance Transform
		GLVABuildAttribData(
			instanceTransform,	          // Buffer
			4,                            // Index
			4,                            // Type-Size
			EGLTypes::Float,              // Type
			4 * sizeof(glm::vec4),        // Stride
			0 * sizeof(glm::vec4),        // offset
			1                             // Instance
		),
		GLVABuildAttribData(
			instanceTransform,	          // Buffer
			5,                            // Index
			4,                            // Type-Size
			EGLTypes::Float,              // Type
			4 * sizeof(glm::vec4),        // Stride
			1 * sizeof(glm::vec4) ,       // offset
			1                             // Instance
		),
		GLVABuildAttribData(
			instanceTransform,	          // Buffer
			6,                            // Index
			4,                            // Type-Size
			EGLTypes::Float,              // Type
			4 * sizeof(glm::vec4),        // Stride
			2 * sizeof(glm::vec4),        // offset
			1                             // Instance
		),
		GLVABuildAttribData(
			instanceTransform,	          // Buffer
			7,                            // Index
			4,                            // Type-Size
			EGLTypes::Float,              // Type
			4 * sizeof(glm::vec4),        // Stride
			3 * sizeof(glm::vec4),        // offset
			1                             // Instance
		)
	};



	vxarray = GLVertexArray::Create();
	vxarray->Build(attributes, mesh->indexBuffer);
}


void RenderRscMeshInstance::UpdateTransforms(const std::vector<glm::mat4>& transforms)
{
	int32_t newSize = transforms.size() * sizeof(glm::mat4);

	if (newSize <= instanceTransform->GetSize())
	{
		instanceTransform->UpdateSubData(newSize, 0, &transforms[0]);
	}
	else
	{
		instanceTransform->UpdateData(newSize, &transforms[0]);
	}

}



} // End of namespace Raven.
