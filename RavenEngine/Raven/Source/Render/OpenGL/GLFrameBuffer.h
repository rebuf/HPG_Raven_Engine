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




#include "GLTypes.h"

#include <map>





namespace Raven
{
	//
	class GLTexture;
	class GLRenderBuffer;




	// Represent a single attachemnt fo framebuffer.
	struct GLAttachmentData
	{
		// The attachment 
		EGLAttachment target;

		// The texture that is attachted.
		GLTexture* texRef;

		// The render buffer that is attachted.
		GLRenderBuffer* rdBufferRef;

		// The texture lod level.
		int level;

		// If the texture has multipe layer, this is the index of 
		// the layer we want to attach
		int layer;
	};


	// Viewport used by frame buffer blit operation.
	struct FBBlitViewport
	{
		// Viewport x0->x1 and y0 -> y1
		int x0;
		int x1;
		int y0;
		int y1;

		// Default Construct - Uninitialized
		FBBlitViewport()
		{

		}

		// Construct.
		FBBlitViewport(int ix0, int iy0, int ix1, int iy1)
			: x0(ix0)
			, x1(ix1)
			, y0(iy0)
			, y1(iy1)
		{

		}
	};



	//
	//
	class GLFrameBuffer
	{
	private:
		// Construct. 
		GLFrameBuffer();

	public:
		// Destruct.
		~GLFrameBuffer();

	  // Return the opengl id of the buffer.
		inline GLUINT GetID() const { return id; }

		// Create the framebuffer.
		static GLFrameBuffer* Create();

		// Attacment texture to a framebuffer attachment.
		//
		void Attach(EGLAttachment attachment, int level, GLTexture* tex, int layer = 0);

		// Attacment renderbuffer to a framebuffer attachment.
		//
		void Attach(EGLAttachment attachment, GLRenderBuffer* tex);

		// Clear an attachment.
		void ClearAttachment(EGLAttachment attachment);

		// Update the OpenGL framebuffer and its attachments.
		void Update();

		// Bind this frame buffer.
		void Bind(EGLFrameBuffer target);

		// Unbind this frame buffer.
		void Unbind(EGLFrameBuffer target);

		// Blit this framebuffer into another framebuffer.
		// @param fb: the framebuffer to blit into, if null will blit into default framebuffer
		void Blit(GLFrameBuffer* fb, EGLBufferMask mask, EGLFilter filter, const FBBlitViewport& src, const FBBlitViewport& dst);

		// Blit this framebuffer into another framebuffer, for a specific color attachment.
		// @param fb: the framebuffer to blit into, if null will blit into default framebuffer
		void Blit(GLFrameBuffer* fb, EGLAttachment readAttachment, EGLAttachment drawAttachment, EGLBufferMask mask, EGLFilter filter, const FBBlitViewport& src, const FBBlitViewport& dst);

	private:
		// Validate framebuffer status.
		void ValidateStatus();

	private:
		// OpenGL Object ID.
		GLUINT id;

		// Framebuffer attachments.
		std::map<EGLAttachment, GLAttachmentData> attachments;
	};

}



