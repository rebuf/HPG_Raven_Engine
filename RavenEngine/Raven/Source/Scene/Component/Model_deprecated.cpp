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

#include "Scene/Component/Model_deprecated.h"
#include "ResourceManager/ResourceManager.h"
#include "Scene/SceneManager.h"
#include "Scene/Scene.h"
#include "Scene/Entity/EntityManager.h"
#include "ResourceManager/FbxLoader_deprecated.h"
#include "Utilities/StringUtils.h"

#include "Engine.h"


namespace Raven
{
	Model_deprecated::Model_deprecated()
	{
		mesh = Ptr<SkinnedMesh>(new SkinnedMesh());
	}

	Model_deprecated::Model_deprecated(const std::string & fileName)
		: primitiveType(EBasicShape::Capsule), filePath(fileName)
	{
		mesh = Ptr<SkinnedMesh>(new SkinnedMesh());
	}

	Model_deprecated::~Model_deprecated()
	{
		mesh.reset();
	}

	void Model_deprecated::LoadOnGpu()
	{
		// loads the meshes onto the GPU
		mesh->LoadRenderResource();
	}

	// return shared pointer to a mesh resource
	SkinnedMeshSection* Model_deprecated::GetMeshSection(size_t index)
	{
		if ( index >= mesh->GetNumSections() )
		{
			return nullptr;
		}
		else
		{
			// can use this pointer to load a single mesh onto the GPU
			return mesh->GetMeshSection(index);
		}
	}

	void Model_deprecated::AddMesh(Ptr<SkinnedMeshSection> inMesh)
	{
		mesh->AddMeshSection(inMesh);
		UpdateBounds();
	}

	void Model_deprecated::UpdateBounds()
	{
		localBounds.Reset();
		localBounds = mesh->GetBounds();
	}


	void Model_deprecated::SetMaterial(uint32_t index, Ptr<Material> mat)
	{
		if (materials.size() < index + 1)
			materials.resize(index + 1);

		materials[index] = mat;
	}

	Material* Model_deprecated::GetMaterial(uint32_t index)
	{
		// Invalid Index?
		if (index >= materials.size() || index < 0)
			return nullptr;

		return materials[index].get();
	}

	const Material* Model_deprecated::GetMaterial(uint32_t index) const
	{
		return const_cast<Model_deprecated*>(this)->GetMaterial(index);
	}

	void Model_deprecated::LoadFile(bool fromLoad)
	{
#if 0
		auto res = Engine::Get().GetModule<ResourceManager>();
		auto loader = res->GetLoader<MeshLoader>();

		std::string extension = StringUtils::GetExtension(filePath);
		// file extension calls appropriate loading function
		if (extension == "obj")
		{
			loader->LoadOBJ(filePath);
			res->LoadResource<Mesh>(filePath);
			res->GetResource(filePath, meshes);
			BindMeshComponent();
		}
		else if (extension == "fbx")
		{
			loader->LoadFBX(filePath, fromLoad ? nullptr : this);
			if(meshes.empty())
				res->GetResource(filePath, meshes);
			BindMeshComponentForFBX();
		}
#endif
	}

	void Model_deprecated::BindMeshComponentForFBX()
	{
#if 0
		auto currentScene = Engine::Get().GetModule<SceneManager>()->GetCurrentScene();
		Entity ent = GetEntity();
		if (ent.GetChildren().empty())
		{
			FbxLoader_deprecated loader;
			loader.LoadHierarchy(filePath, this);

			for (auto i = 0; i < meshes.size(); i++)
			{
				auto entity = ent.GetChildInChildren(meshes[i]->GetName());
				if (!entity.Valid()) {
					entity = currentScene->CreateEntity(meshes[i]->GetName());
					entity.SetParent(ent);
				}

				if (!meshes[i]->blendIndices.empty() ||
					!meshes[i]->blendWeights.empty())
				{
					auto& render = entity.GetOrAddComponent<SkinnedMeshRenderer>();
					render.mesh = meshes[i];
					render.meshIndex = i;
					//skeleton should be copy for every skinned mesh.
					render.skeleton = *SkeletonCache::Get().Get(filePath); //copy a skeleton
					render.skeleton.ResetTransfromTarget(ent);
				}
				else
				{
					auto& render = entity.GetOrAddComponent<MeshRenderer>();
					render.mesh = meshes[i];
					render.meshIndex = i;
				}
			}
		}
#endif
	}

	void Model_deprecated::BindMeshComponent()
	{
#if 0
		auto currentScene = Engine::Get().GetModule<SceneManager>()->GetCurrentScene();
		auto& enManager = currentScene->GetEntityManager();

		Entity ent(entity, currentScene);

		if (ent.GetChildren().empty())
		{
			if (meshes.size() == 1)
			{
				auto& render = ent.GetOrAddComponent<MeshRenderer>();
				render.mesh = meshes[0];
				render.meshIndex = 0;
			}
			else
			{
				for (auto i = 0; i < meshes.size(); i++)
				{
					auto entity = ent.GetChildInChildren(meshes[i]->GetName());
					if (!entity.Valid()) {
						entity = currentScene->CreateEntity(meshes[i]->GetName());
						entity.SetParent(ent);
					}
					auto& render = entity.AddComponent<MeshRenderer>();
					render.mesh = meshes[i];
					render.meshIndex = i;
				}
			}
		}
#endif
	}


	void Model_deprecated::GetMeshRenderersImp(std::vector<ModelMeshRendererData>& outMesRenderers, Entity& ent)
	{
#if 0
		// Try to get mesh renderer.
		auto* rmesh = ent.TryGetComponent<MeshRenderer>();
		if (rmesh)
			outMesRenderers[rmesh->meshIndex].mesh = rmesh;

		// Try to get mesh skinned mesh renderer.
		auto* rskinned = ent.TryGetComponent<SkinnedMeshRenderer>();
		if (rskinned)
			outMesRenderers[rskinned->meshIndex].skinned = rskinned;

		// Iterate on childrens...
		auto children = ent.GetChildren();

		for (auto childEntt : children)
		{
			GetMeshRenderersImp(outMesRenderers, childEntt);
		}
#endif
	}



	void Model_deprecated::GetMeshRenderers(std::vector<ModelMeshRendererData>& outMesRenderers, Scene* scene)
	{
		Entity ent = GetEntity();
		outMesRenderers.resize(mesh->GetNumSections());
		GetMeshRenderersImp(outMesRenderers, ent);
	}



}