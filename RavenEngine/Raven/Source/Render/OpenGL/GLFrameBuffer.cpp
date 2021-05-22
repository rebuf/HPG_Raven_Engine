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
#include "GLFrameBuffer.h"
#include "GLTexture.h"
#include "GLRenderBuffer.h"

#include "Logger/Console.h"
#include "GL/glew.h"



namespace Raven {




GLFrameBuffer::GLFrameBuffer()
	: id(0)
{

}


GLFrameBuffer::~GLFrameBuffer()
{
	if (id != 0)
	{
		glDeleteFramebuffers(1, &id);
	}
}


GLFrameBuffer* GLFrameBuffer::Create()
{
	GLFrameBuffer* fbo = new GLFrameBuffer();

	glGenFramebuffers(1, &fbo->id);

	return fbo;
}


void GLFrameBuffer::Attach(EGLAttachment attachment, int level, GLTexture* tex, int layer)
{
	GLAttachmentData data;
	data.level = level;
	data.target = attachment;
	data.layer = layer;
	data.texRef = tex;
	data.rdBufferRef = nullptr;

	attachments[attachment] = data;
}


void GLFrameBuffer::Attach(EGLAttachment attachment, GLRenderBuffer* tex)
{
	GLAttachmentData data;
	data.level = 0;
	data.target= attachment;
	data.layer = 0;
	data.texRef = nullptr;
	data.rdBufferRef = tex;


	attachments[attachment] = data;
}


void GLFrameBuffer::ClearAttachment(EGLAttachment attachment)
{
	attachments.erase(attachment);
}


void GLFrameBuffer::Update()
{
	glBindFramebuffer(GL_FRAMEBUFFER, id);

	// List of color attachments...
	std::vector<GLENUM> colorAttachments;

	// Attachments...
	for (const auto& data : attachments)
	{
		const auto& att = data.second;

		// Is Color Attachment?
		if (att.target >= EGLAttachment::Color0 && att.target <= EGLAttachment::Color9)
		{
			colorAttachments.push_back((GLENUM)att.target);
		}

		// Texture Attachment?
		if (att.texRef)
		{
			EGLTexture texType = att.texRef->GetType();
			if (texType == EGLTexture::Texture2D)
			{
				// Attach Texture 2D.
				glFramebufferTexture2D(
					GL_FRAMEBUFFER,
					(GLENUM)att.target, 
					GL_TEXTURE_2D, 
					att.texRef->GetID(),
					att.level
				);
			}
			else if (texType == EGLTexture::CubeMap)
			{
				if (att.layer == -1)
				{
					// Attach Entire CubeMap
					glFramebufferTexture(
						GL_FRAMEBUFFER,
						(GLENUM)att.target,
						att.texRef->GetID(),
						att.level
					);
				}
				else
				{
					// Attach CubeMap layer.
					glFramebufferTexture2D(
						GL_FRAMEBUFFER,
						(GLENUM)att.target,
						GL_TEXTURE_CUBE_MAP_POSITIVE_X + att.layer,
						att.texRef->GetID(),
						att.level
					);
				}
			}
			else
			{
				LOGW("GLFrameBuffer - Trying to attach unsupported texture type.");
			}
		}
		else if (att.rdBufferRef) // Render buffer attachment?
		{
			// Attach Render Buffer.
			glFramebufferRenderbuffer(
				GL_FRAMEBUFFER,
				(GLENUM)att.target,
				GL_RENDERBUFFER,
				att.rdBufferRef->GetID() 
			);

		}

	}

	// Color attachments to draw.
	glDrawBuffers(colorAttachments.size(), colorAttachments.data());

	// Validate Completion of Framebuffer.
	ValidateStatus();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void GLFrameBuffer::ValidateStatus()
{
	GLENUM fbostatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	// Complete?
	if (fbostatus== GL_FRAMEBUFFER_COMPLETE)
		return;

	switch (fbostatus)
	{
	case GL_FRAMEBUFFER_UNDEFINED:
		LOGW("GLFrameBuffer Status UNDEFINED.");
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		LOGW("GLFrameBuffer Status INCOMPLETE_ATTACHMENT.");
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		LOGW("GLFrameBuffer Status INCOMPLETE_MISSING_ATTACHMENT.");
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
		LOGW("GLFrameBuffer Status INCOMPLETE_DRAW_BUFFER.");
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
		LOGW("GLFrameBuffer Status INCOMPLETE_READ_BUFFER.");
		break;

	case GL_FRAMEBUFFER_UNSUPPORTED:
		LOGW("GLFrameBuffer Status UNSUPPORTED.");
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
		LOGW("GLFrameBuffer Status INCOMPLETE_MULTISAMPLE.");
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
		LOGW("GLFrameBuffer Status INCOMPLETE_LAYER_TARGETS.");
		break;
	}
}


void GLFrameBuffer::Bind(EGLFrameBuffer target)
{
	glBindFramebuffer((GLENUM)target, id);
}


void GLFrameBuffer::Unbind(EGLFrameBuffer target)
{
	glBindFramebuffer((GLENUM)target, 0);
}


void GLFrameBuffer::Blit(GLFrameBuffer* fb, EGLBufferMask mask, EGLFilter filter, const FBBlitViewport& src, const FBBlitViewport& dst)
{
	Blit(fb, EGLAttachment::None, EGLAttachment::None, mask, filter, src, dst);
}


void GLFrameBuffer::Blit(GLFrameBuffer* fb, EGLAttachment readAttachment, EGLAttachment drawAttachment, EGLBufferMask mask, EGLFilter filter, const FBBlitViewport& src, const FBBlitViewport& dst)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, id);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb != nullptr ? fb->id : 0);

	if (fb != nullptr)
	{
		if (drawAttachment != EGLAttachment::None)
			glDrawBuffer((GLENUM)drawAttachment);
	}


	if (readAttachment != EGLAttachment::None)
		glReadBuffer((GLENUM)readAttachment);


	glBlitFramebuffer(
		// SRC
		src.x0, src.y0,
		src.x1, src.y1,
		
		// DST
		dst.x0, dst.y0,
		dst.x1, dst.y1,
		
		// MASK; COLOR, DEPTH or STENCIL
		(GLENUM)mask,
		(GLENUM)filter
	);


	// Reset...
	if (readAttachment != EGLAttachment::None)
		glReadBuffer((GLENUM)EGLAttachment::Color0);
	
	if (fb != nullptr)
	{
		if (drawAttachment != EGLAttachment::None)
			glDrawBuffer((GLENUM)drawAttachment);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}


} // End of namespace Raven.
