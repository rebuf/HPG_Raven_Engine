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

#include <IconsMaterialDesignIcons.h>

#include "cereal/archives/json.hpp"
#include "cereal/archives/binary.hpp"
#include "cereal/types/polymorphic.hpp"




#define INVALID_RSC_INDEX static_cast<uint32_t>(-1)




namespace Raven
{
	// Types of all the supported resource by the engine.
	enum EResourceType
	{
		// Invalid resource.
		RT_None = -1,

		// Resources - The Value of each enum resource is important and should not change, please add 
		//             new entries but keep the original reserved as it will affect the archiving system.
		RT_Texture2D             = 0,
		RT_TextureCube           = 1,
		RT_DynamicTexture        = 2,
		RT_MaterialShader        = 3,
		RT_Material              = 4,
		RT_Mesh                  = 5,
		RT_SkinnedMesh           = 6,
		RT_Terrain               = 7,
		RT_Scene                 = 8,
		RT_GuiLayout             = 9,
		RT_Skeleton              = 10,
		RT_AnimationClip         = 11,
		RT_AnimationController   = 12
	};


	// Convert resource type to its name as string.
	inline std::string ResourceToString(EResourceType type)
	{
		switch (type)
		{
		// Textures.
		case RT_Texture2D: return "Texture2D";
		case RT_TextureCube: return "TextureCube";
		case RT_DynamicTexture: return "DynamicTexture";

		// Materials.
		case RT_MaterialShader: return "MaterialShader";
		case RT_Material: return "Material";

		// Meshes.
		case RT_Mesh: return "Mesh";
		case RT_SkinnedMesh: return "SkinnedMesh";
		case RT_Terrain: return "Terrain";
		case RT_Skeleton: return "Skeleton";

		// Scene.
		case RT_Scene: return "scene";

		// GUI.
		case RT_GuiLayout: return "GuiLayout";

		// Animation.
		case RT_AnimationClip: return "AnimationClip";
		case RT_AnimationController: return "AnimationController";

		default: return "Invalid resource Type.";
		}

		RAVEN_ASSERT(0, "Invalid resource Type.");
	}

	inline char* ResourceToIcon(EResourceType type)
	{
		switch (type)
		{
			// Textures.
		case RT_Texture2D: return ICON_MDI_TEXTURE;
		case RT_TextureCube: return ICON_MDI_TEXTURE;
		case RT_DynamicTexture: return ICON_MDI_TEXTURE;

			// Materials.
		case RT_MaterialShader: return ICON_MDI_BRUSH;
		case RT_Material: return ICON_MDI_BRUSH;

			// Meshes.
		case RT_Mesh: return ICON_MDI_SHAPE;
		case RT_SkinnedMesh: return ICON_MDI_SHAPE_PLUS;
		case RT_Terrain: return ICON_MDI_TERRAIN;
		case RT_Skeleton: return ICON_MDI_BONE;

			// Scene.
		case RT_Scene: return ICON_MDI_GLOBE_MODEL;

			// GUI.
		case RT_GuiLayout: return ICON_MDI_TABLE;

			// Animation.
		case RT_AnimationClip: return ICON_MDI_ANIMATION;
		case RT_AnimationController: return ICON_MDI_ANIMATION_PLAY;

		}

		RAVEN_ASSERT(0, "Invalid resource Type.");
	}

	// IResource:
	//    - The base resource class, pointers of which are stored in the resource register
	//
	//    - The IResource class serves as the base class for all Resources that can go in the resource  
	//			register in the ResourceManager module.Each derived resource is identifiable by an enum value.
	//
	class IResource
	{
		friend class ResourceManager;
		friend class ResourceRef;
		friend class ILoader;
		friend class ResourcesRegistry;

	public:
		// Default Construct.
		IResource() 
			: type(EResourceType::RT_None)
			, hasRenderResources(false)
			, isOnGPU(false) 
			, load_version(0)
			, resourceIndex(INVALID_RSC_INDEX)
		{

		}

		// Destruct.
		virtual ~IResource()
		{

		}

		// Return the resource type.
		inline EResourceType GetType() const noexcept { return type; }

		// Return true if the resource contain render data that needs to be loaded on GPU.
		inline bool HasRenderResources() const noexcept { return hasRenderResources; }

		// Return true if the is loaded on GPU.
		inline bool IsOnGPU() const noexcept { return isOnGPU; }

		// Load the render resource. Note: only for resources with render data.
		virtual void LoadRenderResource() { RAVEN_ASSERT(0, "Invalid Operation."); }

		// Update the render resource. Note: only for resources with render data.
		virtual void UpdateRenderResource() { RAVEN_ASSERT(0, "Invalid Operation."); }

		// Set the resource name.
		inline void SetName(const std::string& newName) { name = newName; }

		// Return the resource name.
		inline const std::string& GetName() const { return name; }

		// If the resource exist on disk, return the path it was saved at.
		inline const std::string& GetResourcePath() const { return path; }

		// Serialization Save.
		template<typename Archive>
		void save(Archive& archive) const
		{
			archive(name);
		}

		// Serialization Load.
		template<typename Archive>
		void load(Archive& archive)
		{
			archive(name);
		}


	protected:
		// The resource type.
		EResourceType type;

		// If the resource contain/need render resources.
		bool hasRenderResources;

		// Is the resource loaded on GPU.
		bool isOnGPU;

		// The version of the resource when loaded.
		uint32_t load_version;

	private:
		// The name of the resource.
		std::string name;

		// The path to the resource on disk.
		std::string path;

		// Index of the resources in the resource registry.
		uint32_t resourceIndex;
	};




	// ResourceRef:
	//    - Used to reference a resource for archiving operations.
	//
	class ResourceRef
	{
		// Friend...
		friend class ResourceManager;

	private:
		// The Resource relative path.
		std::string path;

		// The type of the Resource.
		EResourceType type;

		// weak pointer to the resource, used for caching.
		WeakPtr<IResource> rsc;

	public:
		// Default Construct - Invalid Reference.
		ResourceRef()
			: type(EResourceType::RT_None)
		{

		}

		// Construct.
		ResourceRef(IResource* resource)
			: path(resource->path)
			, type(resource->type)
		{

		}

		// Copy Construct.
		ResourceRef(const ResourceRef& other)
			: path(other.path)
			, type(other.type)
			, rsc(other.rsc)
		{

		}

		// Copy Assignment
		ResourceRef& operator=(const ResourceRef& other)
		{
			path = other.path;
			type = other.type;
			rsc = other.rsc;
			return *this;
		}

		// Create ResourceRef from input archive.
		template<typename Archive>
		static inline void Save(Archive& archive, IResource* resource)
		{
			if (resource)
			{
				const ResourceRef ref(resource);
				archive(EnumAsInt<const EResourceType>(ref.type), ref.path);
			}
			else
			{
				const ResourceRef invalidRef;
				archive(EnumAsInt<const EResourceType>(invalidRef.type), invalidRef.path);
			}
		}


		// Create ResourceRef from input archive.
		template<typename Archive>
		static inline ResourceRef Load(Archive& archive)
		{
			ResourceRef ref;
			archive(EnumAsInt<EResourceType>(ref.type), ref.path);

			return ref;
		}


		// Serialize the reference it self.
		template<typename Archive>
		void save(Archive& archive) const
		{
			archive(EnumAsInt<const EResourceType>(type), path);
		}

		// Serialize the reference it self.
		template<typename Archive>
		void load(Archive& archive)
		{
			archive(EnumAsInt<EResourceType>(type), path);
		}


		// Return the relative path to the Resource.
		inline const std::string& GetPath() const { return path; }

		// Return if the reference is valid, and try to reference a resource.
		bool IsValid() const { return type != EResourceType::RT_None; }

		// Return the cached resource as raw pointer, this will not try to load the resource.
		// will return null if invalid or not loaded.
		template<class TResource>
		TResource* GetWeak() const 
		{ 
			return static_cast<TResource*>(rsc.expired() ? nullptr : rsc.lock().get());
		}

		// Find or load the Resource. 
		Ptr<IResource> FindOrLoad();

		// Template FindOrLoad.
		template<class TResource>
		Ptr<TResource> FindOrLoad() 
		{
			Ptr<IResource> rscPtr = rsc.lock();

			// Invalid cache?
			if (!rscPtr)
			{
				rscPtr = FindOrLoad(); // Find or Load in the resource registry.
			}

			return std::static_pointer_cast<TResource, IResource>(rscPtr);
		}

	};


}