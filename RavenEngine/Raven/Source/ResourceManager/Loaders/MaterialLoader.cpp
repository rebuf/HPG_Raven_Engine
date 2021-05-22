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

#include "MaterialLoader.h"

#include "ResourceManager/Resources/MaterialShader.h"
#include "ResourceManager/Resources/Material.h"






namespace Raven
{


MaterialLoader::MaterialLoader()
{

}


IResource* MaterialLoader::LoadResource(const ResourceHeaderInfo& info, RavenInputArchive& archive)
{
	switch (info.GetType())
	{
	case EResourceType::RT_Material:
	{
		Material* mat = new Material();
		archive.ArchiveLoad(*mat);
		return mat;
	}
		break;

	case EResourceType::RT_MaterialShader:
	{
		MaterialShader* shader = new MaterialShader();
		archive.ArchiveLoad(*shader);
		return shader;
	}
		break;

	default:
		RAVEN_ASSERT(0, "Not Supported.");
		break;
	}

	return nullptr;
}


void MaterialLoader::SaveResource(RavenOutputArchive& archive, IResource* Resource)
{
	switch (Resource->GetType())
	{
	case EResourceType::RT_Material:
	{
		Material* mat = static_cast<Material*>(Resource);
		archive.ArchiveSave(*mat);
	}
		break;

	case EResourceType::RT_MaterialShader:
	{
		MaterialShader* shader = static_cast<MaterialShader*>(Resource);
		archive.ArchiveSave(*shader);
	}
		break;

	default:
		RAVEN_ASSERT(0, "Not Supported.");
		break;
	}
}


void MaterialLoader::ListResourceTypes(std::vector<EResourceType>& outRscTypes)
{
	outRscTypes.push_back(RT_Material);
	outRscTypes.push_back(RT_MaterialShader);
}



} // End of namespace Raven.

