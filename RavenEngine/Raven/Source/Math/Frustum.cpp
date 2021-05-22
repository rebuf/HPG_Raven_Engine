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
#include "Frustum.h"




namespace Raven {

namespace MathUtils {



Frustum::Frustum()
{

}


Frustum::Frustum(const glm::mat4& mtx)
{
	ExtractPlanes(mtx);
}




void Frustum::ExtractPlanes(const glm::mat4& mtx)
{
	// Left clipping plane   
	planes[0].x = mtx[0][3] + mtx[0][0];
	planes[0].y = mtx[1][3] + mtx[1][0];
	planes[0].z = mtx[2][3] + mtx[2][0];
	planes[0].w = mtx[3][3] + mtx[3][0];

	// Right clipping plane   
	planes[1].x = mtx[0][3] - mtx[0][0];
	planes[1].y = mtx[1][3] - mtx[1][0];
	planes[1].z = mtx[2][3] - mtx[2][0];
	planes[1].w = mtx[3][3] - mtx[3][0];
	
	// Top clipping plane   
	planes[2].x = mtx[0][3] - mtx[0][1];
	planes[2].y = mtx[1][3] - mtx[1][1];
	planes[2].z = mtx[2][3] - mtx[2][1];
	planes[2].w = mtx[3][3] - mtx[3][1];
	
	// Bottom clipping plane   
	planes[3].x = mtx[0][3] + mtx[0][1];
	planes[3].y = mtx[1][3] + mtx[1][1];
	planes[3].z = mtx[2][3] + mtx[2][1];
	planes[3].w = mtx[3][3] + mtx[3][1];
	
	// Near clipping plane    
	planes[4].x = mtx[0][3] - mtx[0][2];
	planes[4].y = mtx[1][3] - mtx[1][2];
	planes[4].z = mtx[2][3] - mtx[2][2];
	planes[4].w = mtx[3][3] - mtx[3][2];
	
	// Far clipping plane    
	planes[5].x = mtx[0][3] + mtx[0][2];
	planes[5].y = mtx[1][3] + mtx[1][2];
	planes[5].z = mtx[2][3] + mtx[2][2];
	planes[5].w = mtx[3][3] + mtx[3][2];
	

	// Normalize Planes.
	Normalize(0);
	Normalize(1);
	Normalize(2);
	Normalize(3);
	Normalize(4);
	Normalize(5);
}


void Frustum::Normalize(uint32_t i)
{
	float len = planes[i].x * planes[i].x + planes[i].y * planes[i].y + planes[i].z * planes[i].z;

	// Zero?
	if (len < SMALL_NUM)
	{
		planes[i] = glm::vec4(0.0f);
		return;
	}

	len = sqrt(len);
	planes[i].x /= len;
	planes[i].y /= len;
	planes[i].z /= len;
	planes[i].w /= len;
}


bool Frustum::IsInFrustum2D(const glm::vec3& center, float radius)
{
	// Left, Right, Near and Far.
	static uint32_t planesIndices[4] = { 0, 1, 4, 5 };

	// Test Planes...
	for (uint32_t i = 0; i < 4; ++i)
	{
		if (TestPlane(planesIndices[i], center, radius))
			return false;
	}

	return true;
}


bool Frustum::IsInFrustum(const glm::vec3& center, float radius)
{
	// Test Planes...
	for (uint32_t i = 0; i < 6; ++i)
	{
		if (TestPlane(i, center, radius))
			return false;
	}

	return true;
}


bool Frustum::TestPlane(uint32_t idx, const glm::vec3& center, float radius)
{
	glm::vec3 n = glm::vec3(planes[idx].x, planes[idx].y, planes[idx].z);

	float dr = glm::dot(n, center) + planes[idx].w;

	if (-(dr + radius) > SMALL_NUM)
		return true;

	return false;
}





} // End of namespace MathUtils.


} // End of namespace Raven.
