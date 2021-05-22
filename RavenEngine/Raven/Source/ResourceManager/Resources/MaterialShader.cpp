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



#include "MaterialShader.h"
#include "Material.h"
#include "Render/RenderResource/Shader/UniformBuffer.h"



namespace Raven {




MaterialShader::MaterialShader()
	: IResource()
	, renderRsc(nullptr)
	, renderShadowRsc(nullptr)
	, sdomain(ERenderShaderDomain::Mesh)
	, stype(ERenderShaderType::Opaque)
	, isComputeMaterialVertex(false)
	, samplersStartIndex(-1)
	, isMakeShadowShader(false)
	, isTwoSided(false)
{
	type = MaterialShader::StaticGetType();
	hasRenderResources = true;
}


MaterialShader::~MaterialShader()
{
	delete renderRsc;
	delete renderShadowRsc;
}



void MaterialShader::LoadRenderResource()
{
	RAVEN_ASSERT(!isOnGPU, "Shader already loaded.");

	if (!HasValidData())
		return;

	// What Stages this materail function support.
	EGLShaderStageBit stages = EGLShaderStageBit::FragmentBit;

	// Has comute vertex material function?
	if (isComputeMaterialVertex)
	{
		stages = stages | EGLShaderStageBit::VertexBit;
	}

	RenderRscShaderCreateData rscData;
	rscData.name = name;
	rscData.type = stype;
	rscData.AddFunction(stages, materialFunction);
	renderRsc = RenderRscShader::Create(sdomain, rscData); // Build Shader

	// Failed to build shader
	if (!renderRsc)
		return;

	// Shader Input...
	if (blockInput.size != -1)
	{
		blockInput.binding = RenderShaderInput::MaterialBlockBinding;
		renderRsc->GetInput().AddBlockInput(blockInput);

		// Create a unifrom buffer for the block input.
		materialUBO = Ptr<UniformBuffer>(UniformBuffer::Create(blockInput, false));
	}


	samplersStartIndex = renderRsc->GetInput().GetNumSamplers();
	renderRsc->GetInput().AddSamplerInputs(samplers);
	renderRsc->BindBlockInputs();
	renderRsc->BindSamplers();

	isOnGPU = true;
	UpdateMaterials();
	CreateShadowShader(rscData);
}


void MaterialShader::UpdateRenderResource()
{
	RAVEN_ASSERT(isOnGPU, "");

	if (!HasValidData())
		return;

	delete renderShadowRsc;
	delete renderRsc;
	isOnGPU = false;
	LoadRenderResource();
}


bool MaterialShader::HasValidData()
{
	return !materialFunction.empty();
}


void MaterialShader::SetShadowShader(bool value)
{
	isMakeShadowShader = value;
}


void MaterialShader::SetTwoSided(bool value)
{
	isTwoSided = value;
}


void MaterialShader::SetName(const std::string& name)
{
	this->name = name;
}


void MaterialShader::SetMaterialFunction(const std::string& func, bool hasComputeVertex)
{
	materialFunction = func;
	isComputeMaterialVertex = hasComputeVertex;
}


void MaterialShader::SetDomain(ERenderShaderDomain val)
{
	RAVEN_ASSERT(val != ERenderShaderDomain::Custom, "Custom Domain not supported by shader resource.");
	sdomain = val;
}


void MaterialShader::SetShaderType(ERenderShaderType val)
{
	stype = val;
}


void MaterialShader::AddSampler(const std::string& name, ESInputDefaultFlag flag)
{
	RSInputDescription inputSampler;
	inputSampler.uniformType = EShaderUniformType::Sampler;
	inputSampler.inputType = EShaderInputType::Int;
	inputSampler.name = name;
	inputSampler.flag = flag;
	samplers.push_back(inputSampler);
}


void MaterialShader::RemoveSampler(int32_t index)
{
	if (index >= 0 && index < samplers.size())
	{
		auto iter = samplers.begin() + index;
		samplers.erase(iter);
	}
}


void MaterialShader::AddRef(Material* inMat)
{
	// Exist?
	auto iter = std::find(materials.begin(), materials.end(), inMat);
	if (iter != materials.end())
		return;

	materials.push_back(inMat);
}


void MaterialShader::RemoveRef(Material* inMat)
{
	// Exist?
	auto iter = std::find(materials.begin(), materials.end(), inMat);
	if (iter == materials.end())
	{
		// Remove.
		materials.erase(iter);
	}

}


void MaterialShader::UpdateMaterials()
{
	for (auto mat : materials)
	{
		bool wasLoaded = mat->IsOnGPU();
		mat->ReloadShader();
	}
}


void MaterialShader::CreateShadowShader(RenderRscShaderCreateData data)
{
	if (!isMakeShadowShader)
	{
		// Is custom shadow shader needed?
		if ( stype   != ERenderShaderType::Masked
			&& stype   != ERenderShaderType::MaskedFoliage
			&& sdomain != ERenderShaderDomain::MeshInstance
			&& !isComputeMaterialVertex )
			return;
	}


	data.isShadow = true;
	data.name.append("_Shadow");
	renderShadowRsc = RenderRscShader::Create(sdomain, data); // Build Shader

	if (blockInput.binding != -1)
		renderRsc->GetInput().AddBlockInput(blockInput);

	renderRsc->GetInput().AddSamplerInputs(samplers);
	renderShadowRsc->BindBlockInputs();
	renderShadowRsc->BindSamplers();
}



} // End of namespace Raven.
