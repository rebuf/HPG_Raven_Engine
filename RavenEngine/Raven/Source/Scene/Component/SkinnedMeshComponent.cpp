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
#include "SkinnedMeshComponent.h"

#include "ResourceManager/Resources/SkinnedMesh.h"
#include "ResourceManager/Resources/Material.h"
#include "ResourceManager/Resources/MaterialShader.h"
#include "Animation/Skeleton.h"
#include "Scene/Entity/Entity.h"

#include "Render/RenderObjects/Primitives/RenderSkinnedMesh.h"


namespace Raven {



SkinnedMeshComponent::SkinnedMeshComponent()
	: isLoading(false)
	, isHierarchyDirty(false)
{
	
}


SkinnedMeshComponent::~SkinnedMeshComponent()
{

}


void SkinnedMeshComponent::SetMesh(Ptr<SkinnedMesh> newMesh)
{
	// Cleanup...
	if (skeleton)
	{
		skeleton->DestroyTransformHierarchy();
	}

	// Set new...
	mesh = newMesh;

	// Invalid?
	if (!mesh)
	{
		localBounds = MathUtils::BoundingBox();
		return;
	}

	// Has Valid Skeleton?
	if (mesh->GetSkeleton())
	{
		skeleton = Ptr<SkeletonInstance>(new SkeletonInstance(this, mesh->GetSkeleton()));

		if (!isLoading)
		{
			skeleton->BuildTransformHierarchy();
			skeleton->UpdateBones();
		}
		else
		{
			isHierarchyDirty = true;
		}
	}

	localBounds = mesh->GetBounds();
}


void SkinnedMeshComponent::CollectRenderPrimitives(RenderPrimitiveCollector& rcollector)
{
	// Invalid Mesh?
	if (!mesh)
	{
		return;
	}

	if (isHierarchyDirty)
	{
		skeleton->SetScene(GetEntity().GetScene());
		skeleton->UpdateBones();
		isHierarchyDirty = false;
	}

	for (uint32_t i = 0; i < mesh->GetNumSections(); ++i)
	{
		SkinnedMeshSection* meshSection = mesh->GetMeshSection(i);

		// Invalid?
		if (!meshSection)
		{
			continue;
		}

		// Add mesh section to be render.
		RenderSkinnedMesh* rmesh = rcollector.NewSkinnedMesh();
		rmesh->SetMesh(meshSection->renderRscMesh.get());
		RAVEN_ASSERT(skeleton->GetBones()->size() <= RENDER_SKINNED_MAX_BONES, "");
		rmesh->SetBones(skeleton->GetBones());

		// Material to use while rendering the mesh section.
		Material* mat = GetMaterial(i);

		// No Materail? Try get default material.
		if (!mat)
		{
			auto& defaultMaterial = meshSection->defaultMaterial;

			// Has Default Material?
			if (defaultMaterial.IsValid())
			{
				mat = defaultMaterial.GetWeak<Material>();
			}
		}

		// Has Material?
		if (mat)
		{
			// Update Material Paramters if Dirty.
			if (mat->IsDirty())
			{
				mat->UpdateRenderResource();
			}

			rmesh->SetMaterial(mat->GetRenderRsc());
		}
	}
}


} // End of namespace Raven 
