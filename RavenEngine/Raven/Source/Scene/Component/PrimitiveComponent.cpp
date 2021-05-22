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

#include "PrimitiveComponent.h"

#include "ResourceManager/ResourceManager.h"

#include "Scene/SceneManager.h"
#include "Scene/Scene.h"
#include "Scene/Entity/EntityManager.h"
#include "Utilities/StringUtils.h"

#include "Engine.h"


namespace Raven {



PrimitiveComponent::PrimitiveComponent()
	: clipDistance(-1.0f)
	, isCastShadow(true)
{
	
}


PrimitiveComponent::~PrimitiveComponent()
{

}


void PrimitiveComponent::SetMaterial(uint32_t index, Ptr<Material> mat)
{
	if (materials.size() < index + 1)
		materials.resize(index + 1);

	materials[index] = mat;
}


Material* PrimitiveComponent::GetMaterial(uint32_t index)
{
	// Invalid Index?
	if (index >= materials.size() || index < 0)
		return nullptr;

	return materials[index].get();
}


const Material* PrimitiveComponent::GetMaterial(uint32_t index) const
{
	return const_cast<PrimitiveComponent*>(this)->GetMaterial(index);
}


void PrimitiveComponent::CollectRenderPrimitives(RenderPrimitiveCollector& rcollector)
{

}




} // End of namespace Raven 
