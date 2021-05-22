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
#include "SkinnedMeshLoader.h"
#include "Utilities/StringUtils.h"


#include "ResourceManager/Resources/SkinnedMesh.h"
#include "Animation/Skeleton.h"




namespace Raven {


SkinnedMeshLoader::SkinnedMeshLoader()
{

}


SkinnedMeshLoader::~SkinnedMeshLoader()
{

}


IResource* SkinnedMeshLoader::LoadResource(const ResourceHeaderInfo& info, RavenInputArchive& archive)
{
	switch (info.GetType())
	{
	case EResourceType::RT_SkinnedMesh:
	{
		SkinnedMesh* skinnedMesh = new SkinnedMesh();
		archive.ArchiveLoad(*skinnedMesh);
		return skinnedMesh;
	}
		break;

	case EResourceType::RT_Skeleton:
	{
		Skeleton* skeleton = new Skeleton();
		archive.ArchiveLoad(*skeleton);
		return skeleton;
	}
		break;

	default:
		RAVEN_ASSERT(0, "Not Supported.");
		break;
	}

	return nullptr;
}


void SkinnedMeshLoader::SaveResource(RavenOutputArchive& archive, IResource* Resource)
{
	switch (Resource->GetType())
	{
	case EResourceType::RT_SkinnedMesh:
	{
		SkinnedMesh* skinnedMesh = static_cast<SkinnedMesh*>(Resource);
		archive.ArchiveSave(*skinnedMesh);
	}
		break;

	case EResourceType::RT_Skeleton:
	{
		Skeleton* skeleton = static_cast<Skeleton*>(Resource);
		archive.ArchiveSave(*skeleton);
	}
		break;

	default:
		RAVEN_ASSERT(0, "Not Supported.");
		break;
	}
}


void SkinnedMeshLoader::ListResourceTypes(std::vector<EResourceType>& outRscTypes)
{
	outRscTypes.push_back(RT_SkinnedMesh);
	outRscTypes.push_back(RT_Skeleton);
}



} // End of namespace Raven 