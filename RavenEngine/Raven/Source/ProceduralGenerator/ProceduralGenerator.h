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



#include "IModule.h"
#include "Utilities/Core.h"

#include "TerrainGeneration.h"



namespace Raven 
{
	class Scene;


	// ProceduralGenerator
	//    - Handle generating a new scene procedurally.
	//
	class ProceduralGenerator : public IModule
	{
	public:
		// Consttruct.
		ProceduralGenerator();

		// Destruct.
		~ProceduralGenerator();

		// initialise module
		virtual void Initialize() override;

		// destroy module
		virtual void Destroy() override;

		// Return the type of the module
		static EModuleType GetModuleType() { return MT_ProceduralGenerator; }

		// Return Terrain Generation instance.
		inline TerrainGeneration* GetTerrainGen() { return terrainGen.get(); }

		// Generate a new scene procedurally.
		Scene* GenerateNewScene(const glm::vec2& size, const glm::vec2& height);

	private:
		// The Terrain Generation object.
		Ptr<TerrainGeneration> terrainGen;
	};


}

