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
#include "RenderScene.h"

#include "Engine.h"
#include "Render/RenderDebug.h"
#include "Render/RenderModule.h"

#include "Primitives/RenderPrimitive.h"
#include "Primitives/RenderTerrain.h"
#include "Primitives/RenderDebugPrimitive.h"
#include "Primitives/RenderPrimitive.h"
#include "Primitives/RenderMesh.h"
#include "Primitives/RenderSkinnedMesh.h"
#include "Primitives/RenderTerrain.h"

#include "RenderPass.h"
#include "RenderLight.h"
#include "RenderShadow.h"


#include "Render/RenderResource/Shader/RenderRscShader.h" 
#include "Render/RenderResource/Shader/RenderRscMaterial.h"
#include "Render/RenderResource/Shader/UniformBuffer.h"
#include "Render/OpenGL/GLBuffer.h"
#include "Render/OpenGL/GLShader.h"


#include "ResourceManager/ResourceManager.h"
#include "ResourceManager/Resources/Mesh.h"
#include "ResourceManager/Resources/Material.h"
#include "ResourceManager/Resources/MaterialShader.h"


#include "Core/Camera.h"
#include "Scene/Scene.h"
#include "Scene/Component/Transform.h"
#include "Scene/Component/Light.h"
#include "Scene/Component/MeshComponent.h"
#include "Scene/Component/SkinnedMeshComponent.h"
#include "Scene/Component/Transform.h"
#include "Scene/Component/TerrainComponent.h"
#include "Scene/Entity/EntityManager.h"
#include <entt/entt.hpp>

#include "Logger/Console.h"


#include "GL/glew.h"
#include "glm/gtc/type_ptr.hpp"









namespace Raven {



// Data reflect TransformVertex Uniform Block.
struct TransformVertexData
{
	glm::mat4 modelMatrix;
	glm::mat4 normalMatrix;
} trData;



// Data reflect TransformVertex Uniform Block for skinned mesh
struct TransformBoneVertexData
{
	glm::mat4 modelMatrix;
	glm::mat4 normalMatrix;
	glm::mat4 bones[RENDER_SKINNED_MAX_BONES];
} trBoneData;



// --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- 




void RenderSceneEnvironment::Reset()
{
	sunColor = glm::vec3(1.0f, 1.0f, 1.0f);
	sunDir = glm::normalize(glm::vec4(-1.0f));
	sunPower = 1.0f;
	sunShadow->Reset();
	isSun = false;
	isSky = false;
}





// --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- 





RenderScene::RenderScene()
	: near(0.0f)
	, far(0.0f)
	, frustum(glm::mat4(1.0f))
	, isGrid(true)
	, fov(false)
{

}


RenderScene::~RenderScene()
{

}


void RenderScene::Setup()
{
	// Transfrom Vertex Uniform 
	transformUniform = Ptr<UniformBuffer>( UniformBuffer::Create(RenderShaderInput::TransformBlock, false) );
	transformBoneUniform = Ptr<UniformBuffer>( UniformBuffer::Create(RenderShaderInput::TransformBoneBlock, false));

	RAVEN_ASSERT(transformUniform->GetDescription().size == sizeof(TransformVertexData), "Invalid Size.");
	RAVEN_ASSERT(transformBoneUniform->GetDescription().size == sizeof(TransformBoneVertexData), "Invalid Size.");

	// Default Textures...
	defaultTextures.resize(3);
	defaultTextures[(int32_t)ESInputDefaultFlag::Normal] =
		Engine::GetModule<ResourceManager>()->GetResource<Texture2D>("assets\\textures\\T_Default_Normal.raven");

	defaultTextures[(int32_t)ESInputDefaultFlag::White] =
		Engine::GetModule<ResourceManager>()->GetResource<Texture2D>("assets\\textures\\T_Default_White.raven");

	defaultTextures[(int32_t)ESInputDefaultFlag::Black] =
		Engine::GetModule<ResourceManager>()->GetResource<Texture2D>("assets\\textures\\T_Default_Black.raven");


	// Sun Shadow Cascade..
	environment.sunShadow = Ptr<RenderShadowCascade>(new RenderShadowCascade());
	environment.sunShadow->SetupCascade(RENDER_MAX_SHADOW_CASCADE, glm::ivec2(1024, 1024));

	// Reset the environment to default.
	environment.Reset();
}


void RenderScene::Build(Scene* scene)
{
	// View & Projection.
	CollectSceneView(scene);

	// Lights.
	CollectSceneLights(scene);

	// Terrain.
	CollectTerrain(scene);

	// Traverse the scene to collected render primitives.
	TraverseScene(scene);

	// ...
	translucentBatch.Sort();
}


void RenderScene::SetView(const glm::mat4& mtx)
{
	view = mtx;
}


void RenderScene::SetProjection(const glm::mat4& mtx, float projFov, float projAspect, float n, float f)
{
	projection = mtx;
	near = n;
	far = f;
	fov = projFov;
	aspect = projAspect;
}


void RenderScene::SetDebugPrimitives(const std::vector<RenderPrimitive*>* primitives)
{
	debugPrimitives = primitives;
}


void RenderScene::TraverseScene(Scene* scene)
{
	// Default Materials.
	const auto& defaultMaterials = Engine::GetModule<RenderModule>()->GetDefaultMaterials();

	// Primitives Collector.
	RenderPrimitiveCollector collector(this);

	// Get All models in the scene
	std::vector<ScenePrimitiveData> outPrimitiveComponents;
	GatherScenePrimitives(scene, outPrimitiveComponents);


	// Iterate over all Models in the scene.
	for (auto& scenePrim : outPrimitiveComponents)
	{
		// Primitive Component - Entity Data
		PrimitiveComponent* primComp = scenePrim.comp;
		Transform* trComp = scenePrim.tr;

		// Compute World Bounds.
		MathUtils::BoundingBox bounds = primComp->GetLocalBounds().Transform(trComp->GetWorldMatrix());

		// distance to view.
		glm::vec3 v = (viewPos - bounds.GetCenter());
		float viewDist2 = v.x * v.x + v.y * v.y + v.z * v.z;


		// Clipping based on distance...
		if (primComp->GetClipDistance() > 0.0)
		{
			float clipDist2 = primComp->GetClipDistance();
			clipDist2 = clipDist2 * clipDist2;

			if (viewDist2 > clipDist2)
			{
				// Don't Draw Component...
				continue;
			}
		}
		
		


		float radius;
		glm::vec3 center;
		bounds.GetSphere(center, radius);

		// View Culling...
		bool isViewCulled = !frustum.IsInFrustum2D(center, radius);

		// Shadow Culling...
		std::vector<uint32_t> shadowCascadeIndices;

		if (primComp->IsCastShadow() && environment.isSun)
		{
			environment.sunShadow->IsInShadow(center, radius, shadowCascadeIndices);
		}

		// Test if its in scene view frustum?
		if (isViewCulled && shadowCascadeIndices.empty())
		{
			// Don't Draw Component...
			continue;
		}


		// Collect Render Render Primitives...
		collector.Reset();
		collector.SetTransform(&trComp->GetWorldMatrix(), &trComp->GetWorldMatrix());
		primComp->CollectRenderPrimitives(collector);

		// Create a RenderPrimitive for each model, and add it to the correct batch.
		for (uint32_t i = 0; i < collector.primitive.size(); ++i)
		{
			RenderPrimitive* rprim = collector.primitive[i];
			bool isDefaultMat = false;

			// Validate Material.
			if (rprim->GetMaterial())
			{
				// Domain Missmatch?
				if (rprim->GetMaterialDomain() != rprim->GetDomain())
				{
					rprim->SetMaterial(nullptr);
				}
			}

			// No Material?
			if (!rprim->GetMaterial())
			{
				Material* defaultMaterial = rprim->isSkinned ? defaultMaterials.skinned.get() : defaultMaterials.mesh.get();
				rprim->SetMaterial(defaultMaterial->GetRenderRsc());
				isDefaultMat = true;
			}


			// Translucent?
			if (!isViewCulled)
			{
				if (rprim->GetShaderType() == ERenderShaderType::Translucent)
				{
					// Gather lights that affect this translucent primitive.
					std::vector<RenderLight*> litPrim;
					GatherLights(center, radius, litPrim);

					for (const auto& lit : litPrim)
						rprim->AddLight(lit->indexInScene);


					// TRANSLUCENT BATCH.
					translucentBatch.Add(rprim, viewDist2);
				}
				else
				{
					// DEFERRED BATCH.
					deferredBatch.Add(rprim);
				}
			}


			if (!shadowCascadeIndices.empty() && rprim->IsCastShadow())
			{
				// Add to shadow scene.
				environment.sunShadow->AddPrimitive(rprim, isDefaultMat, shadowCascadeIndices);
			}

		}
	}

}


void RenderScene::CollectSceneView(Scene* scene)
{
	glm::mat4 camTr;

	// Has Camera? 
	if (scene->GetTargetCamera())
	{
#if CAPTURE_SHOT == 0
		const auto& targetCam = scene->GetTargetCamera();
		SetProjection(targetCam->GetProjectionMatrix(), targetCam->GetFov(), targetCam->GetAspectRatio(), targetCam->GetNear(), targetCam->GetFar());
		camTr = scene->GetCameraTransform()->GetWorldMatrix();
		SetView(glm::inverse(camTr));
#else
		camTr = glm::inverse(view);
#endif
	}
	else
	{
		camTr = glm::inverse(view);
	}

	viewDir = glm::normalize(camTr * glm::vec4(Raven::FORWARD, 0.0f));
	viewPos = camTr * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	viewProjMatrix = projection * view;
	viewMatrixInverse = glm::inverse(view);
	viewProjMatrixInverse = glm::inverse(viewProjMatrix);

	// Set the new Frustum
	frustum = MathUtils::Frustum(viewProjMatrix);
}


void RenderScene::CollectSceneLights(Scene* scene)
{
	// Collect sun lights from global settings...
	if (scene->GetGlobalSettings().isSun)
	{
		environment.isSun = true;
		environment.isSky = scene->GetGlobalSettings().isSky;
		environment.sunDir = scene->GetGlobalSettings().GetSunDir();
		environment.sunPower = scene->GetGlobalSettings().sunPower;
		environment.sunColor = scene->GetGlobalSettings().sunColor;

		// Shadow Cascade...
		environment.sunShadow->ComputeCascade(environment.sunDir, fov, aspect, near, far, viewMatrixInverse);

	}
	else
	{
		environment.isSun = false;
		environment.isSky = false;
		environment.sunColor = glm::vec4(0.0f);
		environment.sunPower = 0.0f;
	}



	// Iterate over all lights in the scene...
	auto lightsEttView = scene->GetRegistry().group<Light>(entt::get<Transform>);

	for (auto entity : lightsEttView)
	{
		const auto& [light, trans] = lightsEttView.get<Light, Transform>(entity);

		// ...
		float lightRadius = glm::max(light.radius, 1.0f);


		//  Clipping...
		if (light.type != (int32_t)LightType::DirectionalLight)
		{
			// Clip based on distance from the view.
			glm::vec3 v = (viewPos - trans.GetWorldPosition());
			float d2 = v.x * v.x + v.y * v.y + v.z * v.z;

			if (d2 > (light.clipDistance * light.clipDistance))
			{
				continue;
			}

			// Clip based on view frustum
			if (!frustum.IsInFrustum(trans.GetWorldPosition(), lightRadius))
			{
				continue;
			}
		}

		// Add new light...
		RenderLight* newLight = NewLight<RenderLight>();
		newLight->type = light.type + 1;
		newLight->colorAndPower = glm::vec4(light.color.x, light.color.y, light.color.z, light.intensity);
		newLight->postion = trans.GetWorldPosition();
		newLight->dir = light.direction;
		newLight->radius = lightRadius;
		newLight->innerAngle = glm::cos(glm::radians(light.innerAngle));
		newLight->outerAngle = glm::cos(glm::radians(light.outerAngle));

	}

}


void RenderScene::CollectTerrain(Scene* scene)
{
	// Default Materials.
	const auto& defaultMaterials = Engine::GetModule<RenderModule>()->GetDefaultMaterials();


	auto TerrainEttView = scene->GetRegistry().view<TerrainComponent>();

	// No Terrain Found?
	if (TerrainEttView.empty())
		return;

	auto entity = TerrainEttView.front();
	const auto& terrainComp = TerrainEttView.get<TerrainComponent>(entity);
	auto terrain = terrainComp.GetTerrain();
	RAVEN_ASSERT(terrain->IsOnGPU(), "Terrain Not Loaded on GPU. Make sure its loaded before rendering.");
	
	// Iterate on bins...
	const auto& bins = *(terrain->GetRenderRsc()->GetBins());
	float binRadius;
	glm::vec3 binCenter;

	std::vector< std::pair<bool, bool> > drawnBins; // first:scene, second:shadow]
	drawnBins.resize(bins.size(), std::make_pair(false, false));


	// Set last cascade distance for terrain scenes.
	environment.sunShadow->SetLastCascadeRange(450);


	for (uint32_t i = 0; i < bins.size(); ++i)
	{
		bins[i].bounds.GetSphere(binCenter, binRadius);

		// View Culling...
		bool isViewCulled = !frustum.IsInFrustum2D(binCenter, binRadius);

		// Shadow Culling...
		std::vector<uint32_t> shadowCascadeIndices;

		if (environment.isSun)
			environment.sunShadow->IsInShadow(binCenter, binRadius, shadowCascadeIndices);

		// Clip?
		if (isViewCulled && shadowCascadeIndices.empty())
			continue;

		// Render Terrain Object...
		RenderTerrain* renderTerrain = NewPrimitive<RenderTerrain>();
		renderTerrain->SetTerrainRsc(terrain->GetRenderRsc());
		renderTerrain->SetWorldMatrix(glm::mat4(1.0f));
		renderTerrain->SetBin(i);

		// Default Terrain Material.
		if (terrain->GetMaterial())
		{
			renderTerrain->SetMaterial(terrain->GetMaterial()->GetRenderRsc());
		}
		else
		{
			renderTerrain->SetMaterial(defaultMaterials.terrain->GetRenderRsc());
		}

		// Add the terrain to the deferred batch.
		if (!isViewCulled)
		{
			deferredBatch.Add(renderTerrain);
			drawnBins[i].first = true;
		}

		// Add terrain to shadow rendering.
		if (!shadowCascadeIndices.empty())
		{
			environment.sunShadow->AddPrimitive(renderTerrain, true, shadowCascadeIndices);
		}


		//Engine::GetModule<RenderModule>()->GetDebug()->DrawBox(binCenter, bins[i].bounds.GetExtent() * 2.0f);
	}


	// For each foliage layer build instance transformation for drawn instances only.
	const auto& foliageLayers = *(terrain->GetRenderRsc()->GetFoliageLayer());
	std::vector< std::vector<glm::mat4> > layersInstanceTransforms;
	layersInstanceTransforms.resize( foliageLayers.size() );


	// Iterate on drawn bins only and determine what foliage to draw...
	for (size_t i = 0; i < drawnBins.size(); ++i)
	{
		// Not Drawn?
		if (!drawnBins[i].first && !drawnBins[i].second)
			continue;

		const auto& bin = bins[i];

		// distance to bin.
		glm::vec3 v = (viewPos - bin.bounds.GetCenter());
		float viewDist2 = v.x * v.x + v.y * v.y + v.z * v.z;

		// Iterate on instances inside this bin...
		for (const auto& foliageInstance : bin.foliage)
		{
			const auto& instanceLayer = foliageLayers[foliageInstance.layer];
			const auto& instance = instanceLayer.GetInstance(foliageInstance.index);
			float layerClipDistance = instanceLayer.GetClipDistanceForTesting();

			// distance to view.
			glm::vec3 v = (viewPos - instance.center);
			float viewDist2 = v.x * v.x + v.y * v.y + v.z * v.z;

			if (viewDist2 > layerClipDistance)
			{
				// Don't Draw instance...
				continue;
			}

			layersInstanceTransforms[foliageInstance.layer].push_back(instance.transform);
		}
	}


	// Iterate on layers...
	for (size_t li = 0; li < layersInstanceTransforms.size(); ++li)
	{
		const auto& layerTr = layersInstanceTransforms[li];

		// Nothing to draw.
		if (layerTr.empty())
			continue;

		const auto& layer = foliageLayers[li];
		const auto& meshInstances = layer.GetMeshInstances();
		const auto& meshMaterials = layer.GetMaterials();

		// Update transforms of all drawn instances...
		if (!meshInstances.empty())
		{
			meshInstances[0]->UpdateTransforms(layerTr);
		}
		
		// Create a RenderPrimitive for each instance
		for (size_t i = 0; i < meshInstances.size(); ++i)
		{
			// Render Foliage...
			RenderTerrainFoliage* renderFoliage = NewPrimitive<RenderTerrainFoliage>();
			renderFoliage->SetMeshRsc( meshInstances[i].get() );
			renderFoliage->SetInstanceCount(layerTr.size());

			// Update material if dirty.
			if (meshMaterials[i]->IsDirty())
			{
				meshMaterials[i]->UpdateRenderResource();
			}

			renderFoliage->SetMaterial( meshMaterials[i]->GetRenderRsc() );
			deferredBatch.Add(renderFoliage);

#if RENDER_MAX_SHADOW_CASCADE == 4
			std::vector<uint32_t> tmp = { 0, 1, 2 , 3 };
			environment.sunShadow->AddPrimitive(renderFoliage, false, tmp);
#else
			std::vector<uint32_t> tmp = { 0 };
			environment.sunShadow->AddPrimitive(renderFoliage, false, tmp);
#endif

		}


	}


}


void RenderScene::Clear()
{
	// Clear Render Batches...
	deferredBatch.Reset();
	translucentBatch.Reset();


	// Delete Dynamic Render Primitives created while building render scene...
	for (auto& prim : rprimitives)
		delete prim;

	// Delete Render Lights created while building render scene...
	for (auto& light : rlights)
		delete light;

	//...
	rprimitives.clear();
	rlights.clear();
	environment.Reset();
	near = 0.0f;
	far = 0.0f;
}


void RenderScene::DrawDeferred()
{
	// ...
	bool isCullFace = true;

	// Bind Transform Uniform Buffer.
	transformUniform->BindBase();
	transformBoneUniform->BindBase();


	// All The Batch Primitives.
	const auto& primitives = deferredBatch.GetPrimitives();

	// First: iterate on shaders...
	for (uint32_t is = 0; is < deferredBatch.GetNumShaders(); ++is)
	{
		const auto& shaderBatch = deferredBatch.GetShaderBatch(is);

		// Two-Sided?
		if (shaderBatch.shader->IsTwoSided())
		{
			if (isCullFace)
			{
				glDisable(GL_CULL_FACE);
				isCullFace = false;
			}
		}
		else 
		{
			if (!isCullFace)
			{
				glEnable(GL_CULL_FACE);
				isCullFace = true;
			}
		}

		// The Shader
		GLShader* shader = shaderBatch.shader->GetShader();
		shader->Use();

		// Second: iterate on materails...
		for (uint32_t im = 0; im < shaderBatch.materials.size(); ++im)
		{
			const auto& materialBatch = deferredBatch.GetMaterialBatch(shaderBatch.materials[im]);

			// The Material
			RenderRscMaterial* material = materialBatch.material;
			material->MakeTexturesActive(defaultTextures);

			if (material->HasMaterialData())
			{
				material->GetUniformBuffer()->BindBase();
				material->UpdateUniformBuffer();
			}


			// Finally: Draw Primitives...
			for (uint32_t ip = 0; ip < materialBatch.primitives.size(); ++ip)
			{
				RenderPrimitive* prim = primitives[materialBatch.primitives[ip]];

				// Transform.
				if (prim->isSkinned)
				{
					auto skinned = static_cast<RenderSkinnedMesh*>(prim);

					// Model & Normal & Bones.
					trBoneData.modelMatrix = prim->GetWorldMatrix();
					trBoneData.normalMatrix = prim->GetWorldMatrix();
					memcpy(&trBoneData.bones, skinned->GetBones()->data(), sizeof(glm::mat4) * skinned->GetBones()->size());
					transformBoneUniform->UpdateData(sizeof(TransformBoneVertexData), 0, (void*)(&trBoneData));
				}
				else
				{
					// Model & Normal.
					trData.modelMatrix = prim->GetWorldMatrix();
					trData.normalMatrix = prim->GetWorldMatrix();
					transformUniform->UpdateData(sizeof(TransformVertexData), 0, &trData);
				}


				// Draw...
				prim->Draw(shader, false);
			}
		}
	}

	// End Drawing...
	glBindVertexArray(0);

}



void RenderScene::DrawDebug()
{
	// Is Empty?
	if (!debugPrimitives || debugPrimitives->empty())
		return;

	const auto& prims = *debugPrimitives;

	// Bind Transform Uniform Buffer.
	transformUniform->BindBase();

	
	// All debug primitives have the same shader & materail.
	RenderRscMaterial* materail = prims[0]->GetMaterial();
	GLShader* shader = materail->GetShaderRsc()->GetShader();
	shader->Use();

	// Draw Debug Primitives...
	for (uint32_t ip = 0; ip < prims.size(); ++ip)
	{
		RenderPrimitive* prim = prims[ip];

		// Transform.
		trData.modelMatrix = prim->GetWorldMatrix();
		trData.normalMatrix = prim->GetWorldMatrix();
		transformUniform->UpdateData(sizeof(TransformVertexData), 0, &trData); // Model & Normal Matrix.

		// Draw...
		prim->Draw(shader, false);
	}

}


void RenderScene::DrawTranslucent(UniformBuffer* lightUB)
{
	// Bind Transform Uniform Buffer.
	transformUniform->BindBase();
	transformBoneUniform->BindBase();

	// All The Batch Primitives.
	const auto& primitives = translucentBatch.GetPrimitives();

	// Iterate over all translucent primitives.
	for (const auto& prim : primitives)
	{
		// The Lights.
		UpdateLights_FORWARD(lightUB, prim.primitive);


		// The Material
		RenderRscMaterial* material = prim.primitive->GetMaterial();
		material->MakeTexturesActive(defaultTextures);

		if (material->HasMaterialData())
		{
			material->GetUniformBuffer()->BindBase();
			material->UpdateUniformBuffer();
		}

		// The Shader.
		GLShader *shader = material->GetShaderRsc()->GetShader();
		shader->Use();

		// Transform.
		if (prim.primitive->isSkinned)
		{
			auto skinned = static_cast<RenderSkinnedMesh*>(prim.primitive);

			// Model & Normal & Bones.
			trBoneData.modelMatrix = prim.primitive->GetWorldMatrix();
			trBoneData.normalMatrix = prim.primitive->GetWorldMatrix();
			memcpy(&trBoneData.bones, skinned->GetBones()->data(), sizeof(glm::mat4) * skinned->GetBones()->size());
			transformBoneUniform->UpdateData(sizeof(TransformBoneVertexData), 0, (void*)(&trBoneData));
		}
		else
		{
			// Model & Normal.
			trData.modelMatrix = prim.primitive->GetWorldMatrix();
			trData.normalMatrix = prim.primitive->GetWorldMatrix();
			transformUniform->UpdateData(sizeof(TransformVertexData), 0, &trData);
		}

		// Draw...
		prim.primitive->Draw(shader, false);
	}

}


void RenderScene::UpdateLights_FORWARD(UniformBuffer* lightUB, RenderPrimitive* prim)
{
	static std::vector<glm::vec4> lightPos(RENDER_PASS_FORWARD_MAX_LIGHTS);
	static std::vector<glm::vec4> lightDir(RENDER_PASS_FORWARD_MAX_LIGHTS);
	static std::vector<glm::vec4> lightPower(RENDER_PASS_FORWARD_MAX_LIGHTS);
	static std::vector<glm::vec4> lightData(RENDER_PASS_FORWARD_MAX_LIGHTS);

	const auto& lit = prim->GetLights();

	for (int32_t i = 0; i < RENDER_PASS_FORWARD_MAX_LIGHTS; ++i)
	{
		if (i >= lit.size())
		{
			lightData[i].r = 0.0f; // No Light.
			continue;
		}

		const auto& light = rlights[ lit[i] ];
		lightPos[i] = glm::vec4(light->postion, 0.0f);
		lightDir[i] = glm::vec4(light->dir, 0.0f);
		lightData[i].r = light->GetType();
		lightData[i].g = light->innerAngle;
		lightData[i].b = light->outerAngle;
		lightData[i].a = light->radius;
		lightPower[i] = light->colorAndPower;
	}



	lightUB->SetDataValues(0, lightDir);
	lightUB->SetDataValues(1, lightPos);
	lightUB->SetDataValues(2, lightPower);
	lightUB->SetDataValues(3, lightData);
	lightUB->Update();
}


void RenderScene::GatherLights(const glm::vec3& center, float radius, std::vector<RenderLight*>& outLights)
{
	float r2 = radius * radius;

	for (uint32_t il = 0; il < rlights.size(); ++il)
	{
		// Invalid or Directional?
		if (rlights[il]->type <= 1)
			continue;

		glm::vec3 v = (center - rlights[il]->postion);
		float d2 = v.x * v.x + v.y * v.y + v.z * v.z;

		float lr2 = rlights[il]->radius;
		lr2 = lr2 * lr2;


		// No intersection?
		if ((d2 - lr2) > r2)
			continue;

		outLights.push_back(rlights[il]);
	}

}


void RenderScene::GatherScenePrimitives(Scene* scene, std::vector<ScenePrimitiveData>& outPrimitivesComp)
{
	// Iterate over all MeshComponent in the scene.
	{
		auto meshPrimitives = scene->GetRegistry().group<MeshComponent>(entt::get<Transform>);

		for (auto entity : meshPrimitives)
		{
			auto& [mesh, trans] = meshPrimitives.get<MeshComponent, Transform>(entity);

			ScenePrimitiveData scenePrim;
			scenePrim.comp = &mesh;
			scenePrim.tr = &trans;
			outPrimitivesComp.push_back(scenePrim);
		}
	}


	// Iterate over all SkinnedMeshComponent in the scene.
	{
		auto skinnedPrimitives = scene->GetRegistry().group<SkinnedMeshComponent>(entt::get<Transform>);

		for (auto entity : skinnedPrimitives)
		{
			auto& [skinned, trans] = skinnedPrimitives.get<SkinnedMeshComponent, Transform>(entity);

			ScenePrimitiveData scenePrim;
			scenePrim.comp = &skinned;
			scenePrim.tr = &trans;
			outPrimitivesComp.push_back(scenePrim);
		}
	}

}


void RenderScene::DrawShadow(UniformBuffer* shadowUB)
{
	// ...
	bool isCullFace = true;

	// Default Materials.
	const auto& defaultMaterials = Engine::GetModule<RenderModule>()->GetDefaultMaterials();


	// Bind Transform Uniform Buffer.
	shadowUB->BindBase();
	transformUniform->BindBase();
	transformBoneUniform->BindBase();

	// Shadow Cascade...
	RenderShadowCascade* shadow = GetEnvironment().sunShadow.get();


	for (uint32_t i = 0; i < shadow->GetNumCascade(); ++i)
	{
		ShadowCascadeData& cascade = shadow->GetCascade(i);
		glm::ivec2 shadowSize = cascade.shadowPass->GetSize();

		// Begin...
		cascade.shadowPass->Begin(glm::ivec4(0, 0, shadowSize.x, shadowSize.y), true);

		// Shadow Transform...
		shadowUB->SetDataValue(0, cascade.viewProj);
		shadowUB->Update();

		// All The Batch Primitives.
		auto& shadowBatch = cascade.shadowBatch;
		const auto& primitives = shadowBatch.GetPrimitives();


		// First: iterate on shaders...
		for (uint32_t is = 0; is < shadowBatch.GetNumShaders(); ++is)
		{
			const auto& shaderBatch = shadowBatch.GetShaderBatch(is);

			// Default Shadow Shader not assigned?
			if (!shaderBatch.shader)
			{
				switch (shaderBatch.domain)
				{
				case Raven::ERenderShaderDomain::Mesh:
					shadowBatch.SetDefaultShader(is, defaultMaterials.mesh->GetRenderRsc()->GetShadowShaderRsc(), defaultMaterials.mesh->GetRenderRsc());
					break;
				case Raven::ERenderShaderDomain::Skinned:
					shadowBatch.SetDefaultShader(is, defaultMaterials.skinned->GetRenderRsc()->GetShadowShaderRsc(), defaultMaterials.skinned->GetRenderRsc());
					break;
				case Raven::ERenderShaderDomain::Terrain:
					shadowBatch.SetDefaultShader(is, defaultMaterials.terrain->GetRenderRsc()->GetShadowShaderRsc(), defaultMaterials.terrain->GetRenderRsc());
					break;
				}
			}


			// Two-Sided?
			if (shaderBatch.shader->IsTwoSided())
			{
				if (isCullFace)
				{
					glDisable(GL_CULL_FACE);
					isCullFace = false;
				}
			}
			else
			{
				if (!isCullFace)
				{
					glEnable(GL_CULL_FACE);
					isCullFace = true;
				}
			}

			// The Shader
			GLShader* shader = shaderBatch.shader->GetShader();
			shader->Use();

			// Second: iterate on materails...
			for (uint32_t im = 0; im < shaderBatch.materials.size(); ++im)
			{
				const auto& materialBatch = shadowBatch.GetMaterialBatch(shaderBatch.materials[im]);

				// The Material
				RenderRscMaterial* material = materialBatch.material;
				material->MakeTexturesActive(defaultTextures);

				if (material->HasMaterialData())
				{
					material->GetUniformBuffer()->BindBase();
					material->UpdateUniformBuffer();
				}


				// Finally: Draw Primitives...
				for (uint32_t ip = 0; ip < materialBatch.primitives.size(); ++ip)
				{
					RenderPrimitive* prim = primitives[materialBatch.primitives[ip]];

					// Transform.
					if (prim->isSkinned)
					{
						auto skinned = static_cast<RenderSkinnedMesh*>(prim);

						// Model & Normal & Bones.
						trBoneData.modelMatrix = prim->GetWorldMatrix();
						trBoneData.normalMatrix = prim->GetWorldMatrix();
						memcpy(&trBoneData.bones, skinned->GetBones()->data(), sizeof(glm::mat4) * skinned->GetBones()->size());
						transformBoneUniform->UpdateData(sizeof(TransformBoneVertexData), 0, (void*)(&trBoneData));
					}
					else
					{
						// Model & Normal.
						trData.modelMatrix = prim->GetWorldMatrix();
						trData.normalMatrix = prim->GetWorldMatrix();
						transformUniform->UpdateData(sizeof(TransformVertexData), 0, &trData);
					}


					// Draw...
					prim->Draw(shader, true);
				}
			}
		}
		
	}

	glBindVertexArray(0);

}




} // End of namespace Raven.
