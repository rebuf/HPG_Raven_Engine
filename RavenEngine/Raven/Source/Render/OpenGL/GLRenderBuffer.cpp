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
#include "GLRenderBuffer.h"

#include "GL/glew.h"



namespace Raven {



GLRenderBuffer::GLRenderBuffer()
	: id(0)
	, format(EGLFormat::None)
{

}


GLRenderBuffer::~GLRenderBuffer()
{
	if (id != 0)
	{
		glDeleteRenderbuffers(1, &id);
	}

}


GLRenderBuffer* GLRenderBuffer::Create(EGLFormat format, int width, int height)
{
	GLRenderBuffer* rd = new GLRenderBuffer();
	rd->format = format;
	rd->size.x = width;
	rd->size.y = height;

	glGenRenderbuffers(1, &rd->id);
	glBindRenderbuffer(GL_RENDERBUFFER, rd->id);
	glRenderbufferStorage(GL_RENDERBUFFER, (GLENUM)format, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	return rd;
}


void GLRenderBuffer::Bind()
{
	glBindRenderbuffer(GL_RENDERBUFFER, id);
}


void GLRenderBuffer::Unbind()
{
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}


void GLRenderBuffer::UpdateStorage(int width, int height)
{
	size.x = width;
	size.y = height;
	glRenderbufferStorage(GL_RENDERBUFFER, (GLENUM)format, width, height);
}



} // End of namespace Raven.
