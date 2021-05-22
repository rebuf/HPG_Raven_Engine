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
#include "AnimationLoader.h"



#include "Animation/Animation.h"
#include "Animation/AnimationController.h"




namespace Raven
{


AnimationLoader::AnimationLoader()
{

}


IResource* AnimationLoader::LoadResource(const ResourceHeaderInfo& info, RavenInputArchive& archive)
{
	switch (info.GetType())
	{
	case EResourceType::RT_AnimationClip:
	{
		AnimationClip* anime = new AnimationClip();
		archive.ArchiveLoad(*anime);
		return anime;
	}

	case EResourceType::RT_AnimationController:
	{
		AnimationController* animeController = new AnimationController();
		archive.ArchiveLoad(*animeController);
		return animeController;
	}

	default:
		RAVEN_ASSERT(0, "Not Supported.");
		break;
	}

	return nullptr;
}


void AnimationLoader::SaveResource(RavenOutputArchive& archive, IResource* Resource)
{
	switch (Resource->GetType())
	{
	case EResourceType::RT_AnimationClip:
	{
		AnimationClip* anime = static_cast<AnimationClip*>(Resource);
		archive.ArchiveSave(*anime);
	}
		break;

	case EResourceType::RT_AnimationController:
	{
		AnimationController* anime = static_cast<AnimationController*>(Resource);
		archive.ArchiveSave(*anime);
	}
		break;

	default:
		RAVEN_ASSERT(0, "Not Supported.");
		break;
	}
}


void AnimationLoader::ListResourceTypes(std::vector<EResourceType>& outRscTypes)
{
	outRscTypes.push_back(RT_AnimationClip);
	outRscTypes.push_back(RT_AnimationController);
}



} // End of namespace Raven.

