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
#include "ResourceManager/Loaders/ImageLoader.h"
#include "ResourceManager/Resources/Texture2D.h"






#include <iostream>
#include <fstream>




namespace Raven
{


ImageLoader::ImageLoader()
{

}


ImageLoader::~ImageLoader()
{

}


IResource* ImageLoader::LoadResource(const ResourceHeaderInfo& info, RavenInputArchive& archive)
{
	switch (info.GetType())
	{
	case EResourceType::RT_Texture2D:
	{
		Texture2D* texture = new Texture2D();
		archive.ArchiveLoad(*texture);
		return texture;
	}

	default:
		RAVEN_ASSERT(0, "Not Supported.");
		break;
	}

	return nullptr;
}


void ImageLoader::SaveResource(RavenOutputArchive& archive, IResource* Resource)
{
	switch (Resource->GetType())
	{
	case EResourceType::RT_Texture2D:
	{
		Texture2D* texture = static_cast<Texture2D*>(Resource);
		archive.ArchiveSave(*texture);
	}
		break;

	default:
		RAVEN_ASSERT(0, "Not Supported.");
		break;
	}
}


void ImageLoader::ListResourceTypes(std::vector<EResourceType>& outRscTypes)
{
	outRscTypes.push_back(RT_Texture2D);
	outRscTypes.push_back(RT_TextureCube);
}


} // End of namespace Raven

