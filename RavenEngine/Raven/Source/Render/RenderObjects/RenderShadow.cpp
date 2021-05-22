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
#include "RenderShadow.h"
#include "RenderPass.h"

#include "Render/OpenGL/GLTexture.h"

#include "glm/matrix.hpp"
#include "glm/gtx/transform.hpp"

#include <limits>


#include "Engine.h"
#include "Render/RenderModule.h"
#include "Render/RenderDebug.h"


#include "GL/glew.h"




namespace Raven {




RenderShadowCascade::RenderShadowCascade()
{

}


RenderShadowCascade::~RenderShadowCascade()
{

}


void RenderShadowCascade::Reset()
{
	for (auto& c : cascade)
	{
		c.shadowBatch.Reset();
	}
}


void RenderShadowCascade::SetupCascade(uint32_t numCascade, const glm::ivec2& shadowSize)
{
	RAVEN_ASSERT(cascade.empty(), "");
	cascade.resize(numCascade);

	for (auto& data : cascade)
	{
		// Shadow Mpa...
		data.shadowMap = Ptr<GLTexture>(GLTexture::Create2D(
			EGLFormat::Depth32,
			shadowSize.x, shadowSize.y,
			EGLFilter::Linear,
			EGLWrap::ClampToBorder
		));


		data.shadowMap->Bind();
		data.shadowMap->BorderColor(1.0f, 1.0f, 1.0f, 1.0f);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		data.shadowMap->Unbind();


		// Shadow Pass...
		data.shadowPass = Ptr<RenderPass>(new RenderPass());
		data.shadowPass->SetDepthTexture(data.shadowMap, false);
		data.shadowPass->SetSize(shadowSize);
		data.shadowPass->Build();
	}
	
	// Ranges...
	cascadeRanges.resize(numCascade + 1);
	cascadeRanges[1] = 7.0f; // Start Range

	for (uint32_t i = 2; i < cascadeRanges.size(); ++i)
	{
		cascadeRanges[i] = cascadeRanges[i-1] * 2.2f;
	}

	cascadeRanges.back() *= 1.5;

}


void RenderShadowCascade::SetLastCascadeRange(float distance)
{
	cascadeRanges.back() = distance; // End Range
}


void RenderShadowCascade::ComputeCascade(const glm::vec3& lightDir, float fov, float a, float n, float f, glm::mat4 viewInv)
{
	RAVEN_ASSERT(!cascade.empty(), "");
	

	fov = glm::radians(fov);
	cascadeRanges.front() = n;


	for (int32_t i = 0; i < cascade.size(); ++i)
	{
		float lightFar = cascadeRanges.back() * 0.7f * (1.0f + (float)i * 0.05f);

		float ht = glm::tan(fov * a * 0.5f);
		float vt = glm::tan(fov * 0.5f);

		float xn = cascadeRanges[i] * ht;
		float xf = cascadeRanges[i + 1] * ht;
		float yn = cascadeRanges[i] * vt;
		float yf = cascadeRanges[i + 1] * vt;



		glm::vec4 frustumCorners[8] = {
			// near face
			glm::vec4( xn,  yn, -cascadeRanges[i], 1.0),
			glm::vec4(-xn,  yn, -cascadeRanges[i], 1.0),
			glm::vec4( xn, -yn, -cascadeRanges[i], 1.0),
			glm::vec4(-xn, -yn, -cascadeRanges[i], 1.0),

			// far face
			glm::vec4( xf,  yf, -cascadeRanges[i + 1], 1.0),
			glm::vec4(-xf,  yf, -cascadeRanges[i + 1], 1.0),
			glm::vec4( xf, -yf, -cascadeRanges[i + 1], 1.0),
			glm::vec4(-xf, -yf, -cascadeRanges[i + 1], 1.0)
		};


		MathUtils::BoundingBox bounds;
		bounds.Add(viewInv * frustumCorners[0]);
		bounds.Add(viewInv * frustumCorners[1]);
		bounds.Add(viewInv * frustumCorners[2]);
		bounds.Add(viewInv * frustumCorners[3]);
		bounds.Add(viewInv * frustumCorners[4]);
		bounds.Add(viewInv * frustumCorners[5]);
		bounds.Add(viewInv * frustumCorners[6]);
		bounds.Add(viewInv * frustumCorners[7]);

		glm::vec3 wext = bounds.GetExtent();
		glm::vec3 wcenter = bounds.GetCenter();


		glm::mat4 lightView = glm::lookAt(
			glm::vec3(0.0f) + wcenter,
			lightDir + wcenter,
			glm::vec3(0.0f, 1.0f, 0.0f));


		MathUtils::BoundingBox lightbounds;
		lightbounds.Add(lightView * glm::vec4(wcenter + glm::vec3(-wext.x, -wext.y, -wext.z), 1.0f));
		lightbounds.Add(lightView * glm::vec4(wcenter + glm::vec3( wext.x, -wext.y, -wext.z), 1.0f));
		lightbounds.Add(lightView * glm::vec4(wcenter + glm::vec3( wext.x,  wext.y, -wext.z), 1.0f));
		lightbounds.Add(lightView * glm::vec4(wcenter + glm::vec3(-wext.x,  wext.y, -wext.z), 1.0f));
		lightbounds.Add(lightView * glm::vec4(wcenter + glm::vec3(-wext.x, -wext.y,  wext.z), 1.0f));
		lightbounds.Add(lightView * glm::vec4(wcenter + glm::vec3( wext.x, -wext.y,  wext.z), 1.0f));
		lightbounds.Add(lightView * glm::vec4(wcenter + glm::vec3( wext.x,  wext.y,  wext.z), 1.0f));
		lightbounds.Add(lightView * glm::vec4(wcenter + glm::vec3(-wext.x,  wext.y,  wext.z), 1.0f));

		glm::vec3 ext = lightbounds.GetExtent() * 1.2f;
		

		glm::mat4 lightProjection = glm::ortho(
			-ext.x,
			 ext.x,
			-ext.y,
			 ext.y,
			-ext.z - lightFar * 0.8f,
			 ext.z + lightFar * 0.2f
		);


		cascade[i].viewProj = lightProjection * lightView;
		cascade[i].frustum.ExtractPlanes( cascade[i].viewProj );
	}


}


void RenderShadowCascade::IsInShadow(const glm::vec3& center, float radius, std::vector<uint32_t>& outIndices)
{
	outIndices.reserve(RENDER_MAX_SHADOW_CASCADE);

	for (int32_t ic = 0; ic < cascade.size(); ++ic)
	{
		if (cascade[ic].frustum.IsInFrustum(center, radius))
			outIndices.push_back(ic);
	}
}


void RenderShadowCascade::AddPrimitive(RenderPrimitive* primitive, bool isDefualtShader, std::vector<uint32_t>& shadowCascadeIndices)
{
	for (auto index : shadowCascadeIndices)
	{
		cascade[index].shadowBatch.Add(primitive, isDefualtShader);
	}
}






} // End of namespace Raven.
