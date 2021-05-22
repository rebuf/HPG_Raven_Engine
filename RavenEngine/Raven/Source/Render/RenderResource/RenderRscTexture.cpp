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
#include "RenderRscTexture.h"
#include "Render/OpenGL/GLTexture.h"



#include "Utilities/Core.h"



namespace Raven {



RenderRscTexture::RenderRscTexture()
{

}


RenderRscTexture::~RenderRscTexture()
{

}


EGLFormat RenderRscTexture::ToGLType(ETextureFormat format)
{
	switch (format)
	{
	case ETextureFormat::R8: return EGLFormat::R;
	case ETextureFormat::RGB24: return EGLFormat::RGB;
	case ETextureFormat::RGBA32: return EGLFormat::RGBA;
	case ETextureFormat::R_Float: return EGLFormat::R16F;
	case ETextureFormat::RGB_Float: return EGLFormat::RGB16F;
	}

	return EGLFormat::None;
}


EGLFilter RenderRscTexture::ToGLType(ETextureFilter filter)
{
	switch (filter)
	{
	case ETextureFilter::Nearest: return EGLFilter::Nearest;
	case ETextureFilter::Linear: return EGLFilter::Linear;
	case ETextureFilter::TriLinear: return EGLFilter::TriLinear;
	}

	return EGLFilter::Nearest;
}


EGLWrap RenderRscTexture::ToGLType(ETextureWrap wrap)
{
	switch (wrap)
	{
	case Raven::ETextureWrap::Repeat: return EGLWrap::Repeat;
	case Raven::ETextureWrap::Mirror: return EGLWrap::Mirror;
	case Raven::ETextureWrap::ClampToEdge: return EGLWrap::ClampToEdge;
	}

	return EGLWrap::ClampToEdge;
}


void RenderRscTexture::Load(EGLTexture type, EGLFormat format, const glm::ivec2& size, void* data)
{
	texture = Ptr<GLTexture>( GLTexture::Create(type, format) );
	
	switch (type)
	{
	case EGLTexture::Texture2D:
	{
		texture->Bind();
		texture->UpdateTexData(0, size.x, size.y, data);
		texture->UpdateTexParams();
		texture->Unbind();
	}
			break;

	case EGLTexture::CubeMap:
	{
		texture->Bind();

		for (int32_t f = 0; f < 6; ++f)
		{
			uint8_t* faceData = (uint8_t*)data;

			// Not Null?
			if (data)
			{
				faceData += f * size.x * size.y * (GLTexture::GetBPP(format) >> 3);
			}

			texture->UpdateTexData(0, size.x, size.y, f, faceData);
		}

		texture->UpdateTexParams();
		texture->Unbind();
	}
		break;

	default: // Not Supported...
		RAVEN_ASSERT(0, "RenderRscTexture - Texture type not supported.");
		break;
	}

	
}


RenderRscTexture* RenderRscTexture::CreateTexture2D(ETextureFormat format, ETextureFilter filter, ETextureWrap wrap,
	const glm::ivec2& size, const void* data, bool isGenMipmaps)
{
	EGLFormat formatGL = ToGLType(format);
	EGLFilter filterGL = ToGLType(filter);
	EGLWrap wrapGL = ToGLType(wrap);

	RenderRscTexture* rsc = new RenderRscTexture();
	rsc->texture = Ptr<GLTexture>(GLTexture::Create(EGLTexture::Texture2D, formatGL));
	rsc->texture->SetFilter(filterGL);
	rsc->texture->SetWrap(wrapGL);

	rsc->texture->Bind();
	rsc->texture->UpdateTexData(0, size.x, size.y, data);
	rsc->texture->UpdateTexParams();

	if (isGenMipmaps)
	{
		rsc->texture->GenerateMipmaps();
	}

	rsc->texture->Unbind();

	return rsc;
}


RenderRscTexture* RenderRscTexture::CreateTextureCube(ETextureFormat format, ETextureFilter filter, ETextureWrap wrap,
	const glm::ivec2& size, const void* data, bool isGenMipmaps)
{
	EGLFormat formatGL = ToGLType(format);
	EGLFilter filterGL = ToGLType(filter);
	EGLWrap wrapGL = ToGLType(wrap);

	RenderRscTexture* rsc = new RenderRscTexture();
	rsc->texture = Ptr<GLTexture>(GLTexture::Create(EGLTexture::CubeMap, formatGL));
	rsc->texture->SetFilter(filterGL);
	rsc->texture->SetWrap(wrapGL);

	rsc->texture->Bind();

	for (int32_t f = 0; f < 6; ++f)
	{
		uint8_t* faceData = (uint8_t*)data;

		// Not Null?
		if (data)
		{
			faceData += f * size.x * size.y * (GLTexture::GetBPP(formatGL) >> 3);
		}

		rsc->texture->UpdateTexData(0, size.x, size.y, f, faceData);
	}

	rsc->texture->UpdateTexParams();

	if (isGenMipmaps)
	{
		rsc->texture->GenerateMipmaps();
	}

	rsc->texture->Unbind();

	return rsc;
}


void RenderRscTexture::UpdateData(int32_t level, int32_t layer, const glm::ivec2& size, const void* data)
{
	texture->Bind();
	texture->UpdateTexData(level, size.x, size.y, layer, data);
	texture->Unbind();
}


void RenderRscTexture::UpdateSubData(int32_t level, int32_t layer, const glm::ivec2& offset, const glm::ivec2& size, const void* data)
{
	texture->Bind();
	texture->UpdateTexSubData(level, offset.x, offset.y, size.x, size.y, layer, data);
	texture->Unbind();
}



void RenderRscTexture::UpdateParamters(ETextureFilter filter, ETextureWrap wrap)
{
	EGLFilter filterGL = ToGLType(filter);
	EGLWrap wrapGL = ToGLType(wrap);

	texture->Bind();
	texture->SetFilter(filterGL);
	texture->SetWrap(wrapGL);
	texture->UpdateTexParams();
	texture->Unbind();
}


ETextureFormat RenderRscTexture::GetTexFormat()
{
	switch (texture->GetFormat())
	{
	case Raven::EGLFormat::R: return ETextureFormat::R8;
	case Raven::EGLFormat::RGB: return ETextureFormat::RGB24;
	case Raven::EGLFormat::RGBA: return ETextureFormat::RGBA32;

	case Raven::EGLFormat::R16F: return ETextureFormat::R_Float;
	case Raven::EGLFormat::RGB16F: return ETextureFormat::RGB_Float;
	}

	return ETextureFormat::MAX_FORMAT;
}



} // End of namespace Raven.


