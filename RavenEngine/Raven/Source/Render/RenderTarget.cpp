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
#include "RenderTarget.h"
#include "OpenGL/GLFrameBuffer.h"
#include "OpenGL/GLRenderBuffer.h"
#include "OpenGL/GLTexture.h"



namespace Raven {




RenderTarget::RenderTarget(EGLTexture type, EGLFormat format)
	: fbo(nullptr)
	, dirty(true)
{
	RAVEN_ASSERT(type == EGLTexture::Texture2D, "NOT SUPPORTED YET....");
	rtTexture = GLTexture::Create(type, format);
}


RenderTarget::~RenderTarget()
{
	delete rtTexture;
	delete fbo;
}


uint32_t RenderTarget::GetRTTextureID()
{
	return rtTexture->GetID();
}


void RenderTarget::Resize(glm::ivec2 newSize)
{
	dirty = newSize.x > size.x || newSize.y > size.y;;
	size = newSize;
}


void RenderTarget::SetClearColor(const glm::vec4& color)
{
	clearColor = color;
}


void RenderTarget::SetSampleProeprties(EGLFilter filter, EGLWrap wrap)
{
	rtTexture->SetFilter(filter);
	rtTexture->SetWrap(wrap);
}


void RenderTarget::Update()
{
	dirty = false;

	// Update Texture...
	bool rtResize = false;
	rtTexture->Bind();

	// Need Resize?
	if ( size.x > rtTexture->GetWidth() 
		|| size.y > rtTexture->GetHeight() )
	{
		rtTexture->UpdateTexData(0, size.x, size.y, nullptr);
		rtResize = true;
	}

	rtTexture->UpdateTexParams();
	rtTexture->Unbind();


	// Framebuffer...
	if (!fbo)
	{
		fbo = GLFrameBuffer::Create();
		fbo->Attach(EGLAttachment::Color0, 0, rtTexture, 0);
		fbo->Update();
	}

}


glm::ivec2 RenderTarget::GetActualSize() const
{
	return glm::ivec2(rtTexture->GetWidth(), rtTexture->GetHeight());
}


} // End of namespace Raven.
