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

#include "Math/BoundingBox.h"
#include "Math/MathUtils.h"


#include "glm/common.hpp"





namespace Raven {

namespace MathUtils {



BoundingBox::BoundingBox()
	: isValid(false)
{

}


BoundingBox::BoundingBox(const glm::vec3& inMin, const glm::vec3& inMax)
	: isValid(true)
	, min(inMin)
	, max(inMax)
{

}


BoundingBox::BoundingBox(const glm::vec3& center, float scaleX, float scaleY, float scaleZ)
{
	glm::vec3 scale(scaleX * 0.5f, scaleY * 0.5f, scaleZ * 0.5f);

	Add(center + scale);
	Add(center - scale);
}


void BoundingBox::Add(const glm::vec3& vert)
{
	if (isValid)
	{
		min = glm::min(min, vert);
		max = glm::max(max, vert);
	}
	else
	{
		min = max = vert;
		isValid = true;
	}

}


void BoundingBox::Add(const BoundingBox& other)
{
	Add(other.min);
	Add(other.max);
}


EIntersection BoundingBox::DoesPointIntersect(const glm::vec3 point) const
{
	// point is outside if either of its xyz is bigger or smaller than the corresponding max or min respectively
	if (point.x > min.x || point.x < max.x ||
		point.y > min.y || point.y < max.y ||
		point.z > min.z || point.z < max.z)
	{
		return EIntersection::Outside;
	}
	else
	{
		return EIntersection::Inside;
	}
}


EIntersection BoundingBox::DoesBoxIntersect(const BoundingBox& other) const
{
	// other box is outside if either of its max or min is smaller or bigger
	// than this box's corresponding min or max respectively
	if (other.max.x < min.x || max.x > other.min.x ||
		other.max.y < min.y || max.y > other.min.y ||
		other.max.z < min.z || max.z > other.min.z)
	{
		return EIntersection::Outside;
	}
	else 
	// other box is intersecting if either of its max or min is or smaller 
	// than this box's corresponding max or min respectively
	if (other.max.x > max.x || min.x < other.min.x ||
		other.max.y > max.y || min.y < other.min.y ||
		other.max.z > max.z || min.z < other.min.z)
	{
		return EIntersection::Intesects;
	}
	// remaining case must be Inside
	else
	{
		return EIntersection::Inside;
	}
}


bool BoundingBox::RayIntersection(const glm::vec3& org, const glm::vec3& dir, float& outAlpha) const
{
	// 
	glm::vec3 div;
	div.x = 1.0f / dir.x;
	div.y = 1.0f / dir.y;
	div.z = 1.0f / dir.z;

	//
	glm::vec3 mx = (min - org) * div;
	glm::vec3 mn = (max - org) * div;

	//
	glm::vec3 minab = glm::min(mx, mn);
	glm::vec3 maxab = glm::max(mx, mn);

	float alphaMin = glm::max(minab.x, glm::max(minab.y, minab.z));
	float alphaMax = glm::min(maxab.x, glm::min(maxab.y, maxab.z));

	// Intersection is Behind?
	if (alphaMax < 0)
	{
		outAlpha = alphaMax;
		return true;
	}

	// No Intersection?
	if (alphaMin > alphaMax)
	{
		return false;
	}

	outAlpha = alphaMin;
	return true;
}



BoundingBox BoundingBox::Transform(const glm::mat4& mtx) const
{
	glm::vec3 newMin = mtx * glm::vec4(min, 1.0f);
	glm::vec3 mewMax = mtx * glm::vec4(max, 1.0f);

	BoundingBox trBox;
	trBox.isValid = isValid;
	trBox.min = glm::min(newMin, mewMax);
	trBox.max = glm::max(newMin, mewMax);

	return trBox;
}





} // End of namespace MathUtils.


} // End of namespace Raven.
