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

#include "Logger/Console.h"
#include "HeightMap.h"

#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>
#include <iostream>
#include <fstream>
#include <vector>




namespace Raven {

	enum class FileFormat
	{
		PNG, BMP, JPG, TGA
	};

	class TerrainGeneration
	{
	public:
		// buffer to store squareGradient data 
		float* squareGradient;

		// buffer to store noise data
		float* data;

		// constructor
		TerrainGeneration();

		// destructor
		~TerrainGeneration();

		// initialise module
		void Initialize();

		// destroy module
		void Destroy();

		// Generate terrain heightmap.
		Ptr<HeightMap> GenerateHeightMap(int32_t width, int32_t height);

		// create a square gradient for island
		void GenerateSquareGradient(int width, int height);

		// create terrain noise
		void GenerateNoise(int width, int height);

		// write out image in the specified format
		bool WriteHeightMap(HeightMap* map, const std::string& path);
		bool WriteHeightMapNormal(HeightMap* map, const std::string& path);

		// write out image in the specified format
		void WriteImage(FileFormat type, int width, int height, const uint8_t* data);

	public:
		// --- -- - --- -- - --- -- - --
		//       Gen-Paramters
		// --- - -- - -- - -- - -- - ---

		// higher value increases bumpiness; originally 0.5
		float a = 3.7;

		// a needs to be much higher than b for a bumpy terrain
		// higher value increases smoothness; originally 1.0
		float b = 2.0;

		// frequency factor, scale frequancy each octave.
		float freqFactor = 2.7f;

		// Offset to the noise seed.
		glm::vec2 seedOffset = glm::vec2(9000.0f, 500.0f);

		// Number of octaves.
		int octaves = 4;

		//
		float gradiantS = 0.95f;
		float gradiantE = 0.6f;

		//
		std::vector<float> octavesFactors = { 1.7f, 0.9f, 0.2f, 0.04f };
	};


	// Scene 1:
	// seedOffset = glm::vec2(90.0f, 500.0f);
	// glm::vec2 seedOffset = glm::vec2(900.0f, 500.0f);

}