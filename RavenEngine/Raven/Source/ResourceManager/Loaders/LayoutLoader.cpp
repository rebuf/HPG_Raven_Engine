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



#include "LayoutLoader.h"
#include "GUI/GUILayout.h"
#include "Utilities/Serialization.h"

#include <cereal/types/vector.hpp>
#include "cereal/archives/json.hpp"

#include <sstream>
#include <fstream>



namespace Raven
{


LayoutLoader::LayoutLoader()
{

}


LayoutLoader::~LayoutLoader()
{

}


IResource* LayoutLoader::LoadResource(const ResourceHeaderInfo& info, RavenInputArchive& archive)
{
	// create the new layout
	GUILayout* layout = new GUILayout();

	// set layout from loaded file
	archive.ArchiveLoad(*layout);

	layout->InitLayout();

	return layout;
}


void LayoutLoader::SaveResource(RavenOutputArchive& archive, IResource* Resource)
{
	RAVEN_ASSERT(0, "Not Supported.");
}


void LayoutLoader::ListResourceTypes(std::vector<EResourceType>& outRscTypes)
{
	outRscTypes.push_back(RT_GuiLayout);
}


}
