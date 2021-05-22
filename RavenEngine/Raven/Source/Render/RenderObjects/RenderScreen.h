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




#include "glm/vec2.hpp"





namespace Raven
{
	class GLVertexArray;
	class GLBuffer;
	class GLShader;


	// RenderScreen:
	//    - Render a triangle covering the entire screen.
	//
	class RenderScreen
	{
		// Cosntruct.
		RenderScreen();

	public:
		// Destruct.
		~RenderScreen();

		// Create a rende screen.
		static RenderScreen* Create();

		// Draw.
		void Draw(GLShader* shader);

		// Set the render target actual size.
		void SetRTSize(const glm::vec2& size) { rtSize = size; }

	private:
		// The OpenGL Vertex Array
		GLVertexArray* vxArray;

		// OpenGL Vertex Buffer for Positions + UV.
		GLBuffer* vxBuffer;

		// The render target actual size, used to scale the uv to correctly sample targets.
		glm::vec2 rtSize;
	};

}


