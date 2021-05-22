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
#pragma once



#include "Utilities/Core.h"
#include "ResourceManager/Resources/IResource.h"





namespace Raven
{
	// Types of all the importers in the engine.
	enum EImporterType
	{
		// Invalid Importer Type.
		IMP_None = -1,

		// Import images into textures.
		IMP_Image,

		// Import obj files into meshes.
		IMP_OBJ,

		// Import from FBX file into meshes, skeleton, animations, etc..
		IMP_FBX,
	};




	// IImporter:
	//    - The base class for importers managed by the resrouce manager.
	//
	class IImporter
	{
	public:
		// Default Construct.
		IImporter()
		{

		}

		// Destruct.
		virtual ~IImporter()
		{

		}

		// Return the importer type.
		inline EImporterType GetType() const noexcept { return type; }

		// List all extensions supported by this importer.
		virtual void ListExtensions(std::vector<std::string>& outExt) = 0;

		// Import a new resrouce.
		virtual bool Import(const std::string& path, std::vector< Ptr<IResource> >& resources) = 0;

	protected:
		// The type of importer.
		EImporterType type;

	};



}