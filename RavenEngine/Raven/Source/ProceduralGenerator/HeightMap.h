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



#include "Utilities/Core.h"
#include "ResourceManager/Resources/DynamicTexture.h"



namespace Raven
{
	// HeightMapData:
	//    - 
	template<class TData>
	struct HeightMapData
	{
	private:
		// Texture Buffer.
		TData* data;

		// The size of the data in bytes.
		uint32_t size;

	public:
		// Construct Null Data.
		HeightMapData()
			: data(nullptr)
			, size(0)
		{

		}

		// Destruct.
		~HeightMapData()
		{
			free(data);
		}

		// Allocate Texture Data.
		// @param size: the size of the data to allocate in bytes.
		inline void Allocate(uint32_t allocateSize)
		{
			RAVEN_ASSERT(data == nullptr, "Invalid Operation, trying to reallocate data.");
			data = (TData*)malloc(allocateSize);
			size = allocateSize;
		}

		// Set Image Data.
		void SetData(uint32_t inOffset, uint32_t inSize, TData* inData)
		{
			RAVEN_ASSERT(size >= (inOffset + inSize), "Invalid Operation, trying to reallocate data.");
			memcpy(data + inOffset, inData, inSize);
		}

		// Return the texture data.
		inline TData* GetData() { return data; }
		inline const TData* GetData() const { return data; }

		// Return the allocated size for the image data.
		inline const uint32_t& GetSize() const { return size; }

		// Reset/Free the data.
		inline void Reset()
		{
			free(data);
			data = nullptr;
			size = 0;
		}
	};



	// HeightMap:
	//    - 
	//
	class HeightMap
	{
	public:
		// Construct.
		HeightMap();

		// Destruct.
		~HeightMap();

		// Allocate height map data.
		void Allocate(int32_t width, int32_t height);

		// Set the height value.
		void SetValue(int32_t x, int32_t y, float value) { heightField.GetData()[x + y * size.x] = value; }

		// Return the height value.
		inline float& GetValue(int32_t x, int32_t y) { return heightField.GetData()[x + y * size.x]; }
		inline const float& GetValue(int32_t x, int32_t y) const { return heightField.GetData()[x + y * size.x]; }

		// Return the height tangent.
		inline const glm::vec2& GetTangent(int32_t x, int32_t y) const { return tangents.GetData()[x + y * size.x]; }

		// Return height field data.
		inline float* GetHeightMapData() { return heightField.GetData(); }
		inline const float* GetHeightMapData() const { return heightField.GetData(); }

		// Return height field size.
		inline const glm::ivec2& GetSize() const { return size; }

		// Return height map texture used for rendering.
		inline DynamicTexture* GetHeightmapTexture() { return heightmapTexture.get(); }
		inline const DynamicTexture* GetHeightmapTexture() const { return heightmapTexture.get(); }

		// Compute tangents from height field.
		void ComputeTangents();

		// Generate Height Map Dynamic Texture to be used in renderings.
		void GenerateTexture();

		// Get normal vector at a height field location.
		glm::vec3 GetNormal(int32_t x, int32_t y) const;

		// Get Height.
		float GetHeight(float x, float y) const;

		// Set the height scale.
		inline void SetHeightScale(const glm::vec2& inScale) 
		{ 
			heightScale = inScale; 
			scale = inScale.y - inScale.x; 
		}

		// Set the height field size.
		inline void SetSizeScale(const glm::vec2& inScale)
		{
			sizeScale = inScale;
		}

	private:
		// The width and height of the height map.
		glm::ivec2 size;

		// The height field in floating points.
		HeightMapData<float> heightField;
		 
		// Tangents of the height field.
		HeightMapData<glm::vec2> tangents;

		// The height map texture we are going to use for rendering.
		Ptr<DynamicTexture> heightmapTexture;

		// Min/Max height scale.
		glm::vec2 heightScale;

		// The scaling applied to the height (Max-Min).
		float scale;

		//
		glm::vec2 sizeScale;
	};

}