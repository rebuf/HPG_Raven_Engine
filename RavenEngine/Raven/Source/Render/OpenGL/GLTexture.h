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

	//
	//
	class GLTexture
	{
	private:
		// Construct.
		GLTexture();

	public:
		// Destruct.
		~GLTexture();

		// Create texture.
		static GLTexture* Create(EGLTexture type, EGLFormat format);

		// Create a texture and allocate data of size.
		static GLTexture* Create2D(EGLFormat format, int width, int height, EGLFilter filter = EGLFilter::Nearest, EGLWrap wrap = EGLWrap::ClampToEdge);

		// Create a texture and copy the data of the image.
		static GLTexture* Create2D(EGLFormat format, int width, int height, const void* data, EGLFilter filter = EGLFilter::Nearest, EGLWrap wrap = EGLWrap::ClampToEdge);

		// Update Texture data depending on its type
		// Note: this assume that the texture is currently bounded.
		void UpdateTexData(int level, int newWidth, int newHeight, const void* data);

	  // Update Texture data depending on its type and with multiple layers
		// Note: this assume that the texture is currently bounded.
		void UpdateTexData(int level, int newWidth, int newHeight, int layer, const void* data);

		// Update a part of an existing texture.
		// Note: this assume that the texture is currently bounded.
		void UpdateTexSubData(int level, int offsetx, int offsety, int newWidth, int newHeight, int layer, const void* data);

		// Update Texture parameters
	  // Note: this assume that the texture is currently bounded.
		void UpdateTexParams();

		// Generate texture mipmaps using glGenerateMipmaps
		// Note: this assume that the texture is currently bounded.
		void GenerateMipmaps();

		// Bind the texture to its target type.
		void Bind();

		// Unbind all buffers from target type.
		void Unbind();

		// Pixel information based on the texture format.
		static void GetPixelInfo(EGLFormat format, GLENUM& pixelFormat, GLENUM& pixelType, int& alignment);

		// Return format Bit Per Pixel.
		static int GetBPP(EGLFormat format);
		 
		// Return the opengl id of the buffer.
		inline GLUINT GetID() const { return id; }

		// Return texture type.
		inline EGLTexture GetType() const { return type; }

		// Return texture format.
		inline EGLFormat GetFormat() const { return format; }

		// Return texture format.
		inline EGLFilter GetFilter() const { return filter; }

		// Return texture height.
		inline int GetHeight() const { return height; }

		// Return texture width.
		inline int GetWidth() const { return width; }

		// Bind this texture and set it as active for TEXTURE(i)
		void Active(int i);

		// Set texture sampling filter.
		void SetFilter(EGLFilter value);

		// Set texture wrap mode.
		void SetWrap(EGLWrap value);

		// Set min & max mip levels.
		void SetMipLevels(int base, int max);

		// Set the border color for the texture.
		// Note: this assume that the texture is currently bounded.
		void BorderColor(float r, float g, float b, float a);

	private:
		// OpenGL Object ID.
		GLUINT id;

		// texture type.
		EGLTexture type;

		// texture format.
		EGLFormat format;

		// sample filter
		EGLFilter filter;

		// sample wrap
		EGLWrap wrap;

		// Texture width.
		int width;

		// Texture height.
		int height;

		// The first mip level of this texture.
		int baseMipLevel;

		// The maximum mip level of this texture.
		int maxMipLevel;
	};

}

