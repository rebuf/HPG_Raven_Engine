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
#pragma once


#include "RenderRscPrimitive.h"
#include "ResourceManager/Resources/TerrainData.h"
#include "Render/RenderResource/RenderRscTexture.h"
#include "Math/BoundingBox.h"


#include "glm/vec2.hpp"



namespace Raven
{
	class GLVertexArray;
	class GLBuffer;
	class GLTexture;
	class UniformBuffer;
	class DynamicTexture;









	// RenderRscPrimitive:
	//		- Parent class for all resources that can be drawn.
	//		
	class RenderRscTerrain : public RenderRscPrimitive
	{
	public:
		// Construct.
		RenderRscTerrain();

		// Destruct..
		~RenderRscTerrain();

		// Load Terrain Render Data:
		// @param inHeightMap: a Height map texture.
		// @param res: the resolution of the terrain mesh.
		// @param inScale: the scale in meter for the terrain mesh.
		// @param inHeight: the min/max height of the terrain mesh.
		// @param inNumBins: num of bins in a single row of the terrain.
		void Load(DynamicTexture* inHeightMap, int32_t inNumBins,
			const glm::ivec2& inScale, const glm::vec2& inHeight);

		// Set bins data to be referenced by the terrain render resrouce.
		inline void SetBins(const std::vector<TerrainBin>* inBins) { bins = inBins; }

		// Return terrain bins.
		inline const std::vector<TerrainBin>* GetBins() const { return bins; }

		// Return mesh vertex array.
		inline const GLVertexArray* GetArray() const { return vertexArray; }

		// Return the number of vertices in the terrain mesh.
		inline uint32_t GetNumVerts() const { return numVerts; }
		inline uint32_t GetNumIndices() const { return numIndices; }

		// Return the terrain sacle.
		inline const glm::vec2& GetScale() const { return scale; }

		// Return the height factor of the terrain.
		inline const glm::vec2& GetHeight() const { return height; }

		// Return the height map texture.
		inline DynamicTexture* GetHeightMap() const { return heightMapTexture; }

		// Bind the bin uniform buffer.
		inline UniformBuffer* GetBinUB() { return binUniform.get(); }

		// Set/Get terrain foliage layers.
		inline const std::vector<TerrainFoliageLayer>* GetFoliageLayer() const { return foliageLayers; }
		inline void SetFoliageLayers(const std::vector<TerrainFoliageLayer>* layers) { foliageLayers = layers; }
		 
	private:
		// Generate Terrain Mesh for a single ben.
		void GenerateTerrainMesh();

	private:
		// Height Map.
		DynamicTexture* heightMapTexture;

		// OpenGL Buffer for Terrain Mesh Positions.
		GLBuffer* positionBuffer;

		//
		GLBuffer* indexBuffer;

		/** The OpenGL Vertex Array of the terrain mesh, defines mesh vertex input. */
		GLVertexArray* vertexArray;

		// Number of vertices in the terrain mesh.
		uint32_t numVerts;

		// Number of indicess in the terrain mesh.
		uint32_t numIndices;

		// Terrain Plane(X,Y) Scale.
		glm::vec2 scale;

		// Terrain Height Scale.
		glm::vec2 height;

		// Number of bins in single row of the terrain.
		int32_t numBins;

		// The scale of a single bin.
		glm::vec2 binScale;

		// Terrain Bin Data Uniform Buffers.
		Ptr<UniformBuffer> binUniform;

		// Terrains bins.
		const std::vector<TerrainBin>* bins;

		// Terrain foliage layers.
		const std::vector<TerrainFoliageLayer>* foliageLayers;
	};


}


