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



#include "MeshComponent.h"
#include "ResourceManager/Resources/Mesh.h"
#include "ResourceManager/Resources/Material.h"
#include "ResourceManager/Resources/MaterialShader.h"
#include "Render/RenderObjects/Primitives/RenderMesh.h"



namespace Raven {



MeshComponent::MeshComponent()
{
	
}


MeshComponent::~MeshComponent()
{

}


void MeshComponent::SetMesh(Ptr<Mesh> newMesh)
{
	mesh = newMesh;
	localBounds = mesh != nullptr ? mesh->GetBounds() : MathUtils::BoundingBox();
}


void MeshComponent::CollectRenderPrimitives(RenderPrimitiveCollector& rcollector)
{
	// Invalid Mesh?
	if (!mesh)
	{
		return;
	}

	MeshLOD* meshLOD;

	// Has LODs?
	uint32_t numLODs = mesh->GetNumLODs();

	if (numLODs > 1)
	{
		uint32_t level;

		for (level = 1; level < numLODs; ++level)
		{
			if (rcollector.GetViewDistance() < mesh->GetMeshLOD(level).distance)
			{
				level -= 1;
				break;
			}
		}

		meshLOD = &mesh->GetMeshLOD(level);
	}
	else
	{
		meshLOD = &mesh->GetMeshLOD(0);
	}


	for (uint32_t i = 0; i < meshLOD->sections.size(); ++i)
	{
		MeshSection* meshSection = meshLOD->sections[i].get();

		// Invalid?
		if (!meshSection)
		{
			continue;
		}

		// Add mesh section to be render.
		RenderMesh* rmesh = rcollector.NewMesh();
		rmesh->SetMesh( meshSection->renderRscMesh.get() );

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
			if (mat->GetMaterialShader() && mat->GetMaterialShader()->IsOnGPU())
			{
				// Update Material Paramters if Dirty.
				if (mat->IsDirty())
				{
					mat->UpdateRenderResource();
				}
			}

			rmesh->SetMaterial(mat->GetRenderRsc());
		}
	}
}


} // End of namespace Raven 
