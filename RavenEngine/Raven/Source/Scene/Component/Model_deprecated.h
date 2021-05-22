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
#include "Component.h"
#include "Animation/Bone.h"
#include "ResourceManager/Resources/SkinnedMesh.h"
#include "ResourceManager/MeshFactory.h"
#include "ResourceManager/Resources/Fbx.h"


#include <string>
#include <vector>
#include <memory>





namespace Raven
{
	class Entity;
	class Mesh;
	class Material;
	class Scene;


	// Data of MeshRenderer as viewed by the model
	struct ModelMeshRendererData
	{
#if 0
		MeshRenderer* mesh;
		SkinnedMeshRenderer* skinned;

		ModelMeshRendererData()
			: mesh(nullptr)
			, skinned(nullptr)
		{

		}
#endif
	};



	// Model:
	//	- 3D models and their related data such as meshes, and materials
	//	- Represent a mesh in the scene with material.
	//
	class Model_deprecated : public Component
	{
		// Friend loader for setting file name
		friend class ModelLoader; 

	public:
		Model_deprecated();
		Model_deprecated(const std::string & fileName);

		~Model_deprecated();

		void LoadOnGpu();

		// 
		SkinnedMeshSection* GetMeshSection(size_t index);

		// Retun the list meshes in the model.
		inline auto& GetMeshSections() { return mesh->GetMeshSections(); };
		inline const auto& GetMeshSections() const { return mesh->GetMeshSections(); };

		// Add new mesh to the list of meshes in the model.
		void AddMesh(Ptr<SkinnedMeshSection> inMesh);

		// Set material at index, the material is going to be used by a mesh on the same index.
		void SetMaterial(uint32_t index, Ptr<Material> mat);

		// Return materail at index, if no material at that index return null.
		Material* GetMaterial(uint32_t index);
		const Material* GetMaterial(uint32_t index) const;

		// Retun the list materials in the model.
		auto& GetMaterials() { return materials; }
		const auto& GetMaterials() const { return materials; }

		// Recompute/Update the bounding box the model.
		void UpdateBounds();

		// Return the current local bounds of the model.
		inline const auto& GetLocalBounds() { return localBounds; }

		inline auto GetPrimitiveType() const { return primitiveType; }
		inline auto SetPrimitiveType(EBasicShape::Type type) { primitiveType = type; }

		inline auto GetFileName() const { return filePath; }

		// serialization load and save
		template<typename Archive>
		void save(Archive& archive) const
		{
			if (meshes.size() > 0)
			{
				archive(
					cereal::make_nvp("PrimitiveType", primitiveType), 
					cereal::make_nvp("FilePath", filePath),
					cereal::make_nvp("Id", entity)
					);
			}
		}

		template<typename Archive>
		void load(Archive& archive)
		{
			archive(
				cereal::make_nvp("PrimitiveType", primitiveType), 
				cereal::make_nvp("FilePath", filePath),
				cereal::make_nvp("Id", entity));

			meshes.clear();

			if (primitiveType != PrimitiveType::File)
			{
				Ptr<Mesh> newMesh(MeshFactory::CreatePrimitive(primitiveType));
				AddMesh(newMesh);
			}
			else
			{
				LoadFile(true);
			}

			UpdateBounds();
		}

		//if fromLoad is true, indicating engine should not generate the bones and the entities from fbx
//
		void LoadFile(bool fromLoad = false);

		// Return mesh renderers for at their mesh indices.
		void GetMeshRenderers(std::vector<ModelMeshRendererData>& outMesRenderers, Scene* scene);

	private:
		// Recursive Implemenation fo GetMeshRenderer
		void GetMeshRenderersImp(std::vector<ModelMeshRendererData>& outMesRenderers, Entity& ent);

	private:

		void BindMeshComponent();
		void BindMeshComponentForFBX();
		inline void SetFileName(const std::string& path) { filePath = path; }

	private:
		std::string filePath;

		// The Model Meshes, each mapped to a materail on the same index.
		Ptr<SkinnedMesh> mesh;

		// The Model Materials, used by meshes mapped to the same index.
		std::vector< Ptr<Material> > materials;

		EBasicShape::Type primitiveType;

		// The local bounding box of the model that contain all its meshes.
		MathUtils::BoundingBox localBounds;
	};
};