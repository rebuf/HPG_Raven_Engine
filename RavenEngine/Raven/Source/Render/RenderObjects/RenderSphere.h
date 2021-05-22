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




#include "glm/vec3.hpp"
#include "glm/vec2.hpp"
#include "glm/matrix.hpp"

#include <vector>



namespace Raven
{
	class GLVertexArray;
	class GLBuffer;
	class GLShader;


	// RenderScreen:
	//    - Render a sphere covering the entire screen.
	//
	class RenderSphere
	{
		// Cosntruct.
		RenderSphere();

	public:
		// Destruct.
		~RenderSphere();

		// Create a rende screen.
		static RenderSphere* Create();

		// Set View to use for rendering the sphere.
		void SetView(const glm::mat4& mtx);

		// Set Proj to use for rendering the sphere.
		void SetProj(const glm::mat4& mtx);

		// Draw.
		void Draw(GLShader* shader);

		// Draw.
		void DrawSky(GLShader* shader);


	private:
		// Create sphere geometry.
		static void MakeSphere(std::vector<glm::vec3>& outVerts, std::vector<uint32_t>& outIndices);

	private:
		// The OpenGL Vertex Array of the mesh, defines mesh vertex input.
		GLVertexArray* vxArray;

		// OpenGL Buffer for Sphere Positions.
		GLBuffer* vxBuffer;

		// OpenGL Index/Element Buffer.
		GLBuffer* idxBuffer;

		// The number of indices in the index buffer.
		int32_t numIndices;

		// The view matrix of the sphere.
		glm::mat4 view;

		// The projection matrix of the sphere.
		glm::mat4 proj;

	};

}


