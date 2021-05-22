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


#include "ResourceManager/Resources/IResource.h"
#include "ResourceManager/Resources/Texture.h"

#include "Utilities/Serialization.h"


#include <glm/glm.hpp>
#include <map>
#include <vector>



namespace Raven
{
	class MaterialShader;
	class RenderRscMaterial;


	// Dirty flag used to update the materail.
	enum class EMaterialDirtyFlag
	{
		// Not Dirty.
		None,

		// The material need a remapping.
		Remap,

		// The material data changed but no remapping needed.
		Update
	};




	// Material:
	//		- Manage material properties and how they are related to a material shader.
	//
	class Material : public IResource
	{
		NOCOPYABLE(Material);

	public:
		// Construct.
		Material();

		// Destruct.
		virtual ~Material();

		// Return the type.
		inline static EResourceType StaticGetType() noexcept { return EResourceType::RT_Material; }

		// Return the materail render resource.
		inline RenderRscMaterial* GetRenderRsc() const { return renderRsc; }

		// Set material paramters by name, Return the index of the paramter.
		int32_t SetTexture(const std::string& name, Ptr<ITexture> texture);
		int32_t SetScalar(const std::string& name, const float& scalar);
		int32_t SetColor(const std::string& name, const glm::vec4& color);

		// Get material paramters by name, return false if name doesn't exist.
		bool GetTexture(const std::string& name, Ptr<ITexture>& outTexture) const;
		bool GetScalar(const std::string& name, float& outScalar) const;
		bool GetColor(const std::string& name, glm::vec4& outColor) const;

		// Set the value of an existing material paramters using its assigned index.
		void SetTexture(int32_t idx, Ptr<ITexture> texture);
		void SetScalar(int32_t idx, const float& scalar);
		void SetColor(int32_t idx, const glm::vec4& color);

		// Return the index of an existing material paramters using its name.
		int32_t GetTextureIndex(const std::string& name) const;
		int32_t GetScalarIndex(const std::string& name) const;
		int32_t GetColorIndex(const std::string& name) const;

		// Return all paramters.
		inline const std::vector< std::pair<std::string, Ptr<ITexture>> >& GetTextures() const { return textures; }
		inline const std::vector< std::pair<std::string, float> >& GetScalars() const { return scalars; }
		inline const std::vector< std::pair<std::string, glm::vec4> >& GetColors() const { return colors; }

		// Return true if the materail is dirty and need update.
		inline bool IsDirty() { return dirtyFlag != EMaterialDirtyFlag::None; }

		// Load the materail on GPU.
		virtual void LoadRenderResource() override;

		// Update The Render Materail with the new paramters.
		virtual void UpdateRenderResource() override;

		// Set the shader this material reference.
		void SetMaterialShader(Ptr<MaterialShader> inShader);

		//  Reload the current shader.
		void ReloadShader();

		// Return the shader this material reference.
		MaterialShader* GetMaterialShader() { return shader.get(); }

		// Clear all paramters in the material.
		void ClearAllParamters();

		// Load paramters from shader and apply it.
		void LoadDefaulFromShader(const std::string& name);

	private:
		// Mark Dirty for update.
		void DirtyUpdate();

		// Mark Dirty for remap.
		void DirtyRemap();

		// Rebuild paramters used after loading from archive..
		void RebuildParamters();

		// -- -- --- ----- ----- --- --- - -- --- - 
		//       Load/Save Utils Fnctions.
		// -- -- --- ----- ----- --- --- - -- --- - 

		template<typename Archive>
		inline void SaveTextureParamters(Archive& archive) const
		{
			uint32_t count = textures.size();
			archive(count);

			for (uint32_t i = 0; i < count; ++i)
			{
				archive(textures[i].first);
				ResourceRef::Save(archive, textures[i].second.get()); // Save Reference To Texture.
			}
		}


		template<typename Archive>
		inline void LoadTextureParamters(Archive& archive)
		{
			uint32_t count = 0;
			archive(count);
			textures.resize(count);

			for (uint32_t i = 0; i < count; ++i)
			{
				std::string name;
				archive(textures[i].first);
				textures[i].second = ResourceRef::Load(archive).FindOrLoad<ITexture>(); // Load Reference To Texture.
			}
		}


		template<typename Archive>
		inline void SaveScalarParamters(Archive& archive) const
		{
			uint32_t count = scalars.size();
			archive(count);

			for (uint32_t i = 0; i < count; ++i)
			{
				archive(scalars[i].first);
				archive(scalars[i].second);
			}
		}


		template<typename Archive>
		inline void LoadScalarParamters(Archive& archive)
		{
			uint32_t count;
			archive(count);
			scalars.resize(count);

			for (uint32_t i = 0; i < count; ++i)
			{
				archive(scalars[i].first);
				archive(scalars[i].second);
			}
		}

		template<typename Archive>
		inline void SaveColorParamters(Archive& archive) const
		{
			uint32_t count = colors.size();
			archive(count);

			for (uint32_t i = 0; i < count; ++i)
			{
				archive(colors[i].first);
				archive(colors[i].second);
			}
		}


		template<typename Archive>
		inline void LoadColorParamters(Archive& archive)
		{
			uint32_t count = 0;
			archive(count);
			colors.resize(count);

			for (uint32_t i = 0; i < count; ++i)
			{
				archive(colors[i].first);
				archive(colors[i].second);
			}
		}

	public:
		// Serialization Save.
		template<typename Archive>
		void save(Archive& archive) const
		{
			archive(cereal::base_class<IResource>(this));

			// Save Resrouce Reference -> MaterialShader.
			ResourceRef::Save(archive, shader.get());

			// Svae Paramters...
			SaveTextureParamters(archive);
			SaveColorParamters(archive);
			SaveScalarParamters(archive);
		}

		// Serialization Load.
		template<typename Archive>
		void load(Archive& archive)
		{
			archive(cereal::base_class<IResource>(this));

			// Load Resrouce Reference -> MaterialShader.
			auto loadShader = ResourceRef::Load(archive).FindOrLoad<MaterialShader>();
			SetMaterialShader(loadShader);

			// Svae Paramters...
			LoadTextureParamters(archive);
			LoadColorParamters(archive);
			LoadScalarParamters(archive);

			RebuildParamters();
		}


	private:
		// The materail shader.
		Ptr<MaterialShader> shader;

		// The resrouce material.
		RenderRscMaterial* renderRsc;

		// Map paramters name to their value.
		std::map<std::string, int32_t> texturesMap;
		std::map<std::string, int32_t> scalarsMap;
		std::map<std::string, int32_t> colorsMap;

		// Texture material paramters
		std::vector< std::pair<std::string, Ptr<ITexture>> > textures;

		// Scalar material paramters
		std::vector< std::pair<std::string, float> > scalars;

		// Colors material paramters
		std::vector< std::pair<std::string, glm::vec4> > colors;

		// Flags used to update material with the render.
		EMaterialDirtyFlag dirtyFlag;

	};
}