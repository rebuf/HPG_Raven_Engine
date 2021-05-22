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



#include "IModule.h"

#include "Resources/IResource.h"
#include "Importers/Importer.h"
#include "Loaders/ILoader.h"



#include "Utilities/Core.h"


#include <string>
#include <unordered_map>
#include <memory>	
#include <array>
#include <vector>





namespace Raven
{
	// Hold Data for a single Resource.
	class ResourceData
	{
	public:
		// The type of the Resource.
		EResourceType type;

		// The Relative Resource Path.
		std::string path;

		// a cleaned relative path used for mapping.
		std::string cleanPath;

		// The header data that contains information about the Resource.
		ResourceHeaderInfo info;

		// The Resource.
		Ptr<IResource> rsc;
	};



	// ResourcesRegistry:
	//     - Handle reference and map all the Resources in the Resource manager.
	class ResourcesRegistry
	{
	//private:
	public:	
		// List of all the resroucs that exist, loaded or not.
		std::vector<ResourceData> resources;

		// Map a relative path to their Resource data.
		std::unordered_map<std::string, uint32_t> resourcePathMap;

		// Map a resource pointer to their Resource data.
		std::unordered_map<IResource*, uint32_t> resourceMap;

	public:
		// Add new loaded Resource.
		void AddResource(const std::string& path, const ResourceHeaderInfo& info, Ptr<IResource> newResource);

		// Remove loaded Resource, this will only remove the loaded Resource not its mapping.
		void RemoveResource(const std::string& path);

		// Remove loaded Resource, this will only remove the loaded Resource not its mapping.
		void RemoveResource(IResource* rsc);

		// Clear all loaded Resources, this will leave the mapping.
		void Reset();

		// Find a resrouce from path.
		const ResourceData* FindResource(const std::string& path) const;

		// Clean Resrouce Path.
		std::string CleanRscPath(const std::string& path) const;

		// Return a resrouce using at index.
		const ResourceData* GetResource(uint32_t index) const;

	};




	// Resource Manager:
	//   - The resource manager module takes care of providing the resources consumed by the
	//     game engine, making sure that only a single copy of any data is ever created/loaded.
	//
	//   - It handles the importing, loading and unloading the resources.
	//
	class ResourceManager : public IModule
	{
		// Delete copy constructor and assignment operator
		NOCOPYABLE(ResourceManager);

		// ILoader is a friend of the resource manager, it can add to the resource register on load
		friend class ILoader; 

	public:
		// Construct.
		ResourceManager();

		// Destruct.
		virtual ~ResourceManager();

	
		// inherited from IModule, must be overridden
		virtual void Initialize() override;
		virtual void Destroy() override;

		// Return Module Type.
		static EModuleType GetModuleType() { return EModuleType::MT_ResourceManager; }


		// --- -- - --- -- - --- -- - --- -- - --- -- - --- 
		//               Pending Save Resources
		// --- -- - --- -- - --- -- - --- -- - --- -- - ---

		// Add a resource that is edited and not saved.
		void AddPendingSave(Ptr<IResource> resource);

		// Remove resource that was added to the pending save list.
		void RemovePendingSave(Ptr<IResource> resource);

		// Is resource in the pending save list.
		bool IsPendingSave(Ptr<IResource> resource);

		// Save all edited pending resources.
		void SavePending();

		// Has resources edited pending resources.
		inline uint32_t GetNumPendingSave() { return (uint32_t)pendingSaveRsc.size(); }



		// --- -- - --- -- - --- -- - --- -- - --- -- - --- 
		//                 Obtain resources
		// --- -- - --- -- - --- -- - --- -- - --- -- - ---

		// Find a Resource and 
		template<class TResource,
			std::enable_if_t< std::is_base_of<IResource, TResource>::value, bool > = true >
		Ptr<TResource> GetResource(const std::string& path);

		// Return true if the Resource exist in the Resource registry whether loaded or not.
		bool HasResource(const std::string& path);
		bool HasResource(Ptr<IResource> rsc);

		// Return true if the Resource exist in the registry and loaded.
		bool IsResourceLoaded(const std::string& path);

		// Find or Load the resrouce ResourceRef is currently referencing.
		Ptr<IResource> FindOrLoad(const ResourceRef& ref);

		// Return the type of the resource at a relative resource path.
		EResourceType GetResourceType(const std::string& path);

		// Unload a resource by removing it from registry. if the resoucce is referenced else where
		// then it will stay alive until no one is referencing it.
		void UnloadResource(Ptr<IResource> rsc);

		// --- -- - --- -- - --- -- - --- -- - --- -- - --- 
		//                 Save/Import resources
		// --- -- - --- -- - --- -- - --- -- - --- -- - ---

		// Import new Resource from other file formats using our importers.
		// @param file: the file we want to import could be an image or .obj file etc...
		// @param optionalSaveDir: if not empty will save the Resource to this directory.
		// @return true if successfully imported.
		bool Import(const std::string& file, std::string optionalSaveDir = "");

		// Save a new resource and add it to be the Resource registry.
		// @param newResource: a new unsaved resource.
		// @param saveFile: a relative resource path.
		bool SaveNewResource(Ptr<IResource> newResource, const std::string& saveFile);

		// Save existing Resource.
		bool SaveResource(Ptr<IResource> rsc);

		// Add a new resrouce path to the resource registry
		bool AddResource(const std::string& path);

		// Return an importer of type TImporter.
		template<class TImporter>
		TImporter* GetImporter();

		// Return a vector containing all the extensions supported by importers
		std::vector<std::string> GetSupportedExtensions();

		// return true if the given extension is supported
		bool IsSupported(const std::string& extension);

		// --- -- - --- -- - --- -- - --- -- - --- -- - --- 
		//           Scan directory for resources
		// --- -- - --- -- - --- -- - --- -- - --- -- - ---

		void ScanDirectory(const std::string& path);

	private:
		// --- -- - --- -- - --- -- - --- -- - --- -- - --- 
		//             Importers & Loaders 
		// --- -- - --- -- - --- -- - --- -- - --- -- - --- 

		// Load a Resource of a specific type at a specific path
		bool LoadResource(const std::string& path, EResourceType type);

		// Return the loader of type TLoader
		template <class TLoader>
		TLoader* GetLoader();

		// Return the loader that can load this resource type.
		ILoader* GetLoader(EResourceType rscType);

		// Return the importer that can load a specific extension
		IImporter* GetImporter(const std::string& ext);

		// Load a resrouce using specific loader.
		bool LoadResource(ILoader* loader, const std::string& path);


	private:
		// Create & Register a new loader to the Resource manager.
		template<class TLoader,
			std::enable_if_t< std::is_base_of<ILoader, TLoader>::value, bool > = true >
		void RegisterLoader();

		// Create & Register a new importer to the Resource manager.
		template<class TImporter,
			std::enable_if_t< std::is_base_of<IImporter, TImporter>::value, bool > = true >
		void RegisterImporter();

		// List of all Resource loaders in the engine.
		std::vector< std::unique_ptr<ILoader> > loaders;

		// Map each loader to their type.
		std::unordered_map<ELoaderType, ILoader*> loadersMap;

		// Map each loader to the type of resources they can load.
		std::unordered_map<EResourceType, ILoader*> loadersRscMap;

		// List of all importers in the engine.
		std::vector< std::unique_ptr<IImporter> > importers;

		// Map each importer to their supported extensions.
		std::unordered_map<std::string, IImporter*> importersExtMap;

		// The Resource registry, use for mapping all Resources that can be loaded or already laoded.
		ResourcesRegistry registry;

		// Resources that are waiting to be saved.
		std::set< Ptr<IResource> > pendingSaveRsc;
	};







	// --- -- - --- -- - --- -- - --- -- - --- -- - --- -- - --- -- - ---   
	//		
	//                 Resource Manager Implementations
	//
	// --- -- - --- -- - --- -- - --- -- - --- -- - --- -- - --- -- - --- 


	template <class TLoader>
	TLoader* ResourceManager::GetLoader()
	{
		auto iter = loadersMap.find(TLoader::Type());

		// Not Found?
		if (iter == loadersMap.end())
		{
			RAVEN_ASSERT(0, "Load type not found, invalid loader type.");
			return nullptr;
		}

		return static_cast<TLoader*>(iter->second);
	}


	template<class TResource,
		std::enable_if_t< std::is_base_of<IResource, TResource>::value, bool > >
	Ptr<TResource> ResourceManager::GetResource(const std::string& path)
	{
		const ResourceData* rscData = registry.FindResource(path);

		// Doesn't Exist?
		if (!rscData)
		{
			LOGW("Error in GetResource, Resource Does not exit {0}", path.c_str());
			return nullptr;
		}

		// Type Mismatch?
		if (rscData->type != TResource::StaticGetType())
		{
			RAVEN_ASSERT(0, "Invalid Resrouce Type.");
			return nullptr;
		}

		// Not Loaded?
		if (!rscData->rsc)
		{
			if ( !LoadResource(rscData->path, rscData->type) )
			{
				RAVEN_ASSERT(0, "Failed to load resource.");
				return nullptr;
			}
		}

		return std::static_pointer_cast<TResource, IResource>(rscData->rsc);
	}


	template<class TLoader,
		std::enable_if_t< std::is_base_of<ILoader, TLoader>::value, bool > >
		void ResourceManager::RegisterLoader()
	{
		ILoader* newLoader = new TLoader();
		loaders.push_back( std::unique_ptr<ILoader>(newLoader) );
		loadersMap.insert(std::make_pair(TLoader::Type(), newLoader));

		std::vector<EResourceType> rscTypes;
		newLoader->ListResourceTypes(rscTypes);

		// Map each file extenions to their importer
		for (const auto& rt : rscTypes)
		{
			loadersRscMap.insert(std::make_pair(rt, newLoader));
		}
	}


	template<class TImporter,
		std::enable_if_t< std::is_base_of<IImporter, TImporter>::value, bool > >
		void ResourceManager::RegisterImporter()
	{
		IImporter* newImporter = new TImporter();
		importers.push_back( std::unique_ptr<IImporter>(newImporter) );

		std::vector<std::string> impExt;
		newImporter->ListExtensions(impExt);

		// Map each file extenions to their importer
		for (const auto& ext : impExt)
		{
			importersExtMap.insert( std::make_pair(ext, newImporter) );
		}
	}


	template<class TImporter>
	TImporter* ResourceManager::GetImporter()
	{
		// Find importer of type.
		for (auto& importer : importers)
		{
			if (importer->GetType() == TImporter::StaticGetType())
			{
				return static_cast<TImporter*>(importer.get());
			}
		}

		return nullptr;
	}


}
