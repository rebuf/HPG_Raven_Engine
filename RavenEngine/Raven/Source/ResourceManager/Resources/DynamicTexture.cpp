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
#include "DynamicTexture.h"

#include "Render/RenderResource/RenderRscTexture.h"




namespace Raven {


DynamicTexture::DynamicTexture()
	: texType(EDynamicTexture::None)
{

}


DynamicTexture::~DynamicTexture()
{

}


void DynamicTexture::UpdateData(glm::ivec2& size, ETextureFormat format, const void* data)
{
	RAVEN_ASSERT(texType != EDynamicTexture::None, "Invalid Dynamic Texture Type.");

	if (!renderRsc)
	{
		this->size = size;
		this->format = format;

		if (texType == EDynamicTexture::Tex2D)
		{
			renderRsc = Ptr<RenderRscTexture>(RenderRscTexture::CreateTexture2D(
				format,
				filter,
				wrap,
				size,
				data,
				isGenMipmaps
			));
		}
		else if (texType == EDynamicTexture::TexCube)
		{
			renderRsc = Ptr<RenderRscTexture>(RenderRscTexture::CreateTextureCube(
				format,
				filter,
				wrap,
				size,
				data,
				isGenMipmaps
			));
		}
	}
	else
	{
		// Update Only...
		RAVEN_ASSERT(renderRsc->GetTexFormat() == format, "you can't change format.");
		renderRsc->UpdateSubData(0, 0, glm::ivec2(0), size, data);
	}
	

	// Make sure that updating is  successful.
	RAVEN_ASSERT(renderRsc != nullptr, "Failed to update.");
	isOnGPU = true;
}


void DynamicTexture::UpdateSubData(const glm::ivec2& offset, const glm::ivec2& size, const void* data)
{
	RAVEN_ASSERT(isOnGPU, "Must be updated first. @see DynamicTexture::UpdateData");
	renderRsc->UpdateSubData(0, 0, glm::ivec2(0), size, data);
}


void DynamicTexture::UpdateParamters()
{
	RAVEN_ASSERT(isOnGPU, "Must be updated first. @see DynamicTexture::UpdateData");
	renderRsc->UpdateParamters(filter, wrap);
}




} // End of namespace Raven


