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




#include "Utilities/Core.h"
#include "Math/Frustum.h"
#include "RenderBatch.h"


#include "glm/matrix.hpp"



#define CAPTURE_SHOT 0




namespace Raven
{
	class Scene;
	class Transform;
	class PrimitiveComponent;
	class RenderLight;
	class UniformBuffer;
	class RenderShadowCascade;
	class RenderPrimitiveCollector;
	class ITexture;






	// The Environment Properties of the scene.
	struct RenderSceneEnvironment
	{
		// The Sun Direction.
		glm::vec3 sunDir;

		// The Sun Color.
		glm::vec3 sunColor;

		// The Sun Power.
		float sunPower;

		// The Sun Shadow.
		Ptr<RenderShadowCascade> sunShadow;

		// if true enable sun.
		bool isSun;

		// if true enable sky.
		bool isSky;

		// Reset All Environment Properties.
		void Reset();
	};





	// Holds data of an entity with a primitive component.
	struct ScenePrimitiveData
	{
		// The Primitive Transform.
		Transform* tr;

		// The Primitive Component.
		PrimitiveComponent* comp;
	};






	// RenderScene:
	//		- The render representation of the scene.
	//
	class RenderScene
	{
		// Friend...
		friend class RenderPrimitiveCollector;

	public:
		// Construct. 
		RenderScene();

		// Destruct. 
		~RenderScene();

		// Setup the scene, called once for initializing.
		void Setup();

		// Build Render scene data form a scene.
		void Build(Scene* scene);

		// Set scene view.
		void SetView(const glm::mat4& mtx);

		// Set scene projection.
		void SetProjection(const glm::mat4& mtx, float projFov, float projAspect, float n, float f);

		// Draw the deferred batch.
		void DrawDeferred();

		// Draw all debug primitives that are set to this scene.
		void DrawDebug();

		// Draw the translucent batch.
		void DrawTranslucent(UniformBuffer* lightUB);

		// Draw Shadow.
		void DrawShadow(UniformBuffer* shadowUB);

		// Add primitives to the debug batch to be draw by this scene.
		void SetDebugPrimitives(const std::vector<RenderPrimitive*>* primitives);
		
		// Clear all render data of the previous frame.
		void Clear();

		// Return the view projection matrix.
		inline const glm::mat4& GetViewProjection() const { return viewProjMatrix; }

		// Return the inverse of the view projection matrix.
		inline const glm::mat4& GetViewProjectionInverse() const { return viewProjMatrixInverse; }

		// Return the view direction.
		inline const glm::vec3& GetViewDir() const { return viewDir; }

		// Return the view position.
		inline const glm::vec3& GetViewPos() const { return viewPos; }

		// Return the scene enviornment.
		inline const RenderSceneEnvironment& GetEnvironment() const { return environment; }

		// Return near clipping plane.
		inline const float& GetNear() const { return near; }

		// Return far clipping plane.
		inline const float& GetFar() const { return far; }

		// Return the light in the scene.
		inline const std::vector<RenderLight*>& GetLights() { return rlights; }
		
		// Return true if the scene want to draw the 2D grid.
		inline bool IsGrid() { return isGrid; }

	private:
		// Collect view & projection from the scene.
		void CollectSceneView(Scene* scene);

		// Collect lights from the scene.
		void CollectSceneLights(Scene* scene);

		// Collect the terrain from the scene.
		void CollectTerrain(Scene* scene);

		// Traverse the scene and collect primitives that needs to be rendered.
		void TraverseScene(Scene* scene);

		// Gather all lights that intesect with the bounding sphere.
		void GatherLights(const glm::vec3& center, float radius, std::vector<RenderLight*>& outLights);

		// Gather all Primitive Components from scene.
		void GatherScenePrimitives(Scene* scene, std::vector<ScenePrimitiveData>& outPrimitivesComp);

		// Create New Primitive to render.
		template<class PrimitiveType>
		PrimitiveType* NewPrimitive()
		{
			// TODO: Memeory management for dynamic primitives.
			PrimitiveType* prim = new PrimitiveType();
			prim->indexInScene = rprimitives.size();
			rprimitives.push_back(prim);

			return prim;
		}

		// Create New Primitive to render.
		template<class LightType>
		inline LightType* NewLight()
		{
			// TODO: Memeory management for lights.
			LightType* light = new LightType();
			light->indexInScene = rlights.size();
			rlights.push_back(light);

			return light;
		}

		// Update Light Unifrom Buffer for forward lighting.
		void UpdateLights_FORWARD(UniformBuffer* lightUB, RenderPrimitive* prim);

	private:
		// Batch for rendering deferred primitives.
		RenderBatch<ERenderBatchType::Deferred> deferredBatch;

		// Batch for rendering translucent primitives.
		RenderBatch<ERenderBatchType::Translucent> translucentBatch;

		// The View Matrix.
		glm::mat4 view;

		// Main Project.
		glm::mat4 projection;

		// View & Projection Matrix
		glm::mat4 viewProjMatrix;

		// View Matrix Inverse.
		glm::mat4 viewMatrixInverse;

		// View & Projection Matrix Inverse
		glm::mat4 viewProjMatrixInverse;

		// The View Direction.
		glm::vec3 viewDir;

		// The View Position.
		glm::vec3 viewPos;

		// Near Clipping Plane.
		float near;

		// Far Clipping Plane.
		float far;

		// Field of view of the projection matrix.
		float fov;

		// Aspect ration of the projecetion matrix.
		float aspect;

		// The current frustum of the view & projection. Computed in RenderScene::CollectSceneView().
		MathUtils::Frustum frustum;

		// Scene Enviornment Data.
		RenderSceneEnvironment environment;

		// Dynamic Primitives Container.
		std::vector<RenderPrimitive*> rprimitives;

		// Transform Uniform Buffers.
		Ptr<UniformBuffer> transformUniform;
		Ptr<UniformBuffer> transformBoneUniform;

		// Lights in the scene.
		std::vector<RenderLight*> rlights;

		// Debug primitive added to the scene to be drawn.
		const std::vector<RenderPrimitive*>* debugPrimitives;

		// Draw 2D Grid.
		bool isGrid;

		// Default textures assigned to materials.
		std::vector< Ptr<ITexture> > defaultTextures;

	};


}


