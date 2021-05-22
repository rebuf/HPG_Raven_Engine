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
#include "Math/BoundingBox.h"
#include "glm/common.hpp"
#include "glm/vec4.hpp"
#include "glm/matrix.hpp"




namespace Raven
{
	namespace MathUtils
	{

		// Frustum:
		//		- view frustum used to perfrom frustum test for culling.
		//
		class Frustum
		{
		public:
			// Default - Unitialized.
			Frustum();

			// Construct using view projection matrix.
			Frustum(const glm::mat4& mtx);

			// Copy Construct.
			Frustum(const Frustum& other)
			{
				memcpy(planes, other.planes, sizeof(glm::vec4) * 6);
			}

			// Copy Operator.
			Frustum& operator=(const Frustum& other)
			{
				memcpy(planes, other.planes, sizeof(glm::vec4) * 6);
				return *this;
			}

			// Test if a sphere inside or intersect the frustum, ignores the Y-Axis and perform the test in 2D.
			bool IsInFrustum2D(const glm::vec3& center, float radius);

			// Test if a sphere inside or intersect the frustum.
			bool IsInFrustum(const glm::vec3& center, float radius);

			// Extract Frustum Planes from view projection matrix.
			void ExtractPlanes(const glm::mat4& mtx);

		private:
			// Normalize the plane.
			void Normalize(uint32_t i);

			// Test if the sphere intersect or on the positive half space of the plane.
			bool TestPlane(uint32_t idx, const glm::vec3& center, float radius);

		private:
			// The frustum planes in ax+by+cz+d=0 form.
			glm::vec4 planes[6];
		};

	}

}


