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
#include "RenderPrimitive.h"
#include "Render/RenderResource/Shader/RenderRscMaterial.h"





namespace Raven {





RenderPrimitive::RenderPrimitive()
	: material(nullptr)
	, indexInScene(-1)
	, isSkinned(false)
{

}


RenderPrimitive::~RenderPrimitive()
{

}


void RenderPrimitive::SetWorldMatrix(const glm::mat4& mtx)
{
	worldMatrix = mtx;
}


void RenderPrimitive::SetNormalMatrix(const glm::mat4& mtx)
{
	normalMatrix = mtx;
}


void RenderPrimitive::SetMaterial(RenderRscMaterial* mat)
{
	material = mat;
}


ERenderShaderDomain RenderPrimitive::GetMaterialDomain() const
{
	return material->GetShaderRsc()->GetDomain();
}


ERenderShaderType RenderPrimitive::GetShaderType() const
{
	return material->GetShaderRsc()->GetType();
}


bool RenderPrimitive::IsCastShadow()
{
	return GetShaderType() != ERenderShaderType::Translucent;
}


} // End of namespace Raven.

