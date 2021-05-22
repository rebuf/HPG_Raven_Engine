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
#include "RenderSphere.h"


#include "Render/OpenGL/GLVertexArray.h"
#include "Render/OpenGL/GLBuffer.h"
#include "Render/OpenGL/GLShader.h"


#include "GL/glew.h"
#include "glm/common.hpp"
#include "glm/gtc/constants.hpp"



namespace Raven {



RenderSphere::RenderSphere()
	: vxArray(nullptr)
	, vxBuffer(nullptr)
	, idxBuffer(nullptr)
	, numIndices(0)
{

}


RenderSphere::~RenderSphere()
{
	delete vxArray;
	delete vxBuffer;
	delete idxBuffer;
}


void RenderSphere::MakeSphere(std::vector<glm::vec3>& outVerts, std::vector<uint32_t>& outIndices)
{
	int32_t xSegments = 16;
	int32_t ySegments = 16;

	float sectorCount = static_cast<float>(xSegments);
	float stackCount = static_cast<float>(ySegments);
	float sectorStep = 2 * glm::pi<float>() / sectorCount;
	float stackStep = glm::pi<float>() / stackCount;
	float radius = 1.0f;


	for (int i = 0; i <= stackCount; ++i)
	{
		float stackAngle = glm::pi<float>() / 2 - i * stackStep;        // starting from pi/2 to -pi/2
		float xy = radius * cos(stackAngle);             // r * cos(u)
		float z = radius * sin(stackAngle);              // r * sin(u)


		for (int32_t j = 0; j <= sectorCount; ++j)
		{
			float sectorAngle = j * sectorStep;           // starting from 0 to 2pi

			// vertex position (x, y, z)
			float x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
			float y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)

			// vertex tex coords (s, t) range between [0, 1]
			float s = static_cast<float>(j / sectorCount);
			float t = static_cast<float>(i / stackCount);

			outVerts.emplace_back(glm::vec3(x, y, z));
		}
	}


	uint32_t k1, k2;
	for (uint32_t i = 0; i < stackCount; ++i)
	{
		k1 = i * (static_cast<uint32_t>(sectorCount) + 1U);     // beginning of current stack
		k2 = k1 + static_cast<uint32_t>(sectorCount) + 1U;      // beginning of next stack

		for (uint32_t j = 0; j < sectorCount; ++j, ++k1, ++k2)
		{
			// 2 triangles per sector excluding first and last stacks
			// k1 => k2 => k1+1
			if (i != 0)
			{
				outIndices.emplace_back(k1);
				outIndices.emplace_back(k2);
				outIndices.emplace_back(k1 + 1);
			}

			// k1+1 => k2 => k2+1
			if (i != (stackCount - 1))
			{
				outIndices.emplace_back(k1 + 1);
				outIndices.emplace_back(k2);
				outIndices.emplace_back(k2 + 1);
			}
		}
	}

}


RenderSphere* RenderSphere::Create()
{
	RenderSphere* scr = new RenderSphere();

	std::vector<glm::vec3> verts;
	std::vector<uint32_t> indices;
	MakeSphere(verts, indices);

	scr->vxBuffer = GLBuffer::Create(
		EGLBufferType::Array,
		(int)(verts.size() * sizeof(glm::vec3)),
		verts.data(),
		EGLBufferUsage::StaticDraw
	);


	// Create/Update Index Buffer.
	scr->numIndices = indices.size();
	scr->idxBuffer = GLBuffer::Create(
		EGLBufferType::Element,
		(int)(indices.size() * sizeof(uint32_t)),
		indices.data(),
		EGLBufferUsage::StaticDraw
	);


	std::vector<GLVABuildAttribData> attributes{
		// Attribute 0 - Position
		GLVABuildAttribData(
			scr->vxBuffer,         // Buffers
			0,                     // Index
			3,                     // Type-Size
			EGLTypes::Float,       // Type
			sizeof(glm::vec3),     // Stride
			0                      // offset
		)
	};


	scr->vxArray = GLVertexArray::Create();
	scr->vxArray->Build(attributes, scr->idxBuffer);

	return scr;
}


void RenderSphere::SetView(const glm::mat4& mtx)
{
	view = mtx;
}


void RenderSphere::SetProj(const glm::mat4& mtx)
{
	proj = mtx;
}


void RenderSphere::Draw(GLShader* shader)
{
	glm::mat4 viewProj = proj * view;
	shader->SetUniform("inViewProjMatrix", viewProj);

	vxArray->Bind();
	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, nullptr);
}


void RenderSphere::DrawSky(GLShader* shader)
{
	vxArray->Bind();
	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, nullptr);
}




} // End of namespace Raven.
