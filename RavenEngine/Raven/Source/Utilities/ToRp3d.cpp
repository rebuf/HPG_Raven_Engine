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

#include "Utilities/ToRp3d.h"

#include <glm/gtc/type_ptr.hpp>

namespace Raven
{
	namespace Rp3dConvert
	{
		rp3d::Transform ToRp3dTransform(const Transform& t)
		{
			return rp3d::Transform(ToRp3dVector3(t.GetWorldPosition()), ToRp3dQuaternion(t.GetWorldRotation()));
		}

		rp3d::Vector3 ToRp3dVector3(const glm::vec3& v)
		{
			return rp3d::Vector3(v.x, v.y, v.z);
		}

		rp3d::Quaternion ToRp3dQuaternion(const glm::quat& q)
		{
			// make sure to normalize the quaternion
			glm::quat quat = glm::normalize(q);
			// !rp3d has scalar after vector! 
			return rp3d::Quaternion(quat.x, quat.y, quat.z, quat.w);
		}

		// As RP3D does not encode model scale in it's transform we have to add the scaling of the object before updating the transform
		Transform ToTransform(const rp3d::Transform& other, const glm::vec3& scale)
		{
			// construct from a glm mat4
			glm::mat4 t;
			// column major
			other.getOpenGLMatrix(glm::value_ptr(t));

			glm::mat4 scaleMat(scale.x, 0, 0, 0, 0, scale.y, 0, 0, 0, 0, scale.z, 0, 0, 0, 0, 1);
			// a new Raven::Transform with the rp3d transform data
			return Transform(t*scaleMat);
		}

		glm::quat ToGLMQuat(const rp3d::Quaternion other)
		{
			glm::quat out;
			out.x = other.x;
			out.y = other.y;
			out.z = other.z;
			out.w = other.w;
			return out;
		}

	}
}