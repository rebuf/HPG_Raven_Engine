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

#include "SceneLoader.h"
#include "Scene/Scene.h"


#include "Utilities/Serialization.h"




namespace Raven
{


SceneLoader::SceneLoader()
{

}


IResource* SceneLoader::LoadResource(const ResourceHeaderInfo& info, RavenInputArchive& archive)
{
	RavenVersionGlobals::SCENE_ARCHIVE_VERSION = info.GetVersion();
	RAVEN_ASSERT(info.GetType() == EResourceType::RT_Scene, "Must be a scene.");

	Scene* scene = new Scene("LOAD_TMP_NAME");

	std::stringstream ss;
	{
		std::string str;
		archive.ArchiveLoad(str); // Load JSON...

		ss << str;
	}

	scene->LoadFromStream(ss);
	RavenVersionGlobals::SCENE_ARCHIVE_VERSION = RAVEN_VERSION;
	return scene;
}


void SceneLoader::SaveResource(RavenOutputArchive& archive, IResource* Resource)
{
	switch (Resource->GetType())
	{
	case EResourceType::RT_Scene:
	{
		Scene* scene = static_cast<Scene*>(Resource);

		std::string str;
		{
			std::stringstream ss;
			scene->SaveToStream(ss); // Save JSON...

			str = ss.str();
		}

		archive.ArchiveSave(str);
	}
	break;

	default:
		RAVEN_ASSERT(0, "Not Supported.");
		break;
	}

}


void SceneLoader::ListResourceTypes(std::vector<EResourceType>& outRscTypes)
{
	outRscTypes.push_back(RT_Scene);
}


} // End of namespace Raven.

