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




#include "IRenderResource.h"
#include "Render/OpenGL/GLTypes.h"
#include "ResourceManager/Resources/Texture.h"

#include "Utilities/Core.h"
#include "glm/vec2.hpp"



namespace Raven
{
	class GLTexture;




	// RenderRscTexture:
	//		-
	//
	class RenderRscTexture : public IRenderResource
	{
	public:
		// Construct.
		RenderRscTexture();

		// Destruct.
		~RenderRscTexture();

		// Return the OpenGL texture.
		inline const Ptr<GLTexture>& GetTexture() { return texture; }

		// Load a render texture resource.
		void Load(EGLTexture type, EGLFormat format, const glm::ivec2& size, void* data);

		// Create Texture 2D Render Resrouce.
		static RenderRscTexture* CreateTexture2D(ETextureFormat format, ETextureFilter filter, ETextureWrap wrap,
			const glm::ivec2& size, const void* data, bool isGenMipmaps);

		// Create Texture Cube Mpa.
		// @param data: contain all the faces of a cube contiguous in a single buffer.
		static RenderRscTexture* CreateTextureCube(ETextureFormat format, ETextureFilter filter, ETextureWrap wrap,
			const glm::ivec2& size, const void* data, bool isGenMipmaps);

		// Update a single texture level and layer.
		void UpdateData(int32_t level, int32_t layer, const glm::ivec2& size, const void* data);

		// Update a reagion of the texture.
		void UpdateSubData(int32_t level, int32_t layer, const glm::ivec2& offset, const glm::ivec2& size, const void* data);

		// Update
		void UpdateParamters(ETextureFilter filter, ETextureWrap wrap);

		// Return the current format of the opengl texture.
		ETextureFormat GetTexFormat();

	public:
		// Conver to format to opengl format.
		static EGLFormat ToGLType(ETextureFormat format);

		// Conver to format to opengl filter.
		static EGLFilter ToGLType(ETextureFilter filter);

		// Conver to format to opengl wrap.
		static EGLWrap ToGLType(ETextureWrap wrap);

	private:
		// The OpenGL Texture.
		Ptr<GLTexture> texture;
	};

}


