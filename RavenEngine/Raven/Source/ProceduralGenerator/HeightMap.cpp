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
#include "HeightMap.h"




namespace Raven {



HeightMap::HeightMap()
{

}


HeightMap::~HeightMap()
{

}


void HeightMap::Allocate(int32_t width, int32_t height)
{
	size = glm::ivec2(width, height);
	heightField.Allocate(width * height * sizeof(float));
}


void HeightMap::GenerateTexture()
{
	// Convert the height map into gl texture format.
	int32_t numPixels = size.x * size.y;
	const float* heightData = heightField.GetData();
	const glm::vec2* tangentsData = tangents.GetData();
	uint8_t* data = (uint8_t*)malloc(size.x * size.y * 4 * sizeof(uint8_t));

	for (int32_t i = 0; i < numPixels; ++i)
	{
		int32_t pi = i * 4;
		data[pi] = (uint8_t)(heightData[i] * 255.0f);
		data[pi + 1] = (uint8_t)0.0f;

		data[pi + 2] = (uint8_t)(((tangentsData[i].x * scale * 0.5f) * 0.5f + 0.5f) * 255.0f);
		data[pi + 3] = (uint8_t)(((tangentsData[i].y * scale * 0.5f) * 0.5f + 0.5f) * 255.0f);
	}

	// Create Texture.
	heightmapTexture = Ptr<DynamicTexture>(new DynamicTexture());
	heightmapTexture->SetTexType(EDynamicTexture::Tex2D);
	heightmapTexture->SetGenMipmaps(true);
	heightmapTexture->SetFitler(ETextureFilter::TriLinear);
	heightmapTexture->SetWrap(ETextureWrap::Mirror);
	heightmapTexture->UpdateData(size, ETextureFormat::RGBA32, data);


	// Free...
	free(data);
}


void HeightMap::ComputeTangents()
{
	tangents.Allocate(size.x * size.y * sizeof(glm::vec2));
	glm::vec2* tanegentData = tangents.GetData();

	for (int32_t y = 0; y < size.y; ++y)
	{
		for (int32_t x = 0; x < size.x; ++x)
		{
			int sx0 = glm::clamp(x - 1, 0, size.x - 1);
			int sx1 = glm::clamp(x + 1, 0, size.x - 1);
			int sy0 = glm::clamp(y - 1, 0, size.y - 1);
			int sy1 = glm::clamp(y + 1, 0, size.y - 1);

			float hx0 = GetValue(x, y);
			float hx1 = GetValue(sx1, y);

			float hy0 = GetValue(x, y);
			float hy1 = GetValue(x, sy1);

			glm::vec2& tangent = tanegentData[x + y * size.x];
			tangent.x = (hx0 - hx1);
			tangent.y = (hy0 - hy1);
		}
	}

}


glm::vec3 HeightMap::GetNormal(int32_t x, int32_t y) const
{
	glm::vec2 t = GetTangent(x, y);
	t.x = t.x * scale * 0.5f;
	t.y = t.y * scale * 0.5f;
	return glm::normalize( glm::vec3(t.x, 1.0f, t.y) );
}


float HeightMap::GetHeight(float x, float y) const
{
	x /= sizeScale.x;
	y /= sizeScale.y;

	x = glm::clamp(x, 0.0f, 1.0f) * (float)size.x;
	y = glm::clamp(y, 0.0f, 1.0f) * (float)size.y;

	float fx = floor(x);
	float fy = floor(y);
	float cx = ceil(x);
	float cy = ceil(y);

	float h0 = GetValue((int32_t)fx, (int32_t)fy);
	float h1 = GetValue((int32_t)cx, (int32_t)fy);
	float h2 = GetValue((int32_t)fx, (int32_t)cx);
	float h3 = GetValue((int32_t)cx, (int32_t)cx);

	float ha = h0 + h1;
	float hb = h2 + h3;

	float h = h0 * (heightScale.y - heightScale.x) - heightScale.x;

	return h;
}


}

