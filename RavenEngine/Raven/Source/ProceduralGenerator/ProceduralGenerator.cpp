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
#include "ProceduralGenerator/ProceduralGenerator.h"


#include "Scene/Scene.h"
#include "Scene/Entity/Entity.h"
#include "Scene/Component/Transform.h"
#include "Scene/Component/TerrainComponent.h"
#include "Scene/Component/Light.h"


#include "Engine.h"
#include "ResourceManager/ResourceManager.h"
#include "ResourceManager/Resources/Terrain.h"
#include "ResourceManager/Resources/Mesh.h"




namespace Raven {


ProceduralGenerator::ProceduralGenerator()
{
	// Create instane of TerrainGeneration.
	terrainGen = Ptr<TerrainGeneration>(new TerrainGeneration());
}


ProceduralGenerator::~ProceduralGenerator()
{

}


void ProceduralGenerator::Initialize()
{

}


void ProceduralGenerator::Destroy()
{

}


void AddGrassLayer(Ptr<Terrain> terrain, Ptr<HeightMap> heightMap)
{
	int32_t grassLayer = -1;

	// Gras...
	{
		Ptr<Mesh> grassMesh = Engine::GetModule<ResourceManager>()->GetResource<Mesh>(
			"assets/Meshes/Grass/SMs_Grass_1_1.raven");

		Ptr<Material> grassMaterail = Engine::GetModule<ResourceManager>()->GetResource<Material>(
			"assets/Materials/M_Grass_1_Instanced.raven");

		std::vector< Ptr<Material> > grassMaterails;
		grassMaterails.push_back(grassMaterail);

		grassLayer = terrain->NewLayer("Grass_1", grassMesh, grassMaterails);
	}


	terrain->GetFoliageLayer(grassLayer).SetClipDistance(150.0f);


	for (size_t i = 0; i < terrain->GetBins().size(); ++i)
	{
		const auto& bin = terrain->GetBins()[i];

		glm::vec3 bcenter = bin.bounds.GetCenter();
		glm::vec3 bext = bin.bounds.GetExtent();
		glm::vec3 binCorner = bcenter - bext;

		for (float z = binCorner.z; z < (binCorner.z + bext.z * 2.0f); z += 5.4f)
		{
			for (float x = binCorner.x; x < (binCorner.x + bext.x * 2.0f); x += 5.4f)
			{
				float h = heightMap->GetHeight(x, z);

				if (h < 6.5 || h > 50.0)
				{
					continue;
				}


				auto p = glm::vec3(x, h - 5.2f, z);
				auto noiseg = glm::perlin(p + 3000.0f);

				if (h < 20 || h > 25.0)
				{
					if (noiseg * 10000.0f > 0.07f)
						continue;
				}

				p.x += glm::fract(noiseg) * 2.4f;

				auto scale = 6.2;
				auto noisex = glm::simplex(p + 100.0f);
				p.z += glm::fract(noisex) * 2.4f;

				glm::mat4 tr;
				tr = glm::translate(glm::mat4(1.0f), p);
				tr = tr * glm::rotate(glm::mat4(1.0f), noisex, glm::vec3(0.0, 1.0, 0.0));
				tr = tr * glm::scale(glm::mat4(1.0f), abs(glm::vec3(scale)));


				terrain->AddFoliageInstance(i, grassLayer, tr);
			}

		}

	}

}


void AddTreeLayer(Ptr<Terrain> terrain, Ptr<HeightMap> heightMap)
{
	int32_t treeLayer = -1;

	// Gras...
	{
		Ptr<Mesh> mesh = Engine::GetModule<ResourceManager>()->GetResource<Mesh>(
			"assets/Meshes/Tree/SMs_Hazelnut_Tree.raven");

		Ptr<Material> materail0 = Engine::GetModule<ResourceManager>()->GetResource<Material>(
			"assets/Meshes/Tree/M_Tree_Instanced.raven");

		Ptr<Material> materail1 = Engine::GetModule<ResourceManager>()->GetResource<Material>(
			"assets/Meshes/RavenTree/M_Tree_Leavs_Instanced.raven");

		std::vector< Ptr<Material> > materials;
		materials.push_back(materail0);
		materials.push_back(materail1);

		treeLayer = terrain->NewLayer("Trees_1", mesh, materials);
	}


	terrain->GetFoliageLayer(treeLayer).SetClipDistance(150.0f);


	for (size_t i = 0; i < terrain->GetBins().size(); ++i)
	{
		const auto& bin = terrain->GetBins()[i];

		glm::vec3 bcenter = bin.bounds.GetCenter();
		glm::vec3 bext = bin.bounds.GetExtent();
		glm::vec3 binCorner = bcenter - bext;

		for (float z = binCorner.z; z < (binCorner.z + bext.z * 2.0f); z += 21.5f)
		{
			for (float x = binCorner.x; x < (binCorner.x + bext.x * 2.0f); x += 21.5f)
			{
				float h = heightMap->GetHeight(x, z);

				if (h < 6.5 || h > 50.0)
				{
					continue;
				}


				auto p = glm::vec3(x, h - 1.7f, z);
				auto noiseg = glm::perlin(p + 3000.0f);

				if (h < 20 || h > 25.0)
				{
					if (noiseg * 10000.0f > 0.07f)
						continue;
				}

				p.x += glm::fract(noiseg) * 2.4f;

				auto noise = glm::mix(0.6, 1.0, glm::perlin(p));
				auto noisex = glm::simplex(p + 100.0f);
				p.z += glm::fract(noisex) * 2.4f;

				glm::mat4 tr;
				tr = glm::translate(glm::mat4(1.0f), p);
				tr = tr * glm::rotate(glm::mat4(1.0f), noisex, glm::vec3(0.0, 1.0, 0.0));
				tr = tr * glm::scale(glm::mat4(1.0f), abs(glm::vec3(2.0 * noise)));


				terrain->AddFoliageInstance(i, treeLayer, tr);
			}

		}

	}

}


Scene* ProceduralGenerator::GenerateNewScene(const glm::vec2& size, const glm::vec2& height)
{
	static float offset = 0.0f;
	terrainGen->seedOffset += offset;
	offset += 100.0f;

	// --- - -- - --- -- --- --- ---
	// Terrain.
	Ptr<HeightMap> heightMap = terrainGen->GenerateHeightMap(512, 512);
	heightMap->SetHeightScale(height);
	heightMap->SetSizeScale(size);
	heightMap->ComputeTangents();

	Ptr<Terrain> terrain( new Terrain() );
	terrain->SetName("The_Terrain");
	terrain->SetTerrainData(heightMap, size, 4, height);
	terrain->LoadRenderResource();

	Ptr<Material> terrainMaterial = Engine::GetModule<ResourceManager>()->GetResource<Material>(
		"assets/Materials/M_Terrain_A.raven");

	terrain->SetMaterial(terrainMaterial);

	AddGrassLayer(terrain, heightMap);
	AddTreeLayer(terrain, heightMap);

	// --- - -- - --- -- --- --- ---
	// Scene.
	Scene* scene = new Scene("Procedural_Scene");

	Entity terrainEntity = scene->CreateEntity("The_Terrain");
	TerrainComponent& terrainComp = terrainEntity.GetOrAddComponent<TerrainComponent>();
	terrainComp.SetTerrainResource(terrain);


	scene->GetGlobalSettings().isSun= true;
	scene->GetGlobalSettings().isSky = true;
	scene->GetGlobalSettings().sunColor = glm::vec3(1.0, 0.7, 0.65);
	scene->GetGlobalSettings().sunPower = 22.0f;


	return scene;
}



} // End of namespace Raven



