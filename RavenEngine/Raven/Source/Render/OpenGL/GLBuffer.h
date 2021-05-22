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





namespace Raven
{



	//	OpenGL buffer.
	//
	class GLBuffer
	{
	private:
		// Privte construct, use GLBuffer::Create to create buffer objects.
		GLBuffer();

	public:
		// Destruct.
		~GLBuffer();

		// Create a new GLBuffer and allocated data of size. 
		static GLBuffer* Create(EGLBufferType type, int size, EGLBufferUsage usage);

		// Create a new GLBuffer with data. 
		static GLBuffer* Create(EGLBufferType type, int size, const void* data, EGLBufferUsage usage);

		// Reallocate and update new data for the buffer.
		void UpdateData(int dataSize, const void* data);

		// Update buffer with a new data.
		void UpdateSubData(int dataSize, int offset, const void* data);

		// Return the current size of the buffer.
		inline int GetSize() const { return size; }

		// Return the current type of the buffer.
		inline EGLBufferType GetType() const { return type; }

		// Return the current usage of the buffer.
		inline EGLBufferUsage GetUsage() const { return usage; }

		// Return the opengl id of the buffer.
		inline GLUINT GetID() const { return id; }

		// Bind the buffer to its target type.
		void Bind();

		// Unbind all buffers from target type.
		void Unbind();

		// Bind a buffer to a specific binding index, used by Unifrom Buffers to bind it to.
		void BindBase(int binding);

		// Bind a buffer to a specific binding index, used by Unifrom Buffers to bind it to.
		void BindRange(int binding, int offset, int size);

	private:
		// The type of the buffer.
		EGLBufferType type;

		// The usage of the buffer.
		EGLBufferUsage usage;

		// The current size of the buffer.
		int size;

		// OpenGL Object ID.
		GLUINT id;
	};


}




