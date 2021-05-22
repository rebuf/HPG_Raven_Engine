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

#include "ResourceManager/Resources/IResource.h"
#include "ResourceManager/Resources/Mesh.h"
#include "ResourceManager/Resources/Material.h"
#include "ResourceManager/Resources/MaterialShader.h"
#include "Render/RenderResource/Shader/RenderRscShader.h"
#include "Render/RenderResource/Primitives/RenderRscMesh.h"


#include "Utilities/Serialization.h"


#include <vector>



namespace Raven
{
	class Mesh;
	class Material;
	class RenderRscMeshInstance;



	// Represent a single layer of foliage on the terrain.
	class TerrainFoliageLayer
	{
		// Friend...
		friend class Terrain;

		// Signle instance in a layer.
		struct FoliageInstance
		{
			// Instance Transform.
			glm::mat4 transform;

			// Instance bounds center.
			glm::vec3 center;

			// Instance bounds center.
			float radius;

			// The bin this instance belongs to.
			int32_t binIndex;
		};

	public:
		// Construct.
		TerrainFoliageLayer()
			: isCastShadow(true)
		{
			SetClipDistance(-1.0f);
		}

		// Destruct.
		~TerrainFoliageLayer()
		{

		}

		// Setup new layer from mesh.
		inline void Setup(Ptr<Mesh> inMesh, const std::vector< Ptr<Material> >& inMaterials, int32_t iniSize)
		{
			mesh = inMesh;
			materials = inMaterials;
			auto& meshData = inMesh->GetMeshLOD(0);


			// Create resrouce for each section.
			for (int32_t i = 0; i < meshData.sections.size(); ++i)
			{
				// Mesh...
				RenderRscMesh* mesh = meshData.sections[i]->renderRscMesh.get();
				Ptr<RenderRscMeshInstance> newInstance( new RenderRscMeshInstance() );
				newInstance->Load(i == 0 ? nullptr : meshInstances[0]->GetInstanceTransform(), iniSize, mesh);
				meshInstances.emplace_back(newInstance);
			}

		}

		// Add new foliage instance.
		inline int32_t AddInstance(int32_t bin, const glm::mat4& transform)
		{
			MathUtils::BoundingBox bounds =  mesh->GetBounds().Transform(transform);

			FoliageInstance newInstance;
			newInstance.binIndex = bin;
			newInstance.transform = transform;
			newInstance.center = bounds.GetCenter();
			newInstance.radius = glm::length(bounds.GetExtent());
			instances.push_back(newInstance);

			return instances.size() - 1;
		}

		// Return all mesh instances.
		inline const std::vector< Ptr<RenderRscMeshInstance> >& GetMeshInstances() const { return meshInstances; }

		// Return all materail instances.
		inline const std::vector< Ptr<Material> >& GetMaterials() const { return materials; }

		// Return foliage instance.
		inline const FoliageInstance& GetInstance(int32_t i) const { return instances[i]; }

		// Return true if this layer cast shadow.
		inline bool IsCastShadow() const { return isCastShadow; }

		// Return the distance the layer clipped distance for it instances.
		inline float GetClipDistance() const { return clipDistance; }
		inline float GetClipDistanceForTesting() const { return clipDistanceTesting; }

		// Set clipping distance of this layer instances.
		inline void SetClipDistance(float distance)
		{
			clipDistance = distance;
			clipDistanceTesting = clipDistance > 0.0f ? (clipDistance * clipDistance) : 320000.0f;
		}

	private:
		// The Mesh.
		Ptr<Mesh> mesh;

		// The materials used to darw the instances.
		std::vector< Ptr<Material> > materials;

		// The Mesh instances used to draw all instances.
		std::vector< Ptr<RenderRscMeshInstance> > meshInstances;

		// List of all instances in this foliage layer.
		std::vector<FoliageInstance> instances;

		// The clip distance of instances inside this layer.
		float clipDistance;

		// its the same as clipDistance but transformed so we can do testing while clipping.
		float clipDistanceTesting;

		// Does the mesh cast shadows.
		bool isCastShadow;
	};


	// Represent a single foliage in a bin.
	struct TerrainBinFoliage
	{
		// Layer index this 
		int32_t layer;

		// The index of the foliage instance inside.
		int32_t index;
	};



	// a single bin in the terrain.
	struct TerrainBin
	{
		// Bin position offset in world space.
		glm::vec2 offset;

		// Texture Coordinate Scaling for each bin.
		glm::vec2 uvScale;

		// Bin Bounding Box, you can ignore the Y-Axis we only care about 2D-Bounds.
		MathUtils::BoundingBox bounds;

		// List of all foliage inside this bin.
		std::vector<TerrainBinFoliage> foliage;
	};


}
