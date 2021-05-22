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





#include "Render/RenderResource/IRenderResource.h"
#include "RenderShaderInput.h"

#include "glm/vec4.hpp"
#include <memory>



namespace Raven
{
	class RenderRscShader;
	class ITexture;
	class Material;
	class UniformBuffer;


	// RenderRscMaterial:
	//			- Mangae the mapping between materail and the shader. maps all the paramters
	//				of the material to their shader input.
	class RenderRscMaterial : public IRenderResource
	{
	public:
		// Construct.
		RenderRscMaterial(RenderRscShader* inShader);

		// Destruct.
		~RenderRscMaterial();

		// Return the shader resrouce.
		inline RenderRscShader* GetShaderRsc() const { return shader; }

		// Return the custom shadow shader of this material if exist.
		inline RenderRscShader* GetShadowShaderRsc() const { return shadowShader; }

		// Set the custom shader of this material.
		inline void SetShadowShader(RenderRscShader* shader) { shadowShader = shader; }

		// Reload shader resrouce and its paramters mapping.
		void ReloadShader(RenderRscShader* inShader);

		// Return true if this resouce has a materail data.
		inline bool HasMaterialData() { return blockIndex != -1 && materialBuffer != nullptr; }

		// Find/Load input block from shader to map our materail paramters to.
		void LoadInputBlock(const std::string& inBlockName, int32_t inSamplersStartIndex);

		// Clear materail paramters mapping
		void ClearMapping();

		// Map Paramters.
		void MapParamter(const std::string& name, Ptr<ITexture>* texture);
		void MapParamter(const std::string& name, const float* scalar);
		void MapParamter(const std::string& name, const glm::vec4* color);

		// Fill the material buffer with our paramters values.
		void FillBuffer();

		// Return textures.
		const std::vector<Ptr<ITexture>*>& GetTextures() { return matInputTexturesMap; }

		// Set the unfirom buffer to update.
		inline void SetUniformBuffer(UniformBuffer* inUBO) { ubo = inUBO; }

		// Return the unfirom buffer this material update.
		inline UniformBuffer* GetUniformBuffer() {  return ubo; }

		// Update the uniform buffer with the materail data.
		void UpdateUniformBuffer();

		// Make Material Textures the current active ones.
		void MakeTexturesActive(const std::vector< Ptr<ITexture> >& defaultTextures);

	private:
		// The shader resource this material is.
		RenderRscShader* shader;

		// The custom shadow shader resource of this material.
		RenderRscShader* shadowShader;

		// The index of the input block in shader.
		int32_t blockIndex;

		// Buffer used to update the material unfirom buffer.
		uint8_t* materialBuffer;

		// Materail Input Mapping of a paramter
		struct MaterialInputMap
		{
			int32_t index;
			int32_t size;
			int32_t offset;
			const void* data;
		};

		// Material Input Paramters mapped to the unifrom block input.
		std::vector<MaterialInputMap> matInputMap;

		// Material Input Texture Paramters mapped to their shader input index.
		std::vector< Ptr<ITexture>* > matInputTexturesMap;

		// The unfirom buffer this material update.
		UniformBuffer* ubo;

	public:
		// Render Batch Use Only, index set by the render batch to performs fast mapping during batching.
		mutable uint32_t renderBatchIndex;

		// The material samplers start index in the shader.
		int32_t samplersStartIndex;
	};

}


