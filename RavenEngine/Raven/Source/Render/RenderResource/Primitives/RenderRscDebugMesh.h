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





#include "RenderRscPrimitive.h"

#include "glm/vec3.hpp"
#include <vector>





namespace Raven
{
	class GLVertexArray;
	class GLBuffer;



	//
	//
	class RenderRscDebugMesh : public RenderRscPrimitive
	{
	public:
		// Construct.
		RenderRscDebugMesh();

		// Destruct.
		~RenderRscDebugMesh();

		// Create a debug box mesh.
		void CreateBox();

		// Return mesh vertex array.
		inline const GLVertexArray* GetArray() const { return vertexArray; }

		//
		inline uint32_t GetNumIndices() const { return numIndices; }

	private:
		// Load a mesh data into the render resoruce.
		void Load(const std::vector<glm::vec3>& verts, const std::vector<uint32_t>& indices);

	private:
		// Mesh Vertex Array.
		GLVertexArray* vertexArray;

		// Mesh Vertex Buffer.
		GLBuffer* vertexBuffer;

		// Mesh Index Buffer.
		GLBuffer* indexBuffer;

		//
		uint32_t numIndices;
	};

}


