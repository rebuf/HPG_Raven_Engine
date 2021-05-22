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



#include "Texture.h"



namespace Raven
{

	enum class EDynamicTexture
	{
		None, Tex2D, TexCube
	};


	// DynamicTexture:
	//    - any type of texture loaded at run time with dynamic data. does not store any data on cpu
	//      it passes the data directly to OpenGL.
	//
	//    - Note: you must load the render resrouce before any data update. @see LoadRenderResource().
	//
	class DynamicTexture : public ITexture
	{
	public:
		// Construct.
		DynamicTexture();

		// Destruct.
		virtual ~DynamicTexture();

		// return the resource type
		inline static EResourceType StaticGetType() noexcept { return EResourceType::RT_DynamicTexture; }

		// This texture is dynamic, Can't Be loaded this way. @see DynamicTexture::UpdateData().
		virtual void LoadRenderResource() override { RAVEN_ASSERT(0, "Invalid Operation."); }

		// This texture is dynamic, Can't Be updated this way. @see DynamicTexture::UpdateData().
		virtual void UpdateRenderResource() override { RAVEN_ASSERT(0, "Invalid Operation."); }

		// Get/Set dynamic texture type.
		inline EDynamicTexture GetTexType() const { return texType; }
		inline void SetTexType(EDynamicTexture type) { RAVEN_ASSERT(!isOnGPU, "Can't Change type once loaded."); texType = type; }

		// Update or Create dynamic texture data and loaded the render resrouce directly.
		void UpdateData(glm::ivec2& size, ETextureFormat format, const void* data);

		// Update a reagion
		void UpdateSubData(const glm::ivec2& offset, const glm::ivec2& size, const void* data);

		// Update texture paramters for the Render Resrouce.
		virtual void UpdateParamters() override;

	private:
		// The Dynamic Texture Type.
		EDynamicTexture texType;
	};

}


