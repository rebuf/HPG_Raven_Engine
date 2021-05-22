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
#include "MeshLoader.h"
#include "Utilities/StringUtils.h"

#include "ResourceManager/Resources/Mesh.h"



namespace Raven {


MeshLoader::MeshLoader()
{

}


MeshLoader::~MeshLoader()
{

}


IResource* MeshLoader::LoadResource(const ResourceHeaderInfo& info, RavenInputArchive& archive)
{
	switch (info.GetType())
	{
	case EResourceType::RT_Mesh:
	{
		Mesh* mesh = new Mesh();
		archive.ArchiveLoad(*mesh);
		return mesh;
	}
		break;

	default:
		RAVEN_ASSERT(0, "Not Supported.");
		break;
	}

	return nullptr;
}


void MeshLoader::SaveResource(RavenOutputArchive& archive, IResource* Resource)
{
	switch (Resource->GetType())
	{
	case EResourceType::RT_Mesh:
	{
		Mesh* mesh = static_cast<Mesh*>(Resource);
		archive.ArchiveSave(*mesh);
	}
		break;


	default:
		RAVEN_ASSERT(0, "Not Supported.");
		break;
	}
}


void MeshLoader::ListResourceTypes(std::vector<EResourceType>& outRscTypes)
{
	outRscTypes.push_back(RT_Mesh);
}



} // End of namespace Raven 