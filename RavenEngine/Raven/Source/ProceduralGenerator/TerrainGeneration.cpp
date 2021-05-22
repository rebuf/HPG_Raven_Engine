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
#include "TerrainGeneration.h"


#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"


namespace Raven {




TerrainGeneration::TerrainGeneration()
{
}


TerrainGeneration::~TerrainGeneration()
{
}


void TerrainGeneration::Initialize()
{
}


void TerrainGeneration::Destroy()
{
}



Ptr<HeightMap> TerrainGeneration::GenerateHeightMap(int32_t width, int32_t height)
{
	GenerateSquareGradient(width, height);
	GenerateNoise(width, height);

	Ptr<HeightMap> heightMap(new HeightMap());
	heightMap->Allocate(width, height);

	float min = FLT_MAX;
	float max =-FLT_MAX;


	float fw = 1.0f / (octavesFactors[0] + octavesFactors[1] + octavesFactors[2] + octavesFactors[3]);

	// add heights from all octaves
	float sumHeight = 0;


	// compute the average height and store as RGB data
	for (int row = 0; row < height; row++)
	{
		for (int col = 0; col < width; col++)
		{
			sumHeight = 0;

			for (int oct = 0; oct < octaves; oct++)
			{
				float goct = data[((row * width + col) * octaves) + oct];
				sumHeight += goct * octavesFactors[oct];
			}

			sumHeight *= fw;
			heightMap->SetValue(row, col, sumHeight);

			min = glm::min(min, sumHeight);
			max = glm::max(max, sumHeight);
		}
	}

	float newHeight;


	for (int row = 0; row < height; row++)
	{
		for (int col = 0; col < width; col++)
		{
			// subtract square gradient from original height.
			newHeight = (heightMap->GetValue(row, col) - min) / (max - min); // Normalize [0.0, 1.0]
			newHeight = newHeight * squareGradient[row * width + col];

			// set negative heights to 0
			newHeight = newHeight < 0 ? 0 : newHeight;

			// invert heights (render has 0 as the tallest)
			//newHeight = 1.0f - newHeight;

			heightMap->SetValue(row, col, newHeight);
		}
	}

	

	Ptr<HeightMap> smoothHeightMap(new HeightMap());
	smoothHeightMap->Allocate(width, height);

	for (int gg = 0; gg < 1; gg++)
	{
		for (int row = 0; row < height; row++)
		{
			for (int col = 0; col < width; col++)
			{
				float s = 0.0f;

				for (int x = -1; x <= 1; x++)
				{
					int sx = glm::clamp(row + x, 0, width - 1);

					for (int y = -1; y <= 1; y++)
					{
						int sy = glm::clamp(col + y, 0, height - 1);
						s += heightMap->GetValue(sx, sy);
					}
				}


				s /= 9.0f;
				smoothHeightMap->SetValue(row, col, s);
			}
		}
	}






	// deallocate memory
	free(squareGradient);
	free(data);

	return smoothHeightMap;
}



// Create a square gradient for making an island
void TerrainGeneration::GenerateSquareGradient(int width, int height)
{
	int halfWidth = width / 2;
	int halfHeight = height / 2;

	glm::vec2 center(width * 0.5f, height * 0.5f);
	float l = glm::min(center.x, center.y) * 2.0;
	float nl = 1.0f / l;


	squareGradient = (float*)malloc(width * height * sizeof(float));

	for (int row = 0; row < height; row++)
	{
		for (int col = 0; col < width; col++)
		{
			glm::vec2 v((float)row, (float)col);
			float value = 1.0 - glm::length(v - center) * nl;
			value = glm::clamp(0.0f, 1.0f, value);

			// use a higher power (value^3) to increase the area of the black centre
			value = glm::smoothstep(gradiantE, gradiantS, value + 0.1f);
			value = value * value;

			squareGradient[row * width + col] = value;
		}
	}

}



void TerrainGeneration::GenerateNoise(int width, int height)
{	



	bool periodic = false;	// for seamless noise

	data = (float*)malloc(width * height * octaves * sizeof(float));

	float xFactor = 1.0f / (width - 1);
	float yFactor = 1.0f / (height - 1);

	for (int row = 0; row < height; row++)
	{
		for (int col = 0; col < width; col++)
		{
			float x = xFactor * col;
			float y = yFactor * row;
			float sum = 0.0f;
			float freq = a;
			float scale = 1.0;

			// compute the sum for each octave
			for (int oct = 0; oct < octaves; oct++)
			{
				glm::vec2 p(x * freq + seedOffset.x, y * freq + seedOffset.y);
				float val = 0.0f;
				// periodic for seamless noise
				if (periodic) {	
					val = glm::perlin(p, glm::vec2(freq)) / scale;
				}
				// normal
				else {
					val = glm::perlin(p) / scale;
				}
				//sum += val;
				//float result = (sum + 1.0f) / 2.0f;

				// store in texture buffer
				data[((row * width + col) * octaves) + oct] = val;

				freq *= freqFactor; // the frequency
				scale *= b; // next power of b
			}
		}
	}

}


bool TerrainGeneration::WriteHeightMap(HeightMap* map, const std::string& path)
{
	int32_t width = map->GetSize().x;
	int32_t height = map->GetSize().x;
	int32_t pixelCount = width * height;

	float* heightData = map->GetHeightMapData();
	uint8_t* data = (uint8_t*)malloc(width * height * sizeof(uint8_t));

	for (int32_t i = 0; i < pixelCount; ++i)
	{
		data[i] = (uint8_t)(heightData[i] * 255.0f);
	}

	int success = stbi_write_png(path.c_str(), width, height, 1, data, width);	// last parameter is stride in bytes

	free(data);
	return success;
}


bool TerrainGeneration::WriteHeightMapNormal(HeightMap* map, const std::string& path)
{
	int32_t width = map->GetSize().x;
	int32_t height = map->GetSize().y;

	int32_t pixelCount = width * height;
	uint8_t* data = (uint8_t*)malloc(width * height * 3 * sizeof(uint8_t));

	for (int32_t i = 0; i < pixelCount; ++i)
	{
		int32_t x = i % width;
		int32_t y = i / width;
		glm::vec3 n = map->GetNormal(x, y);

		int32_t pi = i * 3;
		data[pi]   = (uint32_t)((n.x * 0.5f + 0.5f) * 255.0f);
		data[pi+1] = (uint32_t)((n.z * 0.5f + 0.5f) * 255.0f);
		data[pi+2] = (uint32_t)((n.y * 0.5f + 0.5f) * 255.0f);
	}

	int success = stbi_write_png(path.c_str(), width, height, 3, data, width * 3);	// last parameter is stride in bytes

	free(data);
	return success;
}



void TerrainGeneration::WriteImage(FileFormat type, int width, int height, const uint8_t* data)
{
	// write heights to different image formats
	switch (type)
	{
	case FileFormat::PNG:
		stbi_write_png("heightmap.png", width, height, 1, data, width);	// last parameter is stride in bytes
		break;
	case FileFormat::BMP:
		stbi_write_bmp("heightmap.bmp", width, height, 1, data);
		break;
	case FileFormat::JPG:
		stbi_write_jpg("heightmap.jpg", width, height, 1, data, 100);	// int quality
		break;
	case FileFormat::TGA:
		stbi_write_tga("heightmap.tga", width, height, 1, data);
		break;
	}
}



}