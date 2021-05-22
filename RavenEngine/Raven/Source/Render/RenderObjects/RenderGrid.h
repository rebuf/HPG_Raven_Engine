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
#include "glm/vec3.hpp"
#include "glm/vec2.hpp"
#include "glm/matrix.hpp"

#include <vector>



namespace Raven
{
	class GLVertexArray;
	class GLBuffer;
	class GLShader;
	class RenderRscShader;



	// RenderGrid:
	//    - 
	//
	class RenderGrid
	{
		// Cosntruct.
		RenderGrid();

	public:
		// Destruct.
		~RenderGrid();

		// Create a rende screen.
		static RenderGrid* Create();

		// Draw.
		void Draw();

	private:
		// Create sphere geometry.
		static void MakeGrid(std::vector<glm::vec3>& outVerts);

	private:
		// The OpenGL Vertex Array of the mesh, defines mesh vertex input.
		GLVertexArray* vxArray;

		// OpenGL Buffer for Sphere Positions.
		GLBuffer* vxBuffer;

		// Number of vertices in the grid.
		int32_t numVerts;

		// Grid Shader.
		Ptr<RenderRscShader> shader;
	};

}


