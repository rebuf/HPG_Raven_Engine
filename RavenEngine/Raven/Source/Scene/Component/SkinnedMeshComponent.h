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
#include "PrimitiveComponent.h"
#include "ResourceManager/Resources/SkinnedMesh.h"


#include <string>
#include <vector>
#include <memory>






namespace Raven
{
	class SkinnedMesh;
	class SkeletonInstance;




	// SkinnedMeshComponent:
	//	- 
	//
	class SkinnedMeshComponent : public PrimitiveComponent
	{
	public:
		// Construct.
		SkinnedMeshComponent();

		// Destruct.
		virtual ~SkinnedMeshComponent();

		// Retun the list mesh in the model.
		inline auto& GetMesh() { return mesh; };
		inline const auto& GetMesh() const { return mesh; };

		// Set a new mesh to the list of meshes in the model.
		void SetMesh(Ptr<SkinnedMesh> newMesh);

		// Called by the render to collect render primitives for drawing.
		virtual void CollectRenderPrimitives(RenderPrimitiveCollector& rcollector) override;

		// Return the skeleton of this skinned mesh.
		inline SkeletonInstance* GetSkeleton() { return skeleton.get(); }

	public:
		// Serialization Save.
		template<typename Archive>
		void save(Archive& archive) const
		{
			archive(cereal::base_class<PrimitiveComponent>(this));

			// Save Resrouce Reference -> SkinnedMesh.
			ResourceRef::Save(archive, mesh.get());

			skeleton->SaveTransformHierarchy(archive);
		}

		// Serialization Load.
		template<typename Archive>
		void load(Archive& archive)
		{
			archive(cereal::base_class<PrimitiveComponent>(this));

			isLoading = true;

			// Load Resrouce Reference -> SkinnedMesh.
			Ptr<SkinnedMesh> meshRef = ResourceRef::Load(archive).FindOrLoad<SkinnedMesh>();
			SetMesh(meshRef);

			skeleton->LoadTransformHierarchy(archive);

			isLoading = false;
		}


	private:
		// The Model Mesh, each mapped to a materail on the same index.
		Ptr<SkinnedMesh> mesh;

		// The instance of the skeleton used to update the bone transforms.
		Ptr<SkeletonInstance> skeleton;

		// if true we are currently loading the skinned mesh component.
		bool isLoading;

		// The Hierarchy is dirty and need updating, this is set while loading because we don't have the entities created at this point.
		bool isHierarchyDirty;
	};

};