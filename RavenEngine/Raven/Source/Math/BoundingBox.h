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

//////////////////////////////////////////////////////////////////////////////
// This file is part of the Raven Game Engine			                    //
//////////////////////////////////////////////////////////////////////////////

#pragma once


#include "Utilities/Serialization.h"
#include "Math/MathUtils.h"

#include "glm/glm.hpp"

#include <utility>


namespace Raven
{
	namespace MathUtils
	{

		// BoundingBox:
		//		- Axis Aligned Bounding Box.
		//
		class BoundingBox
		{
		public:
			// Default Invalid Bounding box.
			BoundingBox();

			// Construct bounding box using min/max.
			BoundingBox(const glm::vec3& inMin, const glm::vec3& inMax);

			// Construct bounding box using center & scale.
			BoundingBox(const glm::vec3& center, float scaleX, float scaleY, float scaleZ);

			// Copy Construct
			inline BoundingBox(const BoundingBox& other)
				: min(other.min)
				, max(other.max)
				, isValid(other.isValid)
			{

			}

			// add a vertex and update bounding box min and max.
			void Add(const glm::vec3& vert);

			// add another bounding box to be contained by this bounding box.
			void Add(const BoundingBox& other);

			// Test if a point is inside/outside the bounding box.
			EIntersection DoesPointIntersect(const glm::vec3 point) const;

			// Test if two bounding box intersect with each other.
			EIntersection DoesBoxIntersect(const BoundingBox& other) const;

			// Test if a ray intersect a bounding box and return the paramter if there is an intersection.
			// @parama outAlpha: is a scalar value that can be used to compute the point using the line quation start + dir * outAlpha.
			bool RayIntersection(const glm::vec3& org, const glm::vec3& dir, float& outAlpha) const;

			// Return the center of the bounding box.
			inline glm::vec3 GetCenter() const { return (min + max) * 0.5f; }

			// Return the boudning box extent.
			inline glm::vec3 GetExtent() const { return (max - min) * 0.5f; }

			// Return the radius of the sphere that contain the bounding box.
			inline void GetSphere(glm::vec3& outCenter, float& outRadius) const
			{
				outCenter = GetCenter();
				outRadius = glm::length(GetExtent());
			}

			// Return true if the bounding box values are valid.
			inline bool IsValid() { return isValid; }

			// Reset the bounding box to invalid state.
			inline void Reset() { isValid = false; }

			// Transform the bounding box and return it.
			BoundingBox Transform(const glm::mat4& mtx) const;

			// Return minimum.
			inline glm::vec3 GetMin() const { return min; }

			// Return maximum.
			inline glm::vec3 GetMax() const { return max; }


			// Serialization Save.
			template<typename Archive>
			void save(Archive& archive) const
			{
				archive(isValid);
				archive(min);
				archive(max);
			}

			// Serialization Load.
			template<typename Archive>
			void load(Archive& archive)
			{
				archive(isValid);
				archive(min);
				archive(max);
			}

		private:
			// If false then min and max values are invalid.
			bool isValid;

			// The minimum point of the bounding box.
			glm::vec3 min;

			// The maximum point of the bounding box.
			glm::vec3 max;

		};
	}
}