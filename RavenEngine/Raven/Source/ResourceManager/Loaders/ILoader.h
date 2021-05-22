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

#pragma once


#include "Utilities/Core.h"
#include "ResourceManager/RavenVersion.h"
#include "ResourceManager/Resources/IResource.h"


#include "cereal/archives/json.hpp"
#include "cereal/archives/binary.hpp"


#include <string>
#include <iostream>
#include <fstream>




// Tag at the start of every Raven Resource, translate to raven.
// used to make sure the Resource is raven Resource.
#define ResourceS_HEADER_TAG "RAVEN"






namespace Raven 
{


	// The type of loaders (for each type of resource)
	enum class ELoaderType {
		LT_Image,
		LT_Mesh,
		LT_Material,
		LT_Scene,
		LT_Audio,
		LT_GuiLayout,
		LT_Animation,
		LT_SkinnedMesh,

		LT_MAX
	};




	// RavenInputArchive:
	//		- Provide File Utilities for cereal archive.
	//
	class RavenInputArchive
	{
		NOCOPYABLE(RavenInputArchive);

	public:
		// Create an input archive
		RavenInputArchive(const std::string& file)
		{
			fileStream.open(file, std::ios::in | std::ios::binary);
			archive = new cereal::BinaryInputArchive(fileStream);
		}


		// Destructor.
		~RavenInputArchive()
		{
			if (fileStream.is_open())
			{
				fileStream.close();
			}

			delete archive;
		}

		// Return true if the archive stream is valid.
		inline bool IsValid() { return fileStream.is_open(); }

		// Archive
		template<class T>
		inline void ArchiveLoad(T& obj)
		{
			ILoader::SetLoadVersion(obj, version);
			(*archive)(obj);
		}


	private:
		// Jason Archive.
		cereal::BinaryInputArchive* archive;

		// The file stream.
		std::ifstream fileStream;


	public:
		// The version of the loaded file
		uint32_t version;
	};



	// RavenInputArchive:
	//		- Provide File Utilities for cereal archive.
	//
	class RavenOutputArchive
	{
		NOCOPYABLE(RavenOutputArchive);

	public:
		// Create an output rchive
		RavenOutputArchive(const std::string& file)
		{
			fileStream.open(file, std::ios::out | std::ios::binary);
			archive = new cereal::BinaryOutputArchive(fileStream);
		}


		// Destructor.
		~RavenOutputArchive()
		{
			if (fileStream.is_open())
			{
				fileStream.close();
			}

			delete archive;
		}

		// Return true if the archive stream is valid.
		inline bool IsValid() { return fileStream.is_open(); }

		// Archive
		template<class T>
		inline void ArchiveSave(T& obj)
		{
			(*archive)(obj);
		}

	private:
		// Jason Archive.
		cereal::BinaryOutputArchive* archive;

		// The file stream.
		std::ofstream fileStream;

	};



	// ResourceHeaderInfo:
	//    - a header file info for our Resources.
	struct ResourceHeaderInfo
	{
		// Friend
		friend class ILoader;

	private:
		// The File version 
		uint32_t version;

		// The type of the Resource, converted from EResourceType.
		int32_t type;

	public:
		// Construct.
		ResourceHeaderInfo()
			: type(-1)
			, version(RAVEN_VERSION)
		{

		}

		// Construct from Resource.
		ResourceHeaderInfo(IResource* rsc, bool binary)
		{
			version = RAVEN_VERSION;
			type = static_cast<uint32_t>(rsc->GetType());
		}

		// Save Raven file header.
		template<typename Archive>
		void save(Archive& archive) const
		{
			// Archive Tag.
			std::string tag = ResourceS_HEADER_TAG;
			archive(tag);

			// Archive Header...
			archive(
				version,
				type
			);
		}

		// Load Raven file header.
		template<typename Archive>
		void load(Archive& archive)
		{
			// Archive Tag.
			std::string tag;
			archive(tag);

			// Not Raven Resource?
			if (tag != ResourceS_HEADER_TAG)
			{
				LOGW("Failed to archive load a resrouce. Resrouce is not a RAVEN.");
				return;
			}

			// Archive Header...
			archive(
				version,
				type
			);
		}

		// Getters...
		inline uint32_t GetVersion() const { return version; }
		inline EResourceType GetType() const { return static_cast<EResourceType>(type); }

		// Return true if valid header.
		inline bool IsValid() const { return type != -1; }

	};




	// ILoader:
	//   - Base class for all loaders in the engine.
	//   - Note: Every loader should implement static Type() function, used to identify different loaders.
	class ILoader 
	{
		NOCOPYABLE(ILoader);

		// Friend Archive.
		friend class RavenInputArchive;

	public:
		// Construct.
		ILoader()
		{

		}

		// Destruct.
		virtual ~ILoader()
		{

		}


		// Convert a type to its string name.
		inline static std::string ToString(ELoaderType type)
		{
			switch (type)
			{
			case ELoaderType::LT_Image:
				return "Type: Image Loader.";
			case ELoaderType::LT_Mesh:
				return "Type: Mesh Loader.";
			case ELoaderType::LT_Material:
				return "Type: Material Loader.";
			case ELoaderType::LT_Scene:
				return "Type: Scene Loader.";
			case ELoaderType::LT_Audio:
				return "Type: Audio Loader.";
			case ELoaderType::LT_GuiLayout:
				return "Type: GUI Layout Loader.";
			default:
				return "Loader type not recognised";
			}
		}

		// Load Resource from archive.
		virtual IResource* LoadResource(const ResourceHeaderInfo& info, RavenInputArchive& archive) = 0;

		// Save Resource into archive.
		virtual void SaveResource(RavenOutputArchive& archive, IResource* Resource) = 0;

		// Load only the Resource header info from Resource file.
		static ResourceHeaderInfo LoadHeader(RavenInputArchive& archive);

		// Save the header at the start of the file and return the offset.
		static ResourceHeaderInfo SaveHeader(RavenOutputArchive& archive, IResource* rsc);

		// List all resources that supported by this loader.
		virtual void ListResourceTypes(std::vector<EResourceType>& outRscTypes) = 0;

	private:
		// Set load version value to the resrouce we are loading.
		template< class T, std::enable_if_t< !std::is_base_of<IResource, T>::value, bool > = true >
		static void SetLoadVersion(T& rsc, uint32_t load_version) { }
		static void SetLoadVersion(IResource& rsc, uint32_t load_version) { rsc.load_version = load_version; }


	};
}
