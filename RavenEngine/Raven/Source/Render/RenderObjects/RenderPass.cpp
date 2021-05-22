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
#include "RenderPass.h"


#include "Render/OpenGL/GLFrameBuffer.h"
#include "Render/OpenGL/GLTexture.h"
#include "Render/OpenGL/GLRenderBuffer.h"

#include "GL/glew.h"
#include "glm/gtc/type_ptr.hpp"



#define RENDER_BUFFER_COLOR_INDEX 0
#define RENDER_BUFFER_DEPTH_INDEX 1





namespace Raven {



RenderPass::RenderPass()
	: isColorRenderBuffer(false)
	, isDepthRenderBuffer(false)
	, isStencilDepth(false)
	, isBuilt(false)
{

}


RenderPass::~RenderPass()
{

}


void RenderPass::Build()
{
	RAVEN_ASSERT(size.x > 0 && size.y > 0, "Invalid Render Size.");
	RAVEN_ASSERT(!isBuilt && fbo == nullptr, "Trying to rebuild without destroying the prevous render pass.");


	// Framebuffer
	fbo = Ptr<GLFrameBuffer>(GLFrameBuffer::Create());

	// Render Pass output targets.
	for (uint32_t i = 0; i < (uint32_t)textures.size(); ++i)
	{
		auto& target = textures[i];

		// Invalid?
		if (!target.texture)
			continue;

		// Attach Target.
		fbo->Attach(
			static_cast<EGLAttachment>( (GLUINT)EGLAttachment::Color0 + i ),
			textures[i].level,
			textures[i].texture.get(),
			textures[i].layer
		);
	}


	// Use RenderBuffer for color0...
	if (isColorRenderBuffer && textures.empty())
	{
		GLRenderBuffer* renderBuffer = GLRenderBuffer::Create(
			EGLFormat::RGBA,
			size.x, size.y
		);

		// Attach RenderBuffer -> Color.
		fbo->Attach(
			EGLAttachment::Color0,
			renderBuffer
		);

		renderBuffers[RENDER_BUFFER_COLOR_INDEX] = Ptr<GLRenderBuffer>(renderBuffer);
	}


	// Render Pass depth targets.
	if (depthTexture.texture)
	{
		EGLAttachment depthAttachment = isStencilDepth
			? EGLAttachment::DepthStencil
			: EGLAttachment::Depth;

		// Attach Depth.
		fbo->Attach(
			depthAttachment,
			depthTexture.level,
			depthTexture.texture.get(),
			depthTexture.layer
		);
	}
	else
	{
		// Use RenderBuffer for depth...
		if (isDepthRenderBuffer)
		{
			GLRenderBuffer* renderBuffer = GLRenderBuffer::Create(
				isStencilDepth ? EGLFormat::DepthStencil : EGLFormat::Depth24,
				size.x, size.y
			);


			EGLAttachment depthAttachment = isStencilDepth
				? EGLAttachment::DepthStencil
				: EGLAttachment::Depth;

			// Attach RenderBuffer -> Depth.
			fbo->Attach(
				depthAttachment,
				renderBuffer
			);

			renderBuffers[RENDER_BUFFER_DEPTH_INDEX] = Ptr<GLRenderBuffer>(renderBuffer);
		}
	}

	fbo->Update();

	// Flag Built.
	isBuilt = true;
}


void RenderPass::Destroy()
{
	textures.clear();
	depthTexture.texture.reset();
	renderBuffers[RENDER_BUFFER_COLOR_INDEX].reset();
	renderBuffers[RENDER_BUFFER_DEPTH_INDEX].reset();
	fbo.reset();

	// Clear Built Flag.
	isBuilt = false;
}


void RenderPass::AddTexture(uint32_t index, const Ptr<GLTexture>& inTex, int32_t level, int32_t layer)
{
	RAVEN_ASSERT(!isBuilt, "RenderPass is already built - Can't Edit.");

	if (textures.size() < index + 1)
		textures.resize(index + 1);

	auto& target = textures[index];
	target.texture = inTex;
	target.level = level;
	target.layer = layer;
	target.clearValue = glm::vec4(0.0f);
}


void RenderPass::SetClearColor(uint32_t index, const glm::vec4& color)
{
	auto& target = textures[index];
	target.clearValue = color;
}


void RenderPass::SetDepthTexture(const Ptr<GLTexture>& inTex, bool isWithStencil, int32_t level, int32_t layer)
{
	RAVEN_ASSERT(!isBuilt, "RenderPass is already built - Can't Edit.");

	depthTexture.texture = inTex;
	depthTexture.level = level;
	depthTexture.layer = layer;
	depthTexture.clearValue = glm::vec4(1.0f);

	isStencilDepth = isWithStencil;
}


void RenderPass::SetRenderBufferForColor(bool val)
{
	isColorRenderBuffer = val;
}


void RenderPass::SetRenderBufferForDepth(bool val, bool isWithStencil)
{
	isDepthRenderBuffer = val;
	isStencilDepth = isWithStencil;
}


void RenderPass::ResizeTargets(const glm::ivec2& newSize)
{
	SetSize(newSize);

	// Not Built yet?
	if (!isBuilt)
	{
		return; // Skip..
	}

	// Resize textures...
	for (uint32_t i = 0; i < (uint32_t)textures.size(); ++i)
	{
		auto& target = textures[i];

		// Invalid?
		if (!target.texture)
			continue;

		// Same Size?
		if (target.texture->GetWidth()== newSize.x && target.texture->GetHeight() == newSize.y)
			continue;

		target.texture->Bind();
		target.texture->UpdateTexData(target.level, size.x, size.y, target.layer, nullptr);
	}


	// Has Depth? resize...
	if (depthTexture.texture != nullptr)
	{
		// different Size?
		if (depthTexture.texture->GetWidth() != newSize.x || depthTexture.texture->GetHeight() != newSize.y)
		{
			depthTexture.texture->Bind();
			depthTexture.texture->UpdateTexData(depthTexture.level, size.x, size.y, depthTexture.layer, nullptr);
		}
	}

	
	// Resize Render Buffers...
	for (int32_t i = 0; i < 2; ++i)
	{
		// Invalid?
		if (!renderBuffers[i])
			continue;

		renderBuffers[i]->Bind();
		renderBuffers[i]->UpdateStorage(size.x, size.y);
	}
}


void RenderPass::Begin(const glm::ivec4& viewport, bool isClear)
{
	fbo->Bind(EGLFrameBuffer::Framebuffer);

	// Viewport.
	glViewport(viewport.x, viewport.y, viewport.z, viewport.w);
	glScissor(viewport.x, viewport.y, viewport.z, viewport.w);

	// Clear Targets?
	if (isClear)
	{
		GLENUM clearFlag = 0x0;

		// Clear Color Targets...
		for (uint32_t i = 0; i < (uint32_t)textures.size(); ++i)
		{
			if (!textures[i].texture)
				continue;

			glClearBufferfv(GL_COLOR, i, glm::value_ptr(textures[i].clearValue));
		}

		if (isColorRenderBuffer)
		{
			clearFlag |= GL_COLOR_BUFFER_BIT;
		}

		// Clear Depth Target...
		if (depthTexture.texture != nullptr)
		{
			glClearBufferfv(GL_DEPTH, 0, &depthTexture.clearValue.x);

			if (isStencilDepth)
			{
				clearFlag |= GL_STENCIL_BUFFER_BIT;
			}
		}
		else if (isDepthRenderBuffer)
		{
			glClearDepth(1.0f);
			clearFlag |= GL_DEPTH_BUFFER_BIT;
		}

		// Clear if flag not 0.
		if (clearFlag != 0x0)
			glClear(clearFlag);
	}

}


void RenderPass::ReplaceTarget(uint32_t index, const Ptr<GLTexture>& inTex, int32_t level, int32_t layer)
{
	bool rebuild = isBuilt;
	isBuilt = false;

	// Replace...
	AddTexture(index, inTex, level, layer);

	// Need Rebuilding?
	if (rebuild)
	{
		// Attach Target.
		fbo->Attach(
			static_cast<EGLAttachment>((GLUINT)EGLAttachment::Color0 + index),
			textures[index].level,
			textures[index].texture.get(),
			textures[index].layer
		);

		fbo->Update();
		isBuilt = true;
	}
	else
	{
		Build();
	}

}


void RenderPass::End()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}



} // End of namespace Raven.
