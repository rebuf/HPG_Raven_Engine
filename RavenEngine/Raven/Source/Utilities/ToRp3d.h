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

#include <reactphysics3d/reactphysics3d.h>
#include <reactphysics3d/mathematics/Transform.h>
#include <reactphysics3d/mathematics/Vector3.h>
#include <reactphysics3d/mathematics/Quaternion.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Scene/Component/Transform.h"

//
// Convert our maths data types into the ones used by the react 3d library
//

namespace Raven
{
	namespace Rp3dConvert
	{
		// from engine to physics
		rp3d::Transform ToRp3dTransform(const Transform& t);
		rp3d::Vector3 ToRp3dVector3(const glm::vec3& v);
		rp3d::Quaternion ToRp3dQuaternion(const glm::quat& q);

		// from physics to engine
		Transform ToTransform(const rp3d::Transform& other, const glm::vec3& scale);
		glm::quat ToGLMQuat(const rp3d::Quaternion other);
	}
}