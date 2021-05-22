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
#include "Render/RenderResource/Shader/RenderRscShader.h"


#include "Utilities/Serialization.h"



namespace Raven
{
	class RenderRscShader;
	class UniformBuffer;



	// Shader:
	//		- manage a render shader resource and provide a way to override its material functions.
	//
	class MaterialShader : public IResource
	{
		NOCOPYABLE(MaterialShader)

		friend class Material;

	public:
		// Construct.
		MaterialShader();

		// Destruct.
		virtual ~MaterialShader();

		// Return Resrouce Type.
		inline static EResourceType StaticGetType() noexcept { return EResourceType::RT_MaterialShader; }

		// Return the materail render resource.
		inline RenderRscShader* GetRenderRsc() { return renderRsc; }

		// Return the materail render resource for shadow calculation.
		inline RenderRscShader* GetShadowRenderRsc() { return renderShadowRsc; }

		// Set the shader name.
		void SetName(const std::string& name);

		// Set the material function.
		void SetMaterialFunction(const std::string& func, bool hasComputeVertex = false);

		// Get material function.
		inline const auto& GetMaterialFunction() const { return materialFunction; }

		// Get material function hasComputeVertex.
		inline const auto& HasComputeVertex() { return isComputeMaterialVertex; }

		// Return the block description, which the expected input of our material shader.
		inline RSInputBlockDescription& GetBlockInput() { return blockInput; }
		inline const RSInputBlockDescription& GetBlockInput() const { return blockInput; }

		// Add sampler description as expected input for this shader.
		void AddSampler(const std::string& name, ESInputDefaultFlag flag);

		// Return all samplers.
		inline auto& GetSamplers() { return samplers; }
		inline const auto& GetSamplers() const { return samplers; }

		// Get the start index of the samplers in the shader.
		inline auto& GetSamplersStartIndex() { return samplersStartIndex; }

		// Remove sampler.
		void RemoveSampler(int32_t index);

		// Set Shader Domain.
		void SetDomain(ERenderShaderDomain val);

		// Return the shader domain.
		inline ERenderShaderDomain GetDomain() const { return sdomain; }

		// Set Shader Type.
		void SetShaderType(ERenderShaderType val);

		// Return the shader type.
		inline ERenderShaderType GetShaderType() const { return stype; }

		// Return the shader unifrom buffer.
		inline UniformBuffer* GetUnifromBuffer() const { return materialUBO.get(); }

		// Load the shader on GPU.
		virtual void LoadRenderResource() override;

		// Update the render resource.
		virtual void UpdateRenderResource() override;

		// Does this shader contain the valid data to be loaded to GPU.
		bool HasValidData();

		// Force shadow shader making even if the material doesn't need to.
		void SetShadowShader(bool value);
		
		// The shader will be used to draw object without face culling.
		void SetTwoSided(bool value);

		// Serialization Save.
		template<typename Archive>
		void save(Archive& archive) const
		{
			archive(cereal::base_class<IResource>(this));

			archive(
				name,
				materialFunction,
				isComputeMaterialVertex,
				blockInput,
				EnumAsInt<const ERenderShaderDomain>(sdomain),
				EnumAsInt<const ERenderShaderType>(stype)
			);

			SaveVector(archive, samplers);
		}

		// Serialization Load.
		template<typename Archive>
		void load(Archive& archive)
		{
			archive(cereal::base_class<IResource>(this));

			archive(
				name,
				materialFunction,
				isComputeMaterialVertex,
				blockInput,
				EnumAsInt<ERenderShaderDomain>(sdomain),
				EnumAsInt<ERenderShaderType>(stype)
			);

			LoadVector(archive, samplers);
		}

	private:
		// Only for use by the Material.
		void AddRef(Material* inMat);
		void RemoveRef(Material* inMat);

		// Update all materials that reference this shader.
		void UpdateMaterials();

		// Create a shadow shader that represent this material.
		void CreateShadowShader(RenderRscShaderCreateData data);

	private:
		// The Shader Render Resrouce.
		RenderRscShader* renderRsc;

		// The Shade Name.
		std::string name;

		// Path to the material function to load.
		std::string materialFunction;

		// if true then we have material function for computing vertex.
		bool isComputeMaterialVertex;

		// The block description, which the expected input of our material shader.
		RSInputBlockDescription blockInput;

		// Texture input.
		std::vector<RSInputDescription> samplers;

		// The Shader Domain.
		ERenderShaderDomain sdomain;

		// The Shader Type.
		ERenderShaderType stype;

		// The uniform buffer for the material data.
		Ptr<UniformBuffer> materialUBO;

		// The materials that reference this shader.
		std::vector<Material*> materials;

		// The start index of samplers.
		int32_t samplersStartIndex;

		// Shadow shader for this material.
		RenderRscShader* renderShadowRsc;

		// Enable shader shader creation even if the materail doesn't need to.
		bool isMakeShadowShader;

		// if true the shader will be used to draw object without face culling.
		bool isTwoSided;
	};

}
