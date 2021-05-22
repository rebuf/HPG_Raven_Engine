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

//////////////////////////////////////////////////////////////////////////////
// This file is part of the Raven Game Engine			                    //
//////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Utilities/Core.h"
#include "ResourceManager/Resources/IResource.h" 
#include "ResourceManager/Resources/Material.h" 
#include "ResourceManager/Resources/Mesh.h" 
#include "ResourceManager/Resources/TerrainData.h"
#include "ResourceManager/Resources/DynamicTexture.h"
#include "Render/RenderResource/Primitives/RenderRscTerrain.h"

#include "ProceduralGenerator/HeightMap.h"



#include "glm/glm.hpp"



namespace Raven
{



	// Terrain:
	//   - 
	//
	class Terrain : public IResource
	{
		NOCOPYABLE(Terrain);

	public:
		Terrain() 
			: IResource()
		{
			type = Terrain::StaticGetType();
			hasRenderResources = true;
		}
		

		virtual ~Terrain()
		{
			renderRsc.reset();
			heightMap.reset();
		}

		// Return the resource type
		inline static EResourceType StaticGetType() { return EResourceType::RT_Terrain; }

		// Load the render resource.
		virtual void LoadRenderResource() override
		{
			RAVEN_ASSERT(!isOnGPU, "Resrouce already on GPU. use UpdateRenderRsc to update.");
			isOnGPU = true;

			// 
			renderRsc = Ptr<RenderRscTerrain>( new RenderRscTerrain() );
			renderRsc->Load(heightMap->GetHeightmapTexture(), bins.size(), scale, height);
			renderRsc->SetBins(&bins);
			renderRsc->SetFoliageLayers(&foliageLayers);
		}
		 
		// Return Terrain render resource.
		inline RenderRscTerrain* GetRenderRsc() { return renderRsc.get(); }
		inline const RenderRscTerrain* GetRenderRsc() const { return renderRsc.get(); }

		// Load Terrain Render Data:
		// @param inHeightMap: a Height map texture.
		// @param inScale: the scale in meter for the terrain mesh.
		// @param inNumBins: num of bins in a single row of the terrain.
		// @param inHeight: the min/max height of the terrain mesh.
		inline void SetTerrainData(Ptr<HeightMap> inHeightMap, const glm::vec2& inScale,
			int32_t inNumBinsPerRow, const glm::vec2& inHeight)
		{
			heightMap = inHeightMap;
			scale = inScale;
			height = inHeight;

			// Generate terrain texture if not already.
			if (!heightMap->GetHeightmapTexture())
				heightMap->GenerateTexture();

			GenerateBins(inNumBinsPerRow);
		}


		// Set/Get Material.
		inline void SetMaterial(Ptr<Material> mat) { material = mat; }
		inline Ptr<Material> GetMaterial() const { return material; }


		// Retrun the terrain height map.
		inline const HeightMap* GetHeightMap() const { return heightMap.get(); }

		// Return the terrain height min/max.
		inline const glm::vec2& GetHeight() const { return height; }

		// Create a new foliage layer.
		inline size_t NewLayer(const std::string& name, Ptr<Mesh> mesh, const std::vector< Ptr<Material> >& materails)
		{
			RAVEN_ASSERT(mesh.get(), "");

			TerrainFoliageLayer& newlayer = foliageLayers.emplace_back( TerrainFoliageLayer() );
			newlayer.Setup(mesh, materails, 100);
			
			return foliageLayers.size() - 1;
		}

		// Add a new foliage instance.
		inline void AddFoliageInstance(int32_t binIndex, int32_t layer, const glm::mat4& transform)
		{
			TerrainBinFoliage foliage;
			foliage.index = foliageLayers[layer].AddInstance(binIndex, transform);
			foliage.layer = layer;
			bins[binIndex].foliage.push_back(foliage);
		}


		//
		inline const auto& GetBins()
		{
			return bins;
		}

		//
		inline auto& GetFoliageLayer(size_t i)
		{
			return foliageLayers[i];
		}

	private:
		// Generate Terrain Bins.
		void GenerateBins(int32_t numBinsPerRow)
		{
			RAVEN_ASSERT(numBinsPerRow >= 2, ".");

			binSize = scale * (1.0f / (float)numBinsPerRow);
			glm::vec2 uvScale = glm::vec2(binSize.x / scale.x, binSize.y / scale.y);

			// Gnerate NxN terrain bins.
			bins.resize(numBinsPerRow * numBinsPerRow);

			// The bin size.
			for (uint32_t i = 0; i < bins.size(); ++i)
			{
				float fx = (float)(i % numBinsPerRow);
				float fy = (float)(i / numBinsPerRow);


				bins[i].offset = glm::vec2(fx, fy) * binSize;
				bins[i].uvScale = uvScale;

				bins[i].bounds = MathUtils::BoundingBox(
					glm::vec3(bins[i].offset.x,             height.x, bins[i].offset.y),
					glm::vec3(bins[i].offset.x + binSize.x, height.y, bins[i].offset.y + binSize.y));
			}

			bounds = MathUtils::BoundingBox(glm::vec3(0.0f, 0.0f, height.x), glm::vec3(scale.x, scale.y, height.y));
		}



	private:
		// The height map data.
		Ptr<HeightMap> heightMap;

		// Terrain Render Resrouce.
		Ptr<RenderRscTerrain> renderRsc;

		// Terrain scale.
		glm::vec2 scale;

		// Min(x)/Max(y) height of the terrain.
		glm::vec2 height;

		// Terrain Bins.
		std::vector<TerrainBin> bins;

		// Single Bine Size.
		glm::vec2 binSize;

		// Terrain Resolution.
		int32_t res;

		// The entire terrain bounds.
		MathUtils::BoundingBox bounds;

		// List of all foliage layers inside the terrain.
		std::vector<TerrainFoliageLayer> foliageLayers;

		// The terrain material used to render this terrain.
		Ptr<Material> material;
	};
}
