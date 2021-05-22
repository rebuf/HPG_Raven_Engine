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
#include "RenderRscMaterial.h"
#include "RenderRscShader.h"
#include "UniformBuffer.h"

#include "Render/RenderResource/RenderRscTexture.h"
#include "Render/OpenGL/GLTexture.h"

#include "ResourceManager/Resources/Material.h"
#include "ResourceManager/Resources/Texture2D.h"



#include "glm/gtc/type_ptr.hpp"




namespace Raven {




RenderRscMaterial::RenderRscMaterial(RenderRscShader* inShader)
	: shader(inShader)
	, materialBuffer(nullptr)
	, blockIndex(-1)
	, ubo(nullptr)
	, renderBatchIndex((uint32_t)-1)
	, samplersStartIndex(-1)
	, shadowShader(nullptr)
{

}


RenderRscMaterial::~RenderRscMaterial()
{
	delete materialBuffer;
}


void RenderRscMaterial::ReloadShader(RenderRscShader* inShader)
{
	if (shader)
	{
		ClearMapping();
		LoadInputBlock("", -1);
	}

	shader = inShader;
}


void RenderRscMaterial::LoadInputBlock(const std::string& inBlockName, int32_t inSamplersStartIndex)
{
	RAVEN_ASSERT(shader != nullptr, "Updating Render Material with invalid shader.");
		
	// Find Block
	blockIndex = shader->GetInput().GetBlockInputIndex(inBlockName);

	// Delete materail buffer that match the old block.
	if (materialBuffer)
	{
		delete materialBuffer;
		materialBuffer = nullptr;
	}


	// Default Textures.
	samplersStartIndex = inSamplersStartIndex;

	if (samplersStartIndex != -1)
	{
		uint32_t materialSamplers = shader->GetInput().GetNumSamplers() - samplersStartIndex;
		matInputTexturesMap.resize(materialSamplers);
	}

}


void RenderRscMaterial::ClearMapping()
{
	matInputMap.clear();

	for (auto& tex : matInputTexturesMap)
	{
		tex = nullptr;
	}
}


void RenderRscMaterial::MapParamter(const std::string& name, Ptr<ITexture>* texture)
{
	int32_t idx = shader->GetInput().GetSamplerInputIndex(name);

	// No input with that name?
	if (idx == -1 || samplersStartIndex == -1)
		return;

	matInputTexturesMap[idx - samplersStartIndex] = texture;
}


void RenderRscMaterial::MapParamter(const std::string& name, const float* scalar)
{
	// No Input Block?
	if (blockIndex == -1)
		return;

	int32_t idx = shader->GetInput().GetBlockInput(blockIndex).GetInputIndex(name);

	// No input with that name?
	if (idx == -1)
		return;

	const auto& blockInput = shader->GetInput().GetBlockInput(blockIndex).inputs[idx];

	// Not Scalar Type?
	if (blockInput.first.inputType != EShaderInputType::Float)
		return;

	MaterialInputMap inputScalar{};
	inputScalar.index = idx;
	inputScalar.offset = blockInput.second; // The Offset in the uniform block.
	inputScalar.size = sizeof(float);
	inputScalar.data = scalar;
	matInputMap.push_back(inputScalar);
}


void RenderRscMaterial::MapParamter(const std::string& name, const glm::vec4* color)
{
	// No Input Block?
	if (blockIndex == -1)
		return;

	int32_t idx = shader->GetInput().GetBlockInput(blockIndex).GetInputIndex(name);

	// No input with that name?
	if (idx == -1)
		return;

	const auto& blockInput = shader->GetInput().GetBlockInput(blockIndex).inputs[idx];

	// Not Scalar Type?
	if (blockInput.first.inputType != EShaderInputType::Vec4)
		return;

	MaterialInputMap inputScalar{};
	inputScalar.index = idx;
	inputScalar.offset = blockInput.second; // The Offset in the uniform block.
	inputScalar.size = sizeof(glm::vec4);
	inputScalar.data = glm::value_ptr(*color);
	matInputMap.push_back(inputScalar);
}


void RenderRscMaterial::FillBuffer()
{
	// No Input Block?
	if (blockIndex == -1)
		return;

	const auto& blockInput = shader->GetInput().GetBlockInput(blockIndex);

	// Invalid?
	if (!materialBuffer)
	{
		// Allocate...
		materialBuffer = new uint8_t[ blockInput.size ];
	}

	// Missing Paramters?
	if (matInputMap.size() != blockInput.inputs.size())
	{
		// Set them to default.
		for (const auto& input : blockInput.inputs)
		{
			if (input.first.inputType == EShaderInputType::Float)
			{
				float def = input.first.flag == ESInputDefaultFlag::Black ? 0.0f : 1.0f;
				memcpy(materialBuffer + input.second, &def, sizeof(float));
			}
			else
			{
				glm::vec4 color = input.first.flag == ESInputDefaultFlag::Black ? glm::vec4(0.0f) : glm::vec4(1.0f);
				memcpy(materialBuffer + input.second, glm::value_ptr(color), sizeof(glm::vec4));
			}
		}
	}

	// Fill the buffer with materail data.
	for (const auto& inputParam : matInputMap)
	{
		memcpy(materialBuffer + inputParam.offset, inputParam.data, inputParam.size);
	}

}


void RenderRscMaterial::UpdateUniformBuffer()
{
	// No Input Block? No Unifrom Buffer?
	if (blockIndex == -1 && ubo == nullptr)
		return;

	const auto& blockInput = shader->GetInput().GetBlockInput(blockIndex);

	// Update the unifrom buffer with the material data.
	ubo->UpdateData(blockInput.size, 0, materialBuffer);
}


void RenderRscMaterial::MakeTexturesActive(const std::vector< Ptr<ITexture> >& defaultTextures)
{
	if (samplersStartIndex == -1)
		return;

	for (size_t i = 0; i < matInputTexturesMap.size(); ++i)
	{
		auto inputTex = matInputTexturesMap[i];
		int32_t samplerIndex = i + samplersStartIndex;

		if (inputTex)
		{
			RenderRscTexture* rtex = (*inputTex)->GetRenderRsc();
			rtex->GetTexture()->Active(samplerIndex);
		}
		else
		{
			if (samplerIndex >= shader->GetInput().GetNumSamplers())
				continue;

			const auto& samplerInput = shader->GetInput().GetSamplerInput(samplerIndex);
			auto defaultTex = defaultTextures[static_cast<int32_t>(samplerInput.flag)];

			RenderRscTexture* rtex = defaultTex->GetRenderRsc();
			rtex->GetTexture()->Active(samplerIndex);
		}


	}
}

} // End of namespace Raven.
