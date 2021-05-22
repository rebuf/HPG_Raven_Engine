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
#include "RenderPrimitiveCollector.h"

#include "RenderScene.h"
#include "Primitives/RenderMesh.h"
#include "Primitives/RenderSkinnedMesh.h"





namespace Raven {


RenderPrimitiveCollector::RenderPrimitiveCollector(RenderScene* sceneOwner)
	: owner(sceneOwner)
	, worldMatrix(nullptr)
	, normalMatrix(nullptr)
{

}


void RenderPrimitiveCollector::Reset()
{
	primitive.clear();
	viewDist = -1.0f;
}


RenderMesh* RenderPrimitiveCollector::NewMesh()
{
	RenderMesh* rmesh = owner->NewPrimitive<RenderMesh>();
	rmesh->SetWorldMatrix(*worldMatrix);
	rmesh->SetNormalMatrix(*normalMatrix);
	primitive.push_back(rmesh);

	return rmesh;
}


RenderSkinnedMesh* RenderPrimitiveCollector::NewSkinnedMesh()
{
	RenderSkinnedMesh* rskinned = owner->NewPrimitive<RenderSkinnedMesh>();
	rskinned->SetWorldMatrix(*worldMatrix);
	rskinned->SetNormalMatrix(*normalMatrix);
	primitive.push_back(rskinned);

	return rskinned;
}


} // End of namespace Raven.


