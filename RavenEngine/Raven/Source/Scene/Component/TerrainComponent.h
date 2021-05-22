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

#include <map>

#include "ResourceManager/Resources/Texture2D.h"
#include "ResourceManager/Resources/Terrain.h"
#include "Component.h"






namespace Raven
{

	// Represent The terrain in the scene.
	//     - 
	//
	class TerrainComponent : public Component
	{
	public:
		// Construct.
		TerrainComponent();

		// Destruct.
		~TerrainComponent();

		// Set The Terrain.
		void SetTerrainResource(Ptr<Terrain> newTerrain);

		// Return the terrain resrouce.
		Ptr<Terrain> GetTerrain() const { return terrain; }

		// Serialization Save
		template<typename Archive>
		void save(Archive& archive) const
		{
			archive(cereal::base_class<Component>(this));


		}

		// Serialization Load
		template<typename Archive>
		void load(Archive& archive)
		{
			archive(cereal::base_class<Component>(this));


		}

	private:
		// The Terrain.
		Ptr<Terrain> terrain;

	};
}