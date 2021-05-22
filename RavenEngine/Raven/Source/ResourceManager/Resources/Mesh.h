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

#include "glm/glm.hpp"

#include "Math/BoundingBox.h"

#include "ResourceManager/Resources/IResource.h"
#include "ResourceManager/Resources/Material.h"
#include "Render/RenderResource/Primitives/RenderRscMesh.h"






namespace Raven
{
	// MeshSection:
	//    - contain the data of a single section in a mesh.
	class MeshSection
	{
	public:
		MeshSection()
		{

		}


		// Load Render Resrouces.
		inline void LoadRenderResource()
		{
			RAVEN_ASSERT(!renderRscMesh, "Resrouce already on GPU. use UpdateRenderRsc to update.");

			renderRscMesh = Ptr<RenderRscMesh>(new RenderRscMesh());
			renderRscMesh->Load(
				positions,
				normals,
				tangents,
				texCoords,
				indices
			);
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

		// The bounding box of this mesh section.
		MathUtils::BoundingBox bounds;

		// Render Resrouce Data.
		Ptr<RenderRscMesh> renderRscMesh;

		// Default material for this section.
		ResourceRef defaultMaterial;
	};


	// MeshLOD:
	//   - single level of detail in the mesh's list of LODs.
	//
	class MeshLOD
	{
	public:
		// The LOD Mesh Sections.
		std::vector< Ptr<MeshSection> > sections;

		// The distance this LOD will be display at.
		float distance;

		// Construct.
		MeshLOD()
			: distance(32000.0f)
		{

		}

		// Serialization Save.
		template<typename Archive>
		void SaveLOD(Archive& archive) const
		{
			archive(distance);

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
		}

		// Serialization Load.
		template<typename Archive>
		void LoadLOD(Archive& archive)
		{
			archive(distance);

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
					sections[i] = Ptr<MeshSection>(new MeshSection());
					archive(*sections[i]);
				}
			}
		}

	};




	// Mesh:
	//  - a mesh that is made of multiple seprate sections eached mapped to there own material.
	//  - support multiple level of details LODs.
	//
	class Mesh : public IResource
	{
	public:
		// Construct.
		Mesh() 
			: IResource() 
		{
			type = Mesh::StaticGetType();
			hasRenderResources = true;
		}

		virtual ~Mesh()
		{

		}

		// return the resource type
		inline static EResourceType StaticGetType() noexcept { return EResourceType::RT_Mesh; }

		// Load Render Resrouces.
		inline virtual void LoadRenderResource() override
		{
			RAVEN_ASSERT(!isOnGPU, "Resrouce already on GPU. use UpdateRenderRsc to update.");
			isOnGPU = true;

			for (auto& section : meshLOD0.sections)
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

		// Set a new main mesh section.
		inline void SetMeshSection(uint32_t index, Ptr<MeshSection> section)
		{
			if (meshLOD0.sections.size() < index + 1)
				meshLOD0.sections.resize(index + 1);

			meshLOD0.sections[index] = section;
			UpdateBounds();
		}

		// Add new mesh section at the end of the sections list.
		inline void AddMeshSection(Ptr<MeshSection> section)
		{
			meshLOD0.sections.push_back(section);
			UpdateBounds();
		}

		// Return the bounds of this mesh.
		inline const MathUtils::BoundingBox& GetBounds() const { return bounds; }

		// Add new mesh LOD level.
		inline void AddNewLOD(const std::vector< Ptr<MeshSection> >& lodSections)
		{
			RAVEN_ASSERT(!meshLOD0.sections.empty(), "Should have at least LOD_0");

			MeshLOD& lod = LODs.emplace_back( MeshLOD() );
			lod.sections = lodSections;
		}

		// Remove LOD level.
		inline void RemoveLOD(uint32_t level)
		{
			RAVEN_ASSERT(level == 0, "To remove level 0 you need to remove one section at a time.");
			LODs.erase(LODs.begin() + level);
		}

		// Return the number of lods
		inline uint32_t GetNumLODs() { return (uint32_t)LODs.size() + 1; }

		// Return mesh lod at level.
		inline MeshLOD& GetMeshLOD(uint32_t level) 
		{
			if (level == 0)
				return meshLOD0;

			return LODs[level];
		}

		// Return mesh lod at level.
		inline const MeshLOD& GetMeshLOD(uint32_t level) const { return const_cast<const Mesh*>(this)->GetMeshLOD(level); }

		// Serialization Save.
		template<typename Archive>
		void save(Archive& archive) const
		{
			archive(cereal::base_class<IResource>(this));

			// Save main mesh sections LOD0.
			meshLOD0.SaveLOD(archive);

			// Save LODs...
			uint32_t LODsSize = LODs.size();
			archive(LODsSize);
			for (uint32_t i = 0; i < LODsSize; ++i)
			{
				LODs[i].SaveLOD(archive);
			}
		}

		// Serialization Load.
		template<typename Archive>
		void load(Archive& archive)
		{
			archive(cereal::base_class<IResource>(this));

			// Load main mesh sections LOD0.
			meshLOD0.LoadLOD(archive);
			
			// Load LODs...
			uint32_t LODsSize = 0;
			archive(LODsSize);
			LODs.resize(LODsSize);

			for (uint32_t i = 0; i < LODsSize; ++i)
			{
				LODs[i].LoadLOD(archive);
			}

			// Update bounds after loading.
			UpdateBounds();
		}

	private:
		// Recompute/Update the bounding box the model.
		inline void UpdateBounds()
		{
			bounds.Reset();

			for (const auto& ms : meshLOD0.sections)
			{
				bounds.Add(ms->bounds);
			}
		}

	private:
		// this is the LOD_0 which is the original high-res mesh, contain list of all the mesh main sections, 
		MeshLOD meshLOD0;

		// Mesh LODs > 1, they should reflect meshSections LOD_0 in some way.
		std::vector<MeshLOD> LODs;

		// The boudning box of all mesh sections.
		MathUtils::BoundingBox bounds;
	};

}
