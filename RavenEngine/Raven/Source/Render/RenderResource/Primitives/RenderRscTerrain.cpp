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
#include "RenderRscTerrain.h"

#include "ResourceManager/Resources/DynamicTexture.h"


#include "Render/RenderResource/Shader/UniformBuffer.h"
#include "Render/OpenGL/GLTexture.h"
#include "Render/OpenGL/GLVertexArray.h"
#include "Render/OpenGL/GLBuffer.h"

#include "glm/gtx/common.hpp"
#include "glm/vec3.hpp"



namespace Raven {



RenderRscTerrain::RenderRscTerrain()
	: vertexArray(nullptr)
	, numVerts(0)
	, numBins(0)
	, numIndices(0)
	, foliageLayers(nullptr)
	, bins(nullptr)
{

}


RenderRscTerrain::~RenderRscTerrain()
{
	delete vertexArray;
	delete positionBuffer;

}


void RenderRscTerrain::Load(DynamicTexture* inHeightMap, int32_t inNumBins,
	const glm::ivec2& inScale, const glm::vec2& inHeight)
{
	heightMapTexture = inHeightMap;
	scale = inScale;
	height = inHeight;
	numBins = inNumBins;

	GenerateTerrainMesh();

	binUniform = Ptr<UniformBuffer>(UniformBuffer::Create(RenderShaderInput::TerrainBinBlock, false));
}


void RenderRscTerrain::GenerateTerrainMesh()
{
	RAVEN_ASSERT(
		   ( numBins     >=   4         )
		&& ( scale.x     >   0.0f      )
		&& ( scale.y     >   0.0f      )
		&& ( height.y    >   height.x  )
		, "Invalid Terrain.");


	binScale = scale / (float)sqrt(numBins);
	int32_t res = (int32_t)(glm::length(binScale) / 50.0f);
	res = pow(2, floor(log2(res)));

	std::vector<glm::vec3> terrainVerts;
	std::vector<uint32_t> terrainIndices;

	int vcount = res;
	terrainVerts.reserve((vcount + 1) * (vcount + 1));
	terrainIndices.reserve(vcount * vcount * 4);
	float vof = (1.0f / (float)(vcount));

	for (int y = 0; y < vcount + 1; ++y)
	{
		float fy = (float)y / (float)(vcount);

		for (int x = 0; x < vcount + 1; ++x)
		{
			float fx = (float)x / (float)(vcount);

			float v = binScale.x * (fx);
			float y = binScale.y * (fy);

			terrainVerts.push_back(glm::vec3(v, 0.0f, y));
		}
	}


	for (int y = 0; y < vcount; ++y)
	{
		for (int x = 0; x < vcount; ++x)
		{
			terrainIndices.push_back( x      +  y    * (vcount + 1));
			terrainIndices.push_back((x+1)   +  y    * (vcount + 1));
			terrainIndices.push_back( x      + (y+1) * (vcount + 1));
			terrainIndices.push_back((x+1)   + (y+1) * (vcount + 1));
		}
	}

	numVerts = static_cast<uint32_t>(terrainVerts.size());
	positionBuffer = GLBuffer::Create(
		EGLBufferType::Array,
		(int)(terrainVerts.size() * sizeof(glm::vec3)),
		terrainVerts.data(),
		EGLBufferUsage::StaticDraw
	);
	

	numIndices = static_cast<uint32_t>(terrainIndices.size());
	indexBuffer = GLBuffer::Create(
		EGLBufferType::Element,
		(int)(terrainIndices.size() * sizeof(uint32_t)),
		terrainIndices.data(),
		EGLBufferUsage::StaticDraw
	);


	std::vector<GLVABuildAttribData> attributes{
		// Attribute 0 - Position
		GLVABuildAttribData(
			positionBuffer,    // Buffers
			0,                 // Index
			3,                 // Type-Size
			EGLTypes::Float,   // Type
			sizeof(glm::vec3), // Stride
			0                  // offset
		)
	};


	vertexArray = GLVertexArray::Create();
	vertexArray->Build(attributes, indexBuffer);
}




} // End of namespace Raven.
