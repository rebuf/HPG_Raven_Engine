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

#include <glm/glm.hpp>
#include <string>
#include <array>



namespace Raven
{
	class Mesh;


	// BasicShapes Types.
	namespace EBasicShape
	{
		enum Type
		{
			Plane = 0,
			Quad = 1,
			Cube = 2,
			Pyramid = 3,
			Sphere = 4,
			Capsule = 5,
			Cylinder = 6,
			LENGTH
		};

		// Basic Shape Names.
		const std::array<std::string, LENGTH> NAMES = 
		{
			"Plane",
			"Quad",
			"Cube",
			"Pyramid",
			"Sphere",
			"Capsule",
			"Cylinder",
		};

		// Convert EBasicShape to its name.
		std::string GetBasicShapeName(EBasicShape::Type type);

		// Convert basic shape name to its type.
		EBasicShape::Type GetPrimativeName(const std::string& name);
	};



	// MeshFactory:
	//    - create & manage basic shapes resrouces.
	//
	class MeshFactory
	{
	public:
		// Return the basic shape resrouce, if it doesn't exist create a new one.
		static Ptr<Mesh> GetBasicShape(EBasicShape::Type type);

	private:
		// Return the path of a basic shape resrouce.
		static std::string GetBasicShapeResroucePath(EBasicShape::Type type);


		// -- -- - - -- -- - - -- -- - - -- -- - - 
		//          Create Basic Shapes.
		// -- -- - - -- -- - - -- -- - - -- -- - -

		static Mesh* CreateQuad();
		static Mesh* CreateQuad(float x, float y, float width, float height);
		static Mesh* CreateQuad(const glm::vec2& position, const glm::vec2& size);
		static Mesh* CreateCube();
		static Mesh* CreatePyramid();
		static Mesh* CreateSphere(uint32_t xSegments = 64, uint32_t ySegments = 64);
		static Mesh* CreateCapsule(float radius = 1.0f, float midHeight = 1.0f, int32_t radialSegments = 64, int32_t rings = 8);
		static Mesh* CreatePlane(float width, float height, const glm::vec3& normal);
		static Mesh* CreateCylinder(float bottomRadius = 1.0f, float topRadius = 1.0f, float height = 1.0f, int32_t radialSegments = 64, int32_t rings = 8);
	
	};


	// Compute Tangents from vertices, normals, uvs.
	void ComputeTangents(glm::vec3* out, uint32_t indices_count, uint32_t* indices,
		const glm::vec3* vertices, const glm::vec3* normals, const glm::vec2* uvs);


}

