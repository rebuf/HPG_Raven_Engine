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
#include "Render/RenderResource/Primitives/RenderRscMesh.h"
#include "ResourceManager/Resources/Material.h"

#include "Utilities/Serialization.h"
#include "Math/BoundingBox.h"


#include "glm/glm.hpp"




namespace Raven
{
	class Skeleton;


	// SkinnedMeshSection:
	//			- 
	class SkinnedMeshSection
	{
	public:
		// Construct.
		SkinnedMeshSection()
		{

		}

		// Load Render Resrouces.
		inline void LoadRenderResource()
		{
			RAVEN_ASSERT(!renderRscMesh, "Resrouce already on GPU. use UpdateRenderRsc to update.");

			renderRscMesh = Ptr<RenderRscSkinnedMesh>(new RenderRscSkinnedMesh());
			renderRscMesh->Load(
				positions,
				normals,
				tangents,
				texCoords,
				indices,
				blendWeights,
				blendIndices
			);
		}

		// Normalize weights.
		inline void NormalizeBlendWeights()
		{
			RAVEN_ASSERT(positions.size() == blendWeights.size(), "size is not correct");

			for (int32_t i = 0; i < positions.size(); i++)
			{
				float sum = 0;
				for (int32_t j = 0; j < 4; j++)
				{
					sum += blendWeights[i][j];
				}
				const float invSum = sum > MathUtils::EPS ? 1.0f / sum : 0.0f;
				blendWeights[i] *= invSum;
			}
		}


		// Serialization Save.
		template<typename Archive>
		void save(Archive& archive) const
		{
			// Save Geometry.
			SaveVectorBinary(archive, positions);
			SaveVectorBinary(archive, normals);
			SaveVectorBinary(archive, tangents);
			SaveVectorBinary(archive, texCoords);
			SaveVectorBinary(archive, indices);
			SaveVectorBinary(archive, blendIndices);
			SaveVectorBinary(archive, blendWeights);
			archive(bounds);
			archive(defaultMaterial);
		}

		// Serialization Load.
		template<typename Archive>
		void load(Archive& archive)
		{
			// Save Load.
			LoadVectorBinary(archive, positions);
			LoadVectorBinary(archive, normals);
			LoadVectorBinary(archive, tangents);
			LoadVectorBinary(archive, texCoords);
			LoadVectorBinary(archive, indices);
			LoadVectorBinary(archive, blendIndices);
			LoadVectorBinary(archive, blendWeights);
			archive(bounds);
			archive(defaultMaterial);


			// If Valid try to loaded it.
			if (defaultMaterial.IsValid())
			{
				defaultMaterial.FindOrLoad<Material>();
			}
		}


		// Position Buffer.
		std::vector<glm::vec3> positions;

		// Normals Buffer.
		std::vector<glm::vec3> normals;

		// Tangents Buffer.
		std::vector<glm::vec3> tangents;

		// Texture Coordinate Buffer.
		std::vector<glm::vec2> texCoords;

		// indices Buffer.
		std::vector<uint32_t> indices;

		// Skinned mesh blend indices (max 4 per bone).
		std::vector<glm::ivec4> blendIndices;

		// Skinned mesh index buffer (max 4 per bone).
		std::vector<glm::vec4> blendWeights;

		// The bounding box of this mesh section.
		MathUtils::BoundingBox bounds;

		// Render Resrouce Data.
		Ptr<RenderRscSkinnedMesh> renderRscMesh;

		// Default material for this section.
		ResourceRef defaultMaterial;
	};


	// SkinnedMesh:
	//  - resrouce for skinned meshes.
	//
	class SkinnedMesh : public IResource
	{
	public:
		// Construct.
		SkinnedMesh()
			: IResource()
		{
			type = SkinnedMesh::StaticGetType();
			hasRenderResources = true;
		}

		virtual ~SkinnedMesh()
		{

		}

		// return the resource type
		inline static EResourceType StaticGetType() noexcept { return EResourceType::RT_SkinnedMesh; }

		// Load Render Resrouces.
		inline virtual void LoadRenderResource() override
		{
			RAVEN_ASSERT(!isOnGPU, "Resrouce already on GPU. use UpdateRenderRsc to update.");
			isOnGPU = true;

			for (auto& section : sections)
			{
				// Invalid Section?
				if (!section)
					continue;

				section->LoadRenderResource();
			}
		}

		// Update Render Resrouces.
		inline virtual void UpdateRenderResource() override
		{
			RAVEN_ASSERT(isOnGPU, "Resrouce not on GPU. use LoadRenderRsc to load it first.");
			// TODO: update.
		}

		// Set a new mesh section.
		inline void SetMeshSection(uint32_t index, Ptr<SkinnedMeshSection> section)
		{
			if (sections.size() < index + 1)
				sections.resize(index + 1);

			sections[index] = section;

			UpdateBounds();
		}

		// Add new mesh section at the end of the sections list.
		inline void AddMeshSection(Ptr<SkinnedMeshSection> section)
		{
			sections.push_back(section);

			UpdateBounds();
		}

		// Return mesh section.
		inline SkinnedMeshSection* GetMeshSection(uint32_t index) { return sections[index].get(); }

		// Return mesh section list.
		inline auto& GetMeshSections() { return sections; }
		inline const auto& GetMeshSections() const { return sections; }

		// Return the number of sections in a mesh.
		inline uint32_t GetNumSections() const { return sections.size(); }

		// Return the bounds of this mesh.
		inline MathUtils::BoundingBox GetBounds() const { return bounds; }

		// Return the skeleton of this skinned mesh.
		inline const Ptr<Skeleton>& GetSkeleton() const { return skeleton; }

		// Set the skeleton of this skinned mesh.
		inline void SetSkeleton(Ptr<Skeleton> inSkeleton) { skeleton = inSkeleton; }


		// Normalize weights for all skinned mesh sections.
		inline void NormalizeBlendWeights()
		{
			for (auto& section : sections)
			{
				// Invalid Section?
				if (!section)
					continue;

				section->NormalizeBlendWeights();
			}
		}

		// Serialization Save.
		template<typename Archive>
		void save(Archive& archive) const
		{
			archive(cereal::base_class<IResource>(this));

			uint32_t numSections = (uint32_t)sections.size();
			archive(numSections);

			// Only archive valid mesh sections.
			for (uint32_t i = 0; i < numSections; ++i)
			{
				bool isValid = sections[i] != nullptr;
				archive(isValid);

				if (isValid)
					archive(*sections[i]);
			}

			// Save Resrouce Reference -> Skeleton.
			ResourceRef::Save(archive, skeleton.get());
		}

		// Serialization Load.
		template<typename Archive>
		void load(Archive& archive)
		{
			archive(cereal::base_class<IResource>(this));

			uint32_t numSections;
			archive(numSections);
			sections.resize(numSections);

			// Only archive valid mesh sections.
			for (uint32_t i = 0; i < numSections; ++i)
			{
				bool isValid = false;
				archive(isValid);
				if (isValid)
				{
					sections[i] = Ptr<SkinnedMeshSection>(new SkinnedMeshSection());
					archive(*sections[i]);
				}
			}

			// Load Resrouce Reference -> Skeleton.
			skeleton = ResourceRef::Load(archive).FindOrLoad<Skeleton>();

			// Update bounds after loading.
			UpdateBounds();
		}

	private:
		// Recompute/Update the bounding box the model.
		inline void UpdateBounds()
		{
			bounds.Reset();

			for (const auto& ms : sections)
			{
				bounds.Add(ms->bounds);
			}
		}

	private:
		// List of all mesh sections.
		std::vector< Ptr<SkinnedMeshSection> > sections;

		// The boudning box of all mesh sections.
		MathUtils::BoundingBox bounds;

		// The skeleton of this skinned mesh.
		Ptr<Skeleton> skeleton;
	};

}
