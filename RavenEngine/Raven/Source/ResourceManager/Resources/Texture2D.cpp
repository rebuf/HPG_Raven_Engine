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
#include "Texture2D.h"


#include "Render/RenderResource/RenderRscTexture.h"




namespace Raven {



void Raven::GetFormatInfo(ETextureFormat format, int32_t& BPP)
{
	BPP = 0;

	switch (format)
	{
	case Raven::ETextureFormat::R8:
		BPP = 8;
		break;

	case Raven::ETextureFormat::RGB24:
		BPP = 24;
		break;

	case Raven::ETextureFormat::RGBA32:
		BPP = 32;
		break;

	case Raven::ETextureFormat::R_Float:
		BPP = 32;
		break;

	case Raven::ETextureFormat::RGB_Float:
		BPP = 32 * 3;
		break;
	}

}


// --- -- - --- -- - --- -- - --- -- - --- -- - --- -- - --- -- - --- -- - --- -- - 


Texture2D::Texture2D()
{
	type = Texture2D::StaticGetType();

}


Texture2D::~Texture2D()
{

}



void Texture2D::LoadRenderResource()
{
	RAVEN_ASSERT(!isOnGPU, "Resrouce already on GPU. use UpdateRenderRsc to update.");

	renderRsc = Ptr<RenderRscTexture>( RenderRscTexture::CreateTexture2D(
		format, 
		filter, 
		wrap,
		size, 
		data.GetData(), // Data should be in OpenGL accepted format.
		isGenMipmaps
	));

	isOnGPU = true;
}


void Texture2D::UpdateRenderResource()
{
	RAVEN_ASSERT(isOnGPU, "Resrouce not on GPU. use LoadRenderRsc to load it first.");
	// TODO: update.

}


void Texture2D::UpdateParamters()
{
	RAVEN_ASSERT(isOnGPU, "Resrouce not on GPU. use LoadRenderRsc to load it first.");
	renderRsc->UpdateParamters(filter, wrap);
}


void Texture2D::SetImageData(ETextureFormat imgFormat, const glm::ivec2& imgSize, uint8_t* imgData)
{
	format = imgFormat;
	size = imgSize;

	// Compute the allocate size.
	int32_t BPP = 0; // bit per pixel
	Raven::GetFormatInfo(imgFormat, BPP);

	uint32_t allocateSize = (BPP >> 3) * size.x * size.y;
	data.Allocate(allocateSize);
	data.SetData(0, allocateSize, imgData);
}




} // End of namespace Raven

