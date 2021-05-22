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


#include "ResourceManager/Importers/Importer.h"



namespace Raven
{

	// OBJImporter:
	//    - import obj into a mesh resource.
	class OBJImporter : public IImporter
	{
	public:
		// Construct.
		OBJImporter();

		// Return the importer type.
		inline static EImporterType StaticGetType() noexcept { return IMP_OBJ; }

		// List all supported extensions by this importer.
		virtual void ListExtensions(std::vector<std::string>& outExt) override;

		// Import a new resrouce.
		virtual bool Import(const std::string& path, std::vector< Ptr<IResource> >& resources) override;

	private:
		// Load a mesh resource from an obj file.
		IResource* LoadOBJ(const std::string& path);
	};
}