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


#include "GLBuffer.h"
#include "Utilities/Core.h"
#include "GL/glew.h"





namespace Raven {






GLBuffer::GLBuffer()
	: id(0)
	, size(0)
	, type(EGLBufferType::None)
	, usage(EGLBufferUsage::StaticDraw)
{

}


GLBuffer::~GLBuffer()
{
	if (id != 0)
	{
		glDeleteBuffers(1, &id);
	}
}


GLBuffer* GLBuffer::Create(EGLBufferType type, int size, EGLBufferUsage usage)
{
	GLBuffer* buffer = new GLBuffer();
	buffer->type = type;
	buffer->usage = usage;
	buffer->size = size;


	glGenBuffers(1, &buffer->id);
	glBindBuffer((GLENUM)type, buffer->id);
	glBufferData((GLENUM)type, size, nullptr, (GLENUM)usage);
	glBindBuffer((GLENUM)type, 0);

	return buffer;
}


GLBuffer* GLBuffer::Create(EGLBufferType type, int size, const void* data, EGLBufferUsage usage)
{
	GLBuffer* buffer = new GLBuffer();
	buffer->type = type;
	buffer->usage = usage;
	buffer->size = size;


	glGenBuffers(1, &buffer->id);
	glBindBuffer((GLENUM)type, buffer->id);
	glBufferData((GLENUM)type, size, data, (GLENUM)usage);
	glBindBuffer((GLENUM)type, 0);

	return buffer;
}


void GLBuffer::UpdateData(int dataSize, const void* data)
{
	size = dataSize;

	glBindBuffer((GLENUM)type, id);
	glBufferData((GLENUM)type, size, data, (GLENUM)usage);
	glBindBuffer((GLENUM)type, 0);
}


void GLBuffer::UpdateSubData(int dataSize, int offset, const void* data)
{
	RAVEN_ASSERT((offset + dataSize) <= size, "GLBuffer Update - Invalid Size.");

	glBindBuffer((GLENUM)type, id);
	glBufferSubData((GLENUM)type, offset, dataSize, data);
	glBindBuffer((GLENUM)type, 0);
}


void GLBuffer::Bind()
{
	glBindBuffer((GLENUM)type, id);
}


void GLBuffer::Unbind()
{
	glBindBuffer((GLENUM)type, 0);
}


void GLBuffer::BindBase(int binding)
{
	glBindBufferBase((GLENUM)type, binding, id);
}


void GLBuffer::BindRange(int binding, int offset, int size)
{
	glBindBufferRange((GLENUM)type, binding, id, offset, size);
}


} // End of namespace Raven.
