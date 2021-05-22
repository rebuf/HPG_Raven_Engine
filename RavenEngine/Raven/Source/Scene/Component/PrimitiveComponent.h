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


#include "Utilities/Core.h"
#include "ResourceManager/RavenVersion.h"
#include "Component.h"

#include "Math/BoundingBox.h"
#include "Render/RenderObjects/RenderPrimitiveCollector.h"


#include <string>
#include <vector>
#include <memory>





namespace Raven
{
	class Mesh;
	class Material;
	class Scene;




	// PrimitiveComponent:
	//	- Parent class for all componenet that will be rendered like MeshComponent & SkinnedMeshComponent.
	//
	class PrimitiveComponent : public Component
	{
	public:
		// Construct.
		PrimitiveComponent();

		// Destruct.
		virtual ~PrimitiveComponent();

		// Set material at index, the material is going to be used by a mesh on the same index.
		void SetMaterial(uint32_t index, Ptr<Material> mat);

		// Return materail at index, if no material at that index return null.
		Material* GetMaterial(uint32_t index);
		const Material* GetMaterial(uint32_t index) const;

		// Retun the list materials in the model.
		auto& GetMaterials() { return materials; }
		const auto& GetMaterials() const { return materials; }

		// Return the current local bounds of the model.
		virtual const MathUtils::BoundingBox& GetLocalBounds() const { return localBounds; }

		// Called by the render to collect render primitives for drawing.
		virtual void CollectRenderPrimitives(RenderPrimitiveCollector& rcollector);

		// Get/Set Clip Distance.
		inline float GetClipDistance() { return clipDistance; }
		inline void SetClipDistance(float distance) { clipDistance = distance; }

		// Get/Set Cast Shadow.
		inline bool IsCastShadow() { return isCastShadow; }
		inline void SetCastShadow(bool val) { isCastShadow = val; }

	public:
		// serialization load and save
		template<typename Archive>
		void save(Archive& archive) const
		{
			archive(cereal::base_class<Component>(this));
			archive(clipDistance);

			// Saving Reference to materials.
			if (RavenVersionGlobals::SCENE_ARCHIVE_VERSION >= 10001)
			{
				uint32_t matCount = materials.size();
				archive(matCount);

				for (uint32_t i = 0; i < matCount; ++i)
				{
					// Save Resrouce Reference -> Material.
					ResourceRef::Save(archive, materials[i].get());
				}
			}

			// Start Archiving isCastShadow.
			if (RavenVersionGlobals::SCENE_ARCHIVE_VERSION >= 10002)
			{
				archive(isCastShadow);
			}

		}

		template<typename Archive>
		void load(Archive& archive)
		{
			archive(cereal::base_class<Component>(this));
			archive(clipDistance);

			// Load Reference to materials.
			if (RavenVersionGlobals::SCENE_ARCHIVE_VERSION >= 10001)
			{
				uint32_t matCount = 0;
				archive(matCount);
				materials.resize(matCount);

				for (uint32_t i = 0; i < matCount; ++i)
				{
					// Load Resrouce Reference -> Material.
					materials[i] = ResourceRef::Load(archive).FindOrLoad<Material>();
				}
			}

			// Start Archiving isCastShadow.
			if (RavenVersionGlobals::SCENE_ARCHIVE_VERSION >= 10002)
			{
				archive(isCastShadow);
			}

		}


	private:
		// The Model Materials, used by mesh mapped to the same index.
		std::vector< Ptr<Material> > materials;

	protected:
		// The local bounding box of the component.
		MathUtils::BoundingBox localBounds;

		// The distance this model will be clipped at, if -1 then no clipping.
		float clipDistance;

		// if true this primitive will cast shadow.
		bool isCastShadow;
	};

};