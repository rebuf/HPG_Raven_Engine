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
#include <string>
#include <glm/glm.hpp>
#include <algorithm>

namespace Raven
{
	namespace MathUtils 
	{
		constexpr float EPS = 0.000001f;


		// enum for different intersection types
		enum class EIntersection : int32_t
		{
			Inside,
			Outside,
			Intesects
		};

		inline float Lerp(float from, float to, float t, bool clamp01 = true)
		{
			if (clamp01)
			{
				t = std::min<float>(std::max<float>(t, 0), 1);
			}
			return from + (to - from) * t;
		}


		// vec4 comparators
		inline bool operator >=(const glm::vec4& left, const glm::vec4& other)
		{
			return left.x >= other.x && left.y >= other.y && left.z >= other.z && left.w >= other.w;
		}

		inline bool operator <=(const glm::vec4& left, const glm::vec4& other)
		{
			return left.x <= other.x && left.y <= other.y && left.z <= other.z && left.w <= other.w;;
		}


		// vec3 comparators
		inline bool operator >=(const glm::vec3& left, const glm::vec3& other)
		{
			return left.x >= other.x && left.y >= other.y && left.z >= other.z;
		}

		inline bool operator <=(const glm::vec3& left, const glm::vec3& other)
		{
			return left.x <= other.x && left.y <= other.y && left.z <= other.z;
		}


		// vec3 comparators
		inline bool operator >=(const glm::vec2& left, const glm::vec2& other)
		{
			return left.x >= other.x && left.y >= other.y ;
		}

		inline bool operator <=(const glm::vec2& left, const glm::vec2& other)
		{
			return left.x <= other.x && left.y <= other.y ;
		}

		//only support for float value and vector
		template <class T>
		typename std::enable_if<
			std::is_floating_point<T>::value || 
			std::is_same<T,glm::vec2>::value || 
			std::is_same<T, glm::vec3>::value || 
			std::is_same<T, glm::vec4>::value,bool>::type Equals(const T& lhs, const T& rhs) {
			T eps(0.000001f); 
			return lhs + eps >= rhs && lhs - eps <= rhs; 
		}
	};
};
