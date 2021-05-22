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

#include "GLVertexArray.h"

#include "GL/glew.h"



namespace Raven {





GLVertexArray::GLVertexArray()
	: id(0)
{

}


GLVertexArray::~GLVertexArray()
{
	if (id != 0)
	{
		glDeleteVertexArrays(1, &id);
	}
}


GLVertexArray* GLVertexArray::Create()
{
	GLVertexArray* vxarray = new GLVertexArray();

	glGenVertexArrays(1, &vxarray->id);

	return vxarray;
}


void GLVertexArray::Build(const std::vector<GLVABuildAttribData>& data, const GLBuffer* elementBuffer)
{
	glBindVertexArray(id);


	// Attributes Bindings...
	for (const auto& vd : data)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vd.buffer->GetID());

		glVertexAttribPointer(
			vd.index,
			vd.size,
			(GLENUM)vd.type,
			GL_FALSE,
			vd.stride,
			(void*)(std::ptrdiff_t)vd.offset);

		glEnableVertexAttribArray(vd.index);

		if (vd.instance != 0)
		{
			glVertexAttribDivisor(vd.index, vd.instance);
		}
	}


	// Element Buffer?
	if (elementBuffer != nullptr)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer->GetID());
	}

	// Unbind...
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}


void GLVertexArray::Bind() const
{
	glBindVertexArray(id);
}


void GLVertexArray::Unbind() const
{
	glBindVertexArray(0);
}




} // End of namespace Raven.

