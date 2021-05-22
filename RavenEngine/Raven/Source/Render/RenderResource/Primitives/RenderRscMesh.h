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
#include "glm/vec2.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include <vector>





namespace Raven
{
	class GLVertexArray;
	class GLBuffer;

	// RenderRscMesh:
	//
	class RenderRscMesh : public RenderRscPrimitive
	{
		// Friend...
		friend class RenderRscMeshInstance;

	public:
		// Construct.
		RenderRscMesh();

		// Destruct.
		~RenderRscMesh();

		// Load Mesh Data
		virtual void Load(const std::vector<glm::vec3>& positions,
			const std::vector<glm::vec3>& normals, const std::vector<glm::vec3>& tangents,
			const std::vector<glm::vec2>& texCoord, const std::vector<unsigned int>& indices);

		// Return the vertex array that defines this mesh Vertex Input.
		inline GLVertexArray* GetArray() { return vxarray; }

		// Return the number of indices in the mesh.
		inline int32_t GetNumIndices() const { return numIndices; }


	protected:
		// The OpenGL Vertex Array of the mesh, defines mesh vertex input.
		GLVertexArray* vxarray;

		// OpenGL Buffer for Mesh Positions.
		GLBuffer* positionBuffer;

		// OpenGL Buffer for Mesh Normals.
		GLBuffer* normalBuffer;

		// OpenGL Buffer for Mesh Normals.
		GLBuffer* tangentBuffer;

		// OpenGL Buffer for Mesh Texture Coordinate.
		GLBuffer* texCoordBuffer;

		// OpenGL Buffer for Mesh Indices.
		GLBuffer* indexBuffer;

		// Number of indices in the index buffer.
		int32_t numIndices;
	};



	// RenderRscSkinnedMesh:
	//
	class RenderRscSkinnedMesh : public RenderRscPrimitive
	{
	public:
		// Construct.
		RenderRscSkinnedMesh();

		// Destruct.
		~RenderRscSkinnedMesh();

		// Load Mesh Data
		void Load(const std::vector<glm::vec3>& positions,
			const std::vector<glm::vec3>& normals, const std::vector<glm::vec3>& tangents,
			const std::vector<glm::vec2>& texCoord, const std::vector<unsigned int>& indices,
			const std::vector<glm::vec4>& weight, const std::vector<glm::ivec4>& blendIndices);

		// Return the vertex array that defines this mesh Vertex Input.
		inline GLVertexArray* GetArray() { return vxarray; }

		// Return the number of indices in the mesh.
		inline int32_t GetNumIndices() const { return numIndices; }

	private:
		// The OpenGL Vertex Array of the mesh, defines mesh vertex input.
		GLVertexArray* vxarray;

		// OpenGL Buffer for Mesh Positions.
		GLBuffer* positionBuffer;

		// OpenGL Buffer for Mesh Normals.
		GLBuffer* normalBuffer;

		// OpenGL Buffer for Mesh Normals.
		GLBuffer* tangentBuffer;

		// OpenGL Buffer for Mesh Texture Coordinate.
		GLBuffer* texCoordBuffer;

		// OpenGL Buffer for Mesh Indices.
		GLBuffer* indexBuffer;

		// Number of indices in the index buffer.
		int32_t numIndices;

		// OpenGL Buffer for skin weight.
		GLBuffer* weightBuffer;

		// OpenGL Buffer for bone indices.
		GLBuffer* boneIndicesBuffer;
	};


	// RenderRscMeshInstance:
	//
	class RenderRscMeshInstance : public RenderRscPrimitive
	{
	public:
		// Construct.
		RenderRscMeshInstance();

		// Destruct.
		~RenderRscMeshInstance();

		// Load MeshInstance from src mesh.
		void Load(GLBuffer* instanceBuffer, int32_t iniSize, RenderRscMesh* rscMesh);

		// Return the vertex array that defines this mesh Vertex Input.
		inline GLVertexArray* GetArray() { return vxarray; }

		// Return instance buffer.
		inline GLBuffer* GetInstanceTransform() { return instanceTransform; }

		// Update the instances with a list of transforms.
		void UpdateTransforms(const std::vector<glm::mat4>& transforms);

		// Return the number of indices in the mesh.
		inline int32_t GetNumIndices() const { return mesh->GetNumIndices(); }

	private:
		// The mesh we are instancing.
		RenderRscMesh* mesh;

		// The OpenGL Vertex Array of the mesh, defines mesh vertex input.
		GLVertexArray* vxarray;

		// OpenGL Transform Buffer, for instance transformation.
		GLBuffer* instanceTransform;

		// Onwer of the instance buffer.
		bool instanceBufferOwner;
	};

}


