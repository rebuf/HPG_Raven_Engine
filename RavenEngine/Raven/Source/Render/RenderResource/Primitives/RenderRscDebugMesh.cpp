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
#include "RenderRscDebugMesh.h"



#include "Render/OpenGL/GLVertexArray.h"
#include "Render/OpenGL/GLBuffer.h"



namespace Raven {




RenderRscDebugMesh::RenderRscDebugMesh()
	: vertexArray(nullptr)
	, vertexBuffer(nullptr)
	, indexBuffer(nullptr)
	, numIndices(0)
{

}


RenderRscDebugMesh::~RenderRscDebugMesh()
{
	if (vertexArray != nullptr)
		delete vertexArray;

	if (vertexBuffer != nullptr)
		delete vertexBuffer;

	if (indexBuffer != nullptr)
		delete indexBuffer;
}


void RenderRscDebugMesh::Load(const std::vector<glm::vec3>& verts, const std::vector<uint32_t>& indices)
{
	vertexBuffer = GLBuffer::Create(
		EGLBufferType::Array, 
		(int)(verts.size() * sizeof(glm::vec3)),
		verts.data(),
		EGLBufferUsage::StaticDraw
	);


	numIndices = indices.size();
	indexBuffer = GLBuffer::Create(
		EGLBufferType::Element,
		(int)(indices.size() * sizeof(uint32_t)),
		indices.data(),
		EGLBufferUsage::StaticDraw
	);



	std::vector<GLVABuildAttribData> attributes{
			// Attribute 0
			{
				vertexBuffer,      // Buffers
				0,                 // Index
				3,                 // Type-Size
				EGLTypes::Float,   // Type
				sizeof(glm::vec3), // Stride
				0                  // offset
			}
	};

	vertexArray = GLVertexArray::Create();
	vertexArray->Build(attributes, indexBuffer);
}


void RenderRscDebugMesh::CreateBox()
{
	// Box Vertices...
	std::vector<glm::vec3> verts = {
		glm::vec3(-0.5f,-0.5f,-0.5f),
		glm::vec3( 0.5f,-0.5f,-0.5f),
		glm::vec3( 0.5f,-0.5f, 0.5f),
		glm::vec3(-0.5f,-0.5f, 0.5f),

		glm::vec3(-0.5f, 0.5f,-0.5f),
		glm::vec3( 0.5f, 0.5f,-0.5f),
		glm::vec3( 0.5f, 0.5f, 0.5f),
		glm::vec3(-0.5f, 0.5f, 0.5f)
	};


	// Box Indices...
	std::vector<uint32_t> indices = {
		0, 1, 2, // Face 0.
		0, 2, 3,
		4, 6, 5, // Face 1.
		4, 7, 6,
		0, 7, 4, // Face 2.
		0, 3, 7,
		1, 6, 2, // Face 3.
		1, 5, 6,
		0, 5, 1, // Face 4.
		0, 4, 5,
		3, 2, 6, // Face 5.
		3, 6, 7
	};

	// Load Resoruce...
	Load(verts, indices);
}



} // End of namespace Raven.
