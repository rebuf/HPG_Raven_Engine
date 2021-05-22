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

#pragma  once

#include "EditorWindow.h"

#include "ResourceManager/ResourceManager.h"

#include "Utilities/StringUtils.h"

#include <string>
#include <memory>

namespace Raven
{
	class ImportWindow : public EditorWindow
	{
		friend ResourceManager; // a friend of the resource manager class
	public:
		ImportWindow();
		virtual void OnImGui()  override;

	private:
		Ptr<ResourceManager> resourceManager;

		char input[StringUtils::MAX_PATH_LENGTH];

		std::string filter;
		
		bool onlyAnimation;
		bool selected;

		std::string dragInfo;

		// file info
		std::string fileName;
		std::string filePath;
	};
};