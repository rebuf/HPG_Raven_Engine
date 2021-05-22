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



// Number of IBL prefiltered mip maps. 
#define NUM_IBL_SPEC_MIPS 5




namespace Raven
{
	class RenderSphere;
	class RenderScreen;
	class RenderPass;
	class RenderRscShader;
	class RenderRscTexture;
	class GLTexture;



	// RenderTexFilter:
	//		- provide functionalities for generate/pre-filter textures.
	//    - Filter IBL, 
	//
	//		- Only one instance created/managed by the RenderModule.
	//
	class RenderTexFilter
	{
		// Friend...
		friend class RenderModule;

		// Private Construct, @see RenderModule.
		RenderTexFilter(Ptr<RenderScreen> screen, Ptr<RenderSphere> sphere);

	public:
		// Destruct
		~RenderTexFilter();

		// Initializie the filters.
		void Initialize();

		// Destroyo the filters
		void Destroy();

		// Generate CubeMap texture from Equirectangular/Spherical Texturs.
		void GenCubeMap(RenderRscTexture* inTexture, RenderRscTexture* outCubeMap, bool isHDR);

		// Filter an ennvionment image for IBL irradiance.
		void FilterIrradianceIBL(RenderRscTexture* inEnvTexture, RenderRscTexture* outIrradiance);

		// Filter an ennvionment image for IBL specular.
		void FilterSpecularIBL(RenderRscTexture* inEnvTexture, RenderRscTexture* outSpecular);

		// Generate BRDF look up table.
		void GenBRDFLUT(RenderRscTexture* outBRDF);

		// Setup sky shader for reflection & filtering.
		void SetupSkyShader(RenderRscShader* shader);

		//
		void FilterSky(RenderRscShader* skyShader, RenderPass* skyPass, Ptr<GLTexture> skyEnv);

	private:
		// Create the shader used for converting spherical textures into cubemapss
		void CreateCubeMapGenShader();

		// Create the shader used for converting environment maps into filterd irradiance.
		void CreateIrradainceFilterShader();

		// Create the shader used for converting environment maps into filterd specular.
		void CreateSepcularFilterShader();

		// Create the shader used to create BRDF lookup texture.
		void CreateBRDFLUTShader();


	private:
		// Screen triangle used to render the entire screen.
		Ptr<RenderScreen> rscreen;

		// Sphere.
		Ptr<RenderSphere> rsphere;

		// Render Pass used by multiple filter opeations a single texture.
		Ptr<RenderPass> filterPass;

		// Shader for generating cubemap from spherical/Equirectangular textures.
		Ptr<RenderRscShader> cubeMapGenShader;

		// Shader for computing pre-filterd irradiance map.
		Ptr<RenderRscShader> filterIrradianceShader;

		// Shader for computing pre-filterd specular map.
		Ptr<RenderRscShader> filterSpecularShader[2];

		// Shader for generating BRDF Look up table.
		Ptr<RenderRscShader> brdfLUTShader;

		// Cube map views used for rendering sphere from different views.
		static std::pair<glm::vec3, glm::vec3> CUBE_MAP_VIEWS[6];
	};

}

