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
#include "Material.h"
#include "ResourceManager/Resources/Texture2D.h"
#include "ResourceManager/Resources/MaterialShader.h"
#include "Render/RenderResource/Shader/RenderRscMaterial.h"




namespace Raven
{

Material::Material()
	: IResource()
	, renderRsc(nullptr)
	, dirtyFlag(EMaterialDirtyFlag::None)
{
	type = Material::StaticGetType();
	hasRenderResources = true;
}


Material::~Material()
{
	delete renderRsc;
}


int32_t Material::SetTexture(const std::string& name, Ptr<ITexture> texture)
{
	int32_t idx = GetTextureIndex(name);

	// New?
	if (idx == -1)
	{
		idx = static_cast<int32_t>(textures.size());
		textures.push_back(std::make_pair(name, texture));
		texturesMap.insert(std::make_pair(name, idx));
		DirtyRemap();
	}
	else
	{
		textures[idx].second = texture;
		DirtyUpdate();
	}


	return idx;
}


int32_t Material::SetScalar(const std::string& name, const float& scalar)
{
	int32_t idx = GetScalarIndex(name);

	// New?
	if (idx == -1)
	{
		idx = static_cast<int32_t>(scalars.size());
		scalars.push_back(std::make_pair(name, scalar));
		scalarsMap.insert(std::make_pair(name, idx));
		DirtyRemap();
	}
	else
	{
		scalars[idx].second = scalar;
		DirtyUpdate();
	}


	return idx;
}


int32_t Material::SetColor(const std::string& name, const glm::vec4& color)
{
	int32_t idx = GetColorIndex(name);

	// New?
	if (idx == -1)
	{
		idx = static_cast<int32_t>(colors.size());
		colors.push_back(std::make_pair(name, color));
		colorsMap.insert(std::make_pair(name, idx));
		DirtyRemap();
	}
	else
	{
		colors[idx].second = color;
		DirtyUpdate();
	}


	return idx;
}



void Material::SetTexture(int32_t idx, Ptr<ITexture> texture)
{
	DirtyUpdate();
	textures[idx].second = texture;
}


void Material::SetScalar(int32_t idx, const float& scalar)
{
	DirtyUpdate();
	scalars[idx].second = scalar;
}


void Material::SetColor(int32_t idx, const glm::vec4& color)
{
	DirtyUpdate();
	colors[idx].second = color;
}


int32_t Material::GetTextureIndex(const std::string& name) const
{
	// Search...
	auto iter =  texturesMap.find(name);

	// Not Found?
	if (iter == texturesMap.end())
		return -1;

	return iter->second;
}


int32_t Material::GetScalarIndex(const std::string& name) const
{
	// Search...
	auto iter = scalarsMap.find(name);

	// Not Found?
	if (iter == scalarsMap.end())
		return -1;

	return iter->second;
}


int32_t Material::GetColorIndex(const std::string& name) const
{
	// Search...
	auto iter = colorsMap.find(name);

	// Not Found?
	if (iter == colorsMap.end())
		return -1;

	return iter->second;
}


bool Material::GetTexture(const std::string& name, Ptr<ITexture>& outTexture) const
{
	int32_t idx = GetTextureIndex(name);
	if (idx == -1)
		return false;

	outTexture = textures[idx].second;
	return true;
}


bool Material::GetScalar(const std::string& name, float& outScalar) const
{
	int32_t idx = GetScalarIndex(name);
	if (idx == -1)
		return false;

	outScalar = scalars[idx].second;
	return true;
}


bool Material::GetColor(const std::string& name, glm::vec4& outColor) const
{
	int32_t idx = GetColorIndex(name);
	if (idx == -1)
		return false;

	outColor = colors[idx].second;
	return true;
}


void Material::DirtyUpdate()
{
	if (dirtyFlag == EMaterialDirtyFlag::None)
		dirtyFlag = EMaterialDirtyFlag::Update;
}


void Material::DirtyRemap()
{
	dirtyFlag = EMaterialDirtyFlag::Remap;
}


void Material::RebuildParamters()
{
	// Rebuild Textures...
	for (size_t i = 0; i < textures.size(); ++i)
	{
		texturesMap.insert(std::make_pair(textures[i].first, i));
	}

	// Rebuild Scalars...
	for (size_t i = 0; i < scalars.size(); ++i)
	{
		scalarsMap.insert(std::make_pair(scalars[i].first, i));
	}

	// Rebuild Colors...
	for (size_t i = 0; i < colors.size(); ++i)
	{
		colorsMap.insert(std::make_pair(colors[i].first, i));
	}

}


void Material::LoadRenderResource()
{
	RAVEN_ASSERT(!isOnGPU, "Material already loaded.");

	if (!shader || !shader->IsOnGPU())
		return;

	// Render Material
	renderRsc = new RenderRscMaterial(shader->GetRenderRsc());
	renderRsc->SetShadowShader(shader->GetShadowRenderRsc());
	renderRsc->LoadInputBlock(shader->GetBlockInput().name, shader->GetSamplersStartIndex());
	renderRsc->SetUniformBuffer(shader->GetUnifromBuffer());

	isOnGPU = true;
	dirtyFlag = EMaterialDirtyFlag::Remap;
	UpdateRenderResource();
}


void Material::UpdateRenderResource()
{
	if (!IsOnGPU())
		return;

	// Remapping...
	if (dirtyFlag == EMaterialDirtyFlag::Remap)
	{
		renderRsc->ClearMapping();

		for (auto& param : textures)
			renderRsc->MapParamter(param.first, &param.second);

		for (auto& param : scalars)
			renderRsc->MapParamter(param.first, &param.second);

		for (auto& param : colors)
			renderRsc->MapParamter(param.first, &param.second);
	}

	// Update Buffer.
	renderRsc->FillBuffer();

	// Clear Flag
	dirtyFlag = EMaterialDirtyFlag::None;
}


void Material::SetMaterialShader(Ptr<MaterialShader> inShader)
{
	// Remove from referenced shader.
	if (shader)
	{
		shader->RemoveRef(this);
	}

	// Set..
	shader = inShader;

	// Is Null?
	if (!shader)
	{
		if (IsOnGPU())
		{
			delete renderRsc;
			renderRsc = NULL;
			isOnGPU = false;
		}

		return;
	}

	shader->AddRef(this);

	if (isOnGPU)
	{
		if (shader->IsOnGPU())
		{
			// Reload Render Shader Resource.
			renderRsc->ReloadShader(shader->GetRenderRsc());
			renderRsc->SetShadowShader(shader->GetShadowRenderRsc());
			renderRsc->LoadInputBlock(shader->GetBlockInput().name, shader->GetSamplersStartIndex());
			renderRsc->SetUniformBuffer(shader->GetUnifromBuffer());

			DirtyRemap();
		}
		else
		{
			delete renderRsc;
			renderRsc = NULL;
			isOnGPU = false;
		}
	}

}


void Material::ReloadShader()
{
	SetMaterialShader(shader);
}


void Material::ClearAllParamters()
{
	texturesMap.clear();
	scalarsMap.clear();
	colorsMap.clear();

	textures.clear();
	scalars.clear();
	colors.clear();

	DirtyRemap();
}


void Material::LoadDefaulFromShader(const std::string& name)
{
	if (!shader)
		return;

	for (const auto& input : shader->GetBlockInput().inputs)
	{
		if (input.first.name == name)
		{
			if (input.first.inputType == EShaderInputType::Float)
			{
				SetScalar(input.first.name, input.first.flag == ESInputDefaultFlag::Black ? 0.0 : 1.0f);
			}
			else if (input.first.inputType == EShaderInputType::Vec4)
			{
				SetColor(input.first.name, input.first.flag == ESInputDefaultFlag::Black ? glm::vec4(0.0f) : glm::vec4(1.0f));
			}
		}
	}
}


} // End of namespace Raven.


