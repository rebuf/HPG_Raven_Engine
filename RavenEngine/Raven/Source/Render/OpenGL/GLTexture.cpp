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
#include "GLTexture.h"
#include "Utilities/Core.h"

#include "GL/glew.h"





namespace Raven {





GLTexture::GLTexture()
	: id(0)
	, type(EGLTexture::None)
	, format(EGLFormat::None)
	, filter(EGLFilter::Nearest)
	, wrap(EGLWrap::ClampToEdge)
	, baseMipLevel(0)
	, maxMipLevel(1000)
{

}


GLTexture::~GLTexture()
{
	if (id != 0)
	{
		glDeleteTextures(1, &id);
	}
}


GLTexture* GLTexture::Create(EGLTexture type, EGLFormat format)
{
	GLTexture* tex = new GLTexture();

	tex->type = type;
	tex->format = format;
	tex->width = 0;
	tex->height = 0;
	glGenTextures(1, &tex->id);

	return tex;
}


GLTexture* GLTexture::Create2D(EGLFormat format, int width, int height, EGLFilter filter, EGLWrap wrap)
{
	GLTexture* tex = new GLTexture();

	tex->type = EGLTexture::Texture2D;
	tex->format = format;
	tex->width = width;
	tex->height = height;
	tex->filter = filter;
	tex->wrap = wrap;
	glGenTextures(1, &tex->id);

	tex->Bind();
	tex->UpdateTexData(0, width, height, nullptr);
	tex->UpdateTexParams();
	tex->Unbind();

	return tex;
}


GLTexture* GLTexture::Create2D(EGLFormat format, int width, int height, const void* data, EGLFilter filter, EGLWrap wrap)
{
	GLTexture* tex = new GLTexture();

	tex->type = EGLTexture::Texture2D;
	tex->format = format;
	tex->width = width;
	tex->height = height;
	tex->filter = filter;
	tex->wrap = wrap;
	glGenTextures(1, &tex->id);

	tex->Bind();
	tex->UpdateTexData(0, width, height, data);
	tex->UpdateTexParams();
	tex->Unbind();

	return tex;
}


void GLTexture::UpdateTexData(int level, int newWidth, int newHeight, const void* data)
{
	UpdateTexData(level, newWidth, newHeight, 0, data);
}


void GLTexture::UpdateTexData(int level, int newWidth, int newHeight, int layer, const void* data)
{
	width = newWidth;
	height = newHeight;

	// Get pixel information about based on the current format
	GLENUM pixelFormat;
	GLENUM pixelType;
	int alignment = 4;
	GetPixelInfo(format, pixelFormat, pixelType, alignment);

	// Pixel Data Alignment...
	if (alignment != 4)
		glPixelStorei(GL_UNPACK_ALIGNMENT, alignment);


	// Update Texture Data...
	if (type == EGLTexture::CubeMap)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + layer, level, (GLENUM)format, width, height, 0, pixelFormat, pixelType, data);
	}
	else
	{
		RAVEN_ASSERT(layer == 0, "Layer should be zero for non-layered textuers.");
		glTexImage2D((GLENUM)type, level, (GLENUM)format, width, height, 0, pixelFormat, pixelType, data);
	}


	// Reset alignment back to default.
	if (alignment != 4)
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
}


void GLTexture::UpdateTexSubData(int level, int offsetx, int offsety, int newWidth, int newHeight, int layer, const void* data)
{
	width = newWidth;
	height = newHeight;

	// Get pixel information about based on the current format
	GLENUM pixelFormat;
	GLENUM pixelType;
	int alignment = 4;
	GetPixelInfo(format, pixelFormat, pixelType, alignment);

	// Pixel Data Alignment...
	if (alignment != 4)
		glPixelStorei(GL_UNPACK_ALIGNMENT, alignment);


	// Update Texture Data...
	if (type == EGLTexture::CubeMap)
	{
		glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + layer, level, offsetx, offsety, width, height, (GLENUM)pixelFormat, pixelType, data);
	}
	else
	{
		RAVEN_ASSERT(layer == 0, "Layer should be zero for non-layered textuers.");
		glTexSubImage2D((GLENUM)type, level, offsetx, offsety, width, height, (GLENUM)pixelFormat, pixelType, data);
	}


	// Reset alignment back to default.
	if (alignment != 4)
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
}


void GLTexture::UpdateTexParams()
{
	EGLFilter magFilter = filter == EGLFilter::TriLinear ? EGLFilter::Linear : filter;

	glTexParameteri((GLENUM)type, GL_TEXTURE_MAG_FILTER, (GLINT)magFilter);
	glTexParameteri((GLENUM)type, GL_TEXTURE_MIN_FILTER, (GLINT)filter);
	glTexParameteri((GLENUM)type, GL_TEXTURE_WRAP_S, (GLINT)wrap);
	glTexParameteri((GLENUM)type, GL_TEXTURE_WRAP_T, (GLINT)wrap);
	glTexParameteri((GLENUM)type, GL_TEXTURE_BASE_LEVEL, baseMipLevel);
	glTexParameteri((GLENUM)type, GL_TEXTURE_MAX_LEVEL, maxMipLevel);
}


void GLTexture::GenerateMipmaps()
{
	glGenerateMipmap((GLENUM)type);
}


void GLTexture::Bind()
{
	glBindTexture((GLENUM)type, id);
}


void GLTexture::Unbind()
{
	glBindTexture((GLENUM)type, 0);
}


void GLTexture::GetPixelInfo(EGLFormat format, GLENUM& pixelFormat, GLENUM& pixelType, int& alignment)
{
	alignment = 1;

	switch (format)
	{
	case EGLFormat::Depth24:
	case EGLFormat::Depth32:
		pixelFormat = GL_DEPTH_COMPONENT;
		pixelType = GL_FLOAT;
		break;

	case EGLFormat::DepthStencil:
		pixelFormat = GL_DEPTH_STENCIL;
		pixelType = GL_UNSIGNED_INT_24_8;
		break;

	case EGLFormat::R16F:
		pixelFormat = GL_R;
		pixelType = GL_FLOAT;
		break;

	case EGLFormat::RG16F:
		pixelFormat = GL_RG;
		pixelType = GL_FLOAT;
		break;

	case EGLFormat::RGB16F:
		pixelFormat = GL_RGB;
		pixelType = GL_FLOAT;
		break;

	case EGLFormat::RGBA16F:
		pixelFormat = GL_RGBA;
		pixelType = GL_FLOAT;
		alignment = 4;
		break;

	case EGLFormat::RGBA:
		pixelFormat = GL_RGBA;
		pixelType = GL_UNSIGNED_BYTE;
		alignment = 4;
		break;

	default:
		pixelFormat = (GLENUM)format;
		pixelType = GL_UNSIGNED_BYTE;
		break;
	}

}


int GLTexture::GetBPP(EGLFormat format)
{
	switch (format)
	{
	case EGLFormat::R16F: return sizeof(float) * 8;
	case EGLFormat::RG16F: return sizeof(float) * 16;
	case EGLFormat::RGB16F: return sizeof(float) * 24;
	case EGLFormat::RGBA16F: return sizeof(float) * 32;

	case EGLFormat::R: return sizeof(uint8_t) * 8;
	case EGLFormat::RGB: return sizeof(uint8_t) * 24;
	case EGLFormat::RGBA: return sizeof(uint8_t) * 32;
	}

	RAVEN_ASSERT(0, "No BPP Info.");
	return -1;
}



void GLTexture::Active(int i)
{
	glActiveTexture(GL_TEXTURE0 + i);
	glBindTexture((GLENUM)type, id);
}


void GLTexture::SetFilter(EGLFilter value)
{
	filter = value;
}


void GLTexture::SetWrap(EGLWrap value)
{
	wrap = value;
}


void GLTexture::SetMipLevels(int base, int max)
{
	baseMipLevel = base;
	maxMipLevel = max;
}


void GLTexture::BorderColor(float r, float g, float b, float a)
{
	float color[4] = { r, g, b, a };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);
}


} // End of namespace Raven.
