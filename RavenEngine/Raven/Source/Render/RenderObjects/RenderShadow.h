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




#include "Math/Frustum.h"
#include "Math/BoundingBox.h"
#include "RenderBatch.h"


#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/matrix.hpp"


#include <vector>



namespace Raven
{
	class GLTexture;
	class RenderPass;
	class RenderPrimitive;
	class RenderRscShader;



	// Data for a single Shadow cascade.
	struct ShadowCascadeData
	{
		// The shadow furstum.
		MathUtils::Frustum frustum;

		// The shadow view projection matrix.
		glm::mat4	viewProj;

		// The shadow map texture.
		Ptr<GLTexture> shadowMap;

		// The shadow scene that we want to render to the depth map.
		RenderBatch<ERenderBatchType::Shadow> shadowBatch;

		// The shadow render pass for drawing into the shadow map.
		Ptr<RenderPass> shadowPass;
	};



	// RenderShadowCascaded:
	//		- Shadow for sun directional light with cascading.
	class RenderShadowCascade
	{
	public:
		// Construct. 
		RenderShadowCascade();

		// Destruct. 
		~RenderShadowCascade();

		// Reset shadow data.
		void Reset();

		// Build cascade data.
		// @param numCascade: number of cascade to build.
		// @param shadowSize: the size of the shadow map.
		void SetupCascade(uint32_t numCascade, const glm::ivec2& shadowSize);

		// Compute the cascade data for a directional light.
		// @param lightDir: the light direction that cast this shadow.
		void ComputeCascade(const glm::vec3& lightDir, float fov, float a, float n, float f, glm::mat4 viewInv);


		// Return a cascade at index.
		inline ShadowCascadeData& GetCascade(uint32_t index) { return cascade[index]; }
		inline const ShadowCascadeData& GetCascade(uint32_t index) const { return cascade[index]; }

		// Return the number of cascades.
		inline uint32_t GetNumCascade() const { return cascade.size(); }

		// Test if a sphere is inside the shaodw cascade furstum.
		void IsInShadow(const glm::vec3& center, float radius, std::vector<uint32_t>& outIndices);

		// Add primitive to the cascade shadow scene.
		void AddPrimitive(RenderPrimitive* primitive, bool isDefualtShader, std::vector<uint32_t>& shadowCascadeIndices);

		// Return cascade ranges.
		inline const auto& GetCascadeRanges() const { return cascadeRanges; }

		//
		void SetLastCascadeRange(float distance);

	private:
		// Cascades.
		std::vector<ShadowCascadeData> cascade;

		// Sun light direction.
		glm::vec3 lightDir;

		// Clip Distance for cascade shadow furstums.
		std::vector<float> cascadeRanges;

	};


}


