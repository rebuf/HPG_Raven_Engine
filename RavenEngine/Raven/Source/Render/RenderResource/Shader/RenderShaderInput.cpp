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
#include "RenderShaderInput.h"


#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/matrix.hpp"





#define COMMON_BLOCK_BINDING 1
#define TRANSFROM_BLOCK_BINDING 2
#define TRANSFROM_BONE_BLOCK_BINDING 3
#define LIGHTING_BLOCK_BINDING 4
#define MATERIAL_BLOCK_BINDING 5
#define TERRAIN_BIN_BLOCK_BINDING 6
#define SHADOW_BLOCK_BINDING 7








namespace Raven {




RSInputBlockDescription RenderShaderInput::CommonBlock = RSInputBlockDescription::MakeCommonBlock();
RSInputBlockDescription RenderShaderInput::TransformBlock = RSInputBlockDescription::MakeTransformBlock();
RSInputBlockDescription RenderShaderInput::TransformBoneBlock = RSInputBlockDescription::MakeTransformBoneBlock();
RSInputBlockDescription RenderShaderInput::LightingBlock_DEFERRED = RSInputBlockDescription::MakeLightingBlock_DEFERRED();
RSInputBlockDescription RenderShaderInput::LightingBlock_FORWARD = RSInputBlockDescription::MakeLightingBlock_FORWARD();
RSInputBlockDescription RenderShaderInput::TerrainBinBlock = RSInputBlockDescription::MakeTerrainBinBlock();
RSInputBlockDescription RenderShaderInput::ShadowBlock = RSInputBlockDescription::MakeShadowBlock();
RSInputBlockDescription RenderShaderInput::LightShadowBlock = RSInputBlockDescription::LightShadowBlock();


int32_t RenderShaderInput::MaterialBlockBinding = MATERIAL_BLOCK_BINDING;



// --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- 
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- 



void RSInputBlockDescription::BeginUniformBlock(const std::string& name)
{
	RAVEN_ASSERT(size == -1, "Uniform block already built.");
	this->name = name;
}


void RSInputBlockDescription::EndUniformBlock()
{
	RAVEN_ASSERT(inputs.size() != 0, "Empty blocks not allowed.");

	// Compute Block Size
	size = inputs.back().second;
	int32_t prevCount = inputs.back().first.count;
	EShaderInputType prevType = inputs.back().first.inputType;

	// Is Array?
	if (prevCount > 1)
	{
		int32_t baseSize;
		if (prevType == EShaderInputType::Mat3 || prevType == EShaderInputType::Mat4)
			baseSize = RenderShaderInput::GetSize(EShaderInputType::Mat4);
		else
			baseSize = RenderShaderInput::GetSize(EShaderInputType::Vec4);

		// Each element in the array is aligned with vec4.
		size += prevCount * baseSize;
	}
	else
	{
		size += RenderShaderInput::GetSize(prevType);
	}

	size = RenderShaderInput::AlignOffset(size, 16); // always align the final size with 16.
}


void RSInputBlockDescription::AddInput(EShaderInputType inputType, const std::string& inputName, ESInputDefaultFlag flag, int32_t offset)
{
	AddInputArray(inputType, inputName, 1, flag);
}


void RSInputBlockDescription::AddInputArray(EShaderInputType inputType, const std::string& inputName, int32_t count, ESInputDefaultFlag flag, int32_t offset)
{
	RAVEN_ASSERT(!name.empty(), "You must begin block to be able to add input.");

	// Offset not specified?
	if (offset == -1)
	{
		if (inputs.empty())
		{
			offset = 0;
		}
		else
		{
			// Compute Current Offset...
			int32_t size = inputs.back().second;
			int32_t prevCount = inputs.back().first.count;
			EShaderInputType prevType = inputs.back().first.inputType;

			// Is Array?
			if (prevCount > 1)
			{
				int32_t baseSize;
				if (prevType == EShaderInputType::Mat3 || prevType == EShaderInputType::Mat4)
					baseSize = RenderShaderInput::GetSize(EShaderInputType::Mat4);
				else
					baseSize = RenderShaderInput::GetSize(EShaderInputType::Vec4);

				// Each element in the array is aligned with vec4.
				size += prevCount * baseSize;
				offset = RenderShaderInput::AlignOffset(size, RenderShaderInput::GetAlignment(EShaderInputType::Vec4));
			}
			else
			{
				size += RenderShaderInput::GetSize(prevType);
				offset = RenderShaderInput::AlignOffset(size, RenderShaderInput::GetAlignment(inputType));
			}
		}
	}

	// New Input to the block
	std::pair<RSInputDescription, int32_t> blockInput;
	blockInput.first.uniformType = EShaderUniformType::UniformBlock;
	blockInput.first.inputType = inputType;
	blockInput.first.name = inputName;
	blockInput.first.count = count;
	blockInput.first.flag = flag;
	blockInput.second = offset;
	inputs.push_back(blockInput);
}


int32_t RSInputBlockDescription::GetInputIndex(const std::string& name)
{
	for (int32_t i = 0; i < (int32_t)inputs.size(); ++i)
	{
		if (inputs[i].first.name == name)
			return i;
	}

	return -1;
}


RSInputBlockDescription RSInputBlockDescription::MakeCommonBlock()
{
	RSInputBlockDescription inputblock;
	inputblock.binding = COMMON_BLOCK_BINDING;
	inputblock.BeginUniformBlock("CommonBlock");
	inputblock.AddInput(EShaderInputType::Mat4, "viewProjMatrix");
	inputblock.AddInput(EShaderInputType::Mat4, "viewProjMatrixInverse");
	inputblock.AddInput(EShaderInputType::Vec3, "viewDir");
	inputblock.AddInput(EShaderInputType::Vec3, "viewPos");
	inputblock.AddInput(EShaderInputType::Vec4, "viewport");
	inputblock.AddInput(EShaderInputType::Vec4, "sunDir");
	inputblock.AddInput(EShaderInputType::Vec4, "sunColorAndPower");
	inputblock.AddInput(EShaderInputType::Vec2, "nearFar");
	inputblock.AddInput(EShaderInputType::Float, "time");
	inputblock.EndUniformBlock();

	return inputblock;
}


RSInputBlockDescription RSInputBlockDescription::MakeTransformBlock()
{
	RSInputBlockDescription inputblock;
	inputblock.binding = TRANSFROM_BLOCK_BINDING;
	inputblock.BeginUniformBlock("TransformBlock");
	inputblock.AddInput(EShaderInputType::Mat4, "inModelMatrix");
	inputblock.AddInput(EShaderInputType::Mat4, "inNormalMatrix");
	inputblock.EndUniformBlock();

	return inputblock;
}


RSInputBlockDescription RSInputBlockDescription::MakeTransformBoneBlock()
{
	RSInputBlockDescription inputblock;
	inputblock.binding = TRANSFROM_BONE_BLOCK_BINDING;
	inputblock.BeginUniformBlock("TransformBoneBlock");
	inputblock.AddInput(EShaderInputType::Mat4, "inModelMatrix");
	inputblock.AddInput(EShaderInputType::Mat4, "inNormalMatrix");
	inputblock.AddInputArray(EShaderInputType::Mat4, "bones", RENDER_SKINNED_MAX_BONES);
	inputblock.EndUniformBlock();

	return inputblock;
}


RSInputBlockDescription RSInputBlockDescription::MakeLightingBlock_FORWARD()
{
	RSInputBlockDescription inputblock;
	inputblock.binding = LIGHTING_BLOCK_BINDING;
	inputblock.BeginUniformBlock("LightingBlock");
	inputblock.AddInputArray(EShaderInputType::Vec4, "lightDir", RENDER_PASS_FORWARD_MAX_LIGHTS);
	inputblock.AddInputArray(EShaderInputType::Vec4, "lightPos", RENDER_PASS_FORWARD_MAX_LIGHTS);
	inputblock.AddInputArray(EShaderInputType::Vec4, "lightPower", RENDER_PASS_FORWARD_MAX_LIGHTS);
	inputblock.AddInputArray(EShaderInputType::Vec4, "lightData", RENDER_PASS_FORWARD_MAX_LIGHTS);
	inputblock.EndUniformBlock();

	return inputblock;
}


RSInputBlockDescription RSInputBlockDescription::MakeLightingBlock_DEFERRED()
{
	RSInputBlockDescription inputblock;
	inputblock.binding = LIGHTING_BLOCK_BINDING;
	inputblock.BeginUniformBlock("LightingBlock");
	inputblock.AddInputArray(EShaderInputType::Vec4, "lightDir", RENDER_PASS_DEFERRED_MAX_LIGHTS);
	inputblock.AddInputArray(EShaderInputType::Vec4, "lightPos", RENDER_PASS_DEFERRED_MAX_LIGHTS);
	inputblock.AddInputArray(EShaderInputType::Vec4, "lightPower", RENDER_PASS_DEFERRED_MAX_LIGHTS);
	inputblock.AddInputArray(EShaderInputType::Vec4, "lightData", RENDER_PASS_DEFERRED_MAX_LIGHTS);
	inputblock.EndUniformBlock();

	return inputblock;
}


RSInputBlockDescription RSInputBlockDescription::MakeTerrainBinBlock()
{
	RSInputBlockDescription inputblock;
	inputblock.binding = TERRAIN_BIN_BLOCK_BINDING;
	inputblock.BeginUniformBlock("TerrainBinBlock");
	inputblock.AddInput(EShaderInputType::Vec2, "inScale");
	inputblock.AddInput(EShaderInputType::Vec2, "inHeight");
	inputblock.AddInput(EShaderInputType::Vec2, "inOffset");
	inputblock.AddInput(EShaderInputType::Vec2, "inUVScale");
	inputblock.EndUniformBlock();

	return inputblock;
}


RSInputBlockDescription RSInputBlockDescription::MakeShadowBlock()
{
	RSInputBlockDescription inputblock;
	inputblock.binding = SHADOW_BLOCK_BINDING;
	inputblock.BeginUniformBlock("ShadowBlock");
	inputblock.AddInput(EShaderInputType::Mat4, "inShadowViewProj");
	inputblock.EndUniformBlock();

	return inputblock;
}


RSInputBlockDescription RSInputBlockDescription::LightShadowBlock()
{
	RSInputBlockDescription inputblock;
	inputblock.binding = SHADOW_BLOCK_BINDING;
	inputblock.BeginUniformBlock("LightShadowBlock");
	inputblock.AddInputArray(EShaderInputType::Mat4, "sunViewProj", RENDER_MAX_SHADOW_CASCADE);
	inputblock.AddInputArray(EShaderInputType::Float, "cascadeRanges", RENDER_MAX_SHADOW_CASCADE+1);
	inputblock.EndUniformBlock();

	return inputblock;
}



// --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- 
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- 





RenderShaderInput::RenderShaderInput()
{

}


RenderShaderInput::~RenderShaderInput()
{

}


void RenderShaderInput::AddInput(EShaderInputType inputType, const std::string& name, ESInputDefaultFlag flag)
{
	// Find index of an input with the same name.
	int32_t idx = GetInputIndex(name);

	// Exist?
	if (idx != -1)
	{
		// Override.
		inputs[idx].uniformType = EShaderUniformType::Uniform;
		inputs[idx].inputType = inputType;
		inputs[idx].flag = flag;
	}
	else
	{
		// Add New
		RSInputDescription newInput;
		newInput.uniformType = EShaderUniformType::Uniform;
		newInput.inputType = inputType;
		newInput.name = name;
		newInput.flag = flag;
		newInput.count = 1;
		inputs.push_back(newInput);
	}
}


void RenderShaderInput::AddSamplerInput(const std::string& name, ESInputDefaultFlag flag)
{
	// Find index of an input with the same name.
	int32_t idx = GetSamplerInputIndex(name);

	// Exist?
	if (idx != -1)
	{
		// Override.
		samplers[idx].uniformType = EShaderUniformType::Sampler;
		samplers[idx].inputType = EShaderInputType::Int;
		samplers[idx].flag = flag;
	}
	else
	{
		// Add New
		RSInputDescription newInput;
		newInput.uniformType = EShaderUniformType::Sampler;
		newInput.inputType = EShaderInputType::Int;
		newInput.name = name;
		newInput.flag = flag;
		samplers.push_back(newInput);
	}
}


void RenderShaderInput::AddSamplerInputs(const std::vector<RSInputDescription>& inSamplers)
{
	for (const auto& s : inSamplers)
	{
		AddSamplerInput(s.name, s.flag);
	}
}


void RenderShaderInput::AddBlockInput(const RSInputBlockDescription& block)
{
	RAVEN_ASSERT(block.size != -1, "Invalid Block.");

	// Find index of an input block with the same name.
	int32_t idx = GetBlockInputIndex(block.name);

	// Exist?
	if (idx != -1)
	{
		blockInputs[idx] = block; // Override.
	}
	else
	{
		// Add New
		blockInputs.push_back(block);
	}
}


int32_t RenderShaderInput::GetInputIndex(const std::string& name)
{
	for (int32_t i = 0; i < (int32_t)inputs.size(); ++i)
	{
		if (inputs[i].name == name)
			return i;
	}

	return -1;
}


int32_t RenderShaderInput::GetSamplerInputIndex(const std::string& name)
{
	for (int32_t i = 0; i < (int32_t)samplers.size(); ++i)
	{
		if (samplers[i].name == name)
			return i;
	}

	return -1;
}


int32_t RenderShaderInput::GetBlockInputIndex(const std::string& name)
{
	for (int32_t i = 0; i < (int32_t)blockInputs.size(); ++i)
	{
		if (blockInputs[i].name == name)
			return i;
	}

	return -1;
}


int32_t RenderShaderInput::GetSize(EShaderInputType type)
{
	switch (type)
	{
	case Raven::EShaderInputType::Float: return sizeof(float);
	case Raven::EShaderInputType::Int:   return sizeof(int32_t);
	case Raven::EShaderInputType::Vec2:  return sizeof(glm::vec2);
	case Raven::EShaderInputType::Vec3:  return sizeof(glm::vec3);
	case Raven::EShaderInputType::Vec4:  return sizeof(glm::vec4);
	case Raven::EShaderInputType::Mat3:  return sizeof(glm::mat3);
	case Raven::EShaderInputType::Mat4:  return sizeof(glm::mat4);
	}

	return -1;
}


int32_t RenderShaderInput::GetAlignment(EShaderInputType type)
{
	switch (type)
	{
	case Raven::EShaderInputType::Float: return 4;
	case Raven::EShaderInputType::Int:   return 4;
	case Raven::EShaderInputType::Vec2:  return 8;

	case Raven::EShaderInputType::Vec3:
	case Raven::EShaderInputType::Vec4:
		return 16;

	// Each column align with 16.
	case Raven::EShaderInputType::Mat3: 
	case Raven::EShaderInputType::Mat4:
		return 16;
	}

	RAVEN_ASSERT(0, "GetAlignment() - Shader Input Type Not Supported.");
	return -1;
}


int32_t RenderShaderInput::AlignOffset(int32_t offset, int32_t align)
{
	int32_t m = (offset % align);

	// Aligned?
	if (m == 0)
		return offset;

	return offset + (align - m);
}



} // End of namespace Raven.


