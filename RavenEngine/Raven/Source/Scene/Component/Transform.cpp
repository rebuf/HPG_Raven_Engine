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

#include "Transform.h"
#include "Scene/Entity/Entity.h"
#include "Scene/SceneManager.h"
#include "Scene/Scene.h"


#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>



namespace Raven
{


const Transform Transform::Identity( glm::mat4(1.0f) );




Transform::Transform()
	: position(0.0f)
	, scale(1.0f)
	, rotation(1.0f, 0.0f, 0.0f, 0.0f)
	, isWorldMatrixDiry(true)
	, isLocalMatrixCacheDiry(true)
{

}


Transform::Transform(const glm::mat4& matrix)
	: worldMatrix(1.0f)
	, isWorldMatrixDiry(true)
	, isLocalMatrixCacheDiry(true)
{
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(matrix, scale, rotation, position, skew, perspective);
	UpdateLocalMatrix();
}


Transform::Transform(const glm::vec3& inPosition)
	: scale(1.0f)
	, rotation(1.0f, 0.0f, 0.0f, 0.0f)
	, isWorldMatrixDiry(true)
	, isLocalMatrixCacheDiry(true)

{
	position = inPosition;
	UpdateLocalMatrix();
}


Transform::~Transform()
{

}


void Transform::UpdateLocalMatrix() const
{
	localMatrix_cache = glm::translate(glm::mat4(1), position);
	localMatrix_cache *= glm::toMat4(rotation);
	localMatrix_cache = glm::scale(localMatrix_cache, scale);
	isLocalMatrixCacheDiry = false;
}


void Transform::UpdateWorldMatrix() const
{
	Entity ent = GetEntity();

	// Is Entity Valid?
	if (ent.Valid())
	{
		Entity parentEnt = ent.GetParent();

		if (parentEnt.Valid())
		{
			Transform* parentTransform = parentEnt.TryGetComponent<Transform>();

			if (parentTransform)
			{
				worldMatrix = parentTransform->GetWorldMatrix() * GetLocalMatrix();
			}
		}
		else
		{
			worldMatrix = GetLocalMatrix();
		}

		isWorldMatrixDiry = false;
		return;
	}
	else
	{
		worldMatrix = GetLocalMatrix();
		isWorldMatrixDiry = true;
	}
}


void Transform::UpdateChildrenWorld() const
{
	Entity ent = GetEntity();

	// Is Entity Valid?
	if (!ent.Valid())
		return;

	std::vector<Entity> children = ent.GetChildren();

	for (auto& child : children)
	{
		Transform* childTransform = child.TryGetComponent<Transform>();

		// Only Iterate on transform children.
		if (!childTransform)
			continue;

		childTransform->worldMatrix = GetWorldMatrix() * childTransform->GetLocalMatrix();
		childTransform->UpdateChildrenWorld();
	}
}


bool Transform::GetParentWorldMatrix(glm::mat4& outMtx) const
{
	Entity ent = GetEntity();

	// Is Entity Valid?
	if (ent.Valid())
	{
		Entity parentEnt = ent.GetParent();

		if (parentEnt.Valid())
		{
			Transform* parentTransform = parentEnt.TryGetComponent<Transform>();

			if (parentTransform)
			{
				outMtx = parentTransform->GetWorldMatrix();
				return true;
			}
		}
	}

	return false;
}


void Transform::ResetTransform()
{
	position = glm::vec3(0.0f);
	scale = glm::vec3(1.0f);
	rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	isWorldMatrixDiry = true;
	isLocalMatrixCacheDiry = true;
}


glm::vec3 Transform::GetForwardDirection() const
{
	return GetWorldRotation() * FORWARD;
}


glm::vec3 Transform::GetRightDirection() const
{
	return GetWorldRotation() * RIGHT;
}


glm::vec3 Transform::GetUpDirection() const
{
	return GetWorldRotation() * UP;
}


void Transform::UpdateDirty()
{
	// Not Dirty?
	if (!isWorldMatrixDiry)
		return;

	UpdateWorldMatrix();
	UpdateChildrenWorld();
}


void Transform::Dirty()
{
	isWorldMatrixDiry = true;
	isLocalMatrixCacheDiry = true;
}


void Transform::SetPosition(const glm::vec3& v, bool update)
{
	position = v;

	Dirty();

	if (update)
	{
		UpdateDirty();
	}
}


void Transform::SetScale(const glm::vec3& v, bool update)
{
	scale = v;

	Dirty();

	if (update)
	{
		UpdateDirty();
	}
}


void Transform::SetRotation(const glm::quat& v, bool update)
{
	rotation = v;

	Dirty();

	if (update)
	{
		UpdateDirty();
	}
}


void Transform::SetRotationEuler(float pitch, float yaw, float roll)
{
	// Make quat that does this order v' = Yaw * Pitch * Roll * v;
	rotation = 
		  glm::angleAxis(yaw, UP)
		* glm::angleAxis(pitch, RIGHT)
		* glm::angleAxis(roll, FORWARD);

	Dirty();
	UpdateDirty();
}


void Transform::SetWorldPosition(const glm::vec3& v)
{
	glm::mat4 mtx;
	if ( GetParentWorldMatrix(mtx) )
	{
		mtx = glm::inverse(mtx);
		position = mtx * glm::vec4(v, 1.0f);
	}
	else
	{
		position = v;
	}



	Dirty();
	UpdateDirty();
}


void Transform::SetWorldScale(const glm::vec3& v)
{
	RAVEN_ASSERT(0, "TODO Implement SetWorldScale.");
}


void Transform::SetWorldRotation(const glm::quat& v)
{
	glm::mat4 mtx;
	if (GetParentWorldMatrix(mtx))
	{
		mtx = glm::inverse(mtx);
		mtx = mtx * glm::toMat4(v);

		rotation = glm::toQuat(mtx);
		rotation = glm::normalize(rotation);
	}
	else
	{
		rotation = v;
	}


	Dirty();
	UpdateDirty();
}


void Transform::SetWorldRotation(float pitch, float yaw, float roll)
{
	// Make quat that does this order v' = Yaw * Pitch * Roll * v;
	glm::quat q =
		  glm::angleAxis(yaw, UP)
		* glm::angleAxis(pitch, RIGHT)
		* glm::angleAxis(roll, FORWARD);

	SetWorldRotation(q);
}


void Transform::SetMatrixTransform(const glm::mat4& mtx)
{
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(mtx, scale, rotation, position, skew, perspective);

	Dirty();
	UpdateDirty();
}


void Transform::SetWorldMatrixTransform(const glm::mat4& mtx)
{
	glm::mat4 invWorldMtx;
	if (GetParentWorldMatrix(invWorldMtx))
	{
		invWorldMtx = glm::inverse(invWorldMtx);
		invWorldMtx = invWorldMtx * mtx;
		SetMatrixTransform(invWorldMtx);
	}
	else
	{
		SetMatrixTransform(mtx);
	}
}


glm::vec3 Transform::ExtractScale(const glm::mat4& mtx)
{
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::vec3 mtxScale;
	glm::quat mtxRot;
	glm::vec3 mtxPosition;
	glm::decompose(mtx, mtxScale, mtxRot, mtxPosition, skew, perspective);

	return mtxScale;
}


void Transform::SetTransform(Transform& other)
{
	localMatrix_cache = other.GetLocalMatrix();
	worldMatrix = other.GetWorldMatrix();
	position = other.GetPosition();
	rotation = other.GetRotation();

	isLocalMatrixCacheDiry = false;
	isWorldMatrixDiry = false;
	UpdateChildrenWorld();
}


glm::vec3 Transform::GetRotationEuler() const
{
	glm::vec3 euler;

	// Rool Z-Axis
	float sinr_cosp = 2     * (rotation.w * rotation.z + rotation.x * rotation.y);
	float cosr_cosp = 1 - 2 * (rotation.z * rotation.z + rotation.x * rotation.x);
	euler.z = std::atan2(sinr_cosp, cosr_cosp);

	// Pitch X-Axis 
	float sinp = 2 * (rotation.w * rotation.x - rotation.y * rotation.z);
	euler.x = std::asin(sinp);

	// Yaw Y-Axis
	float siny_cosp = 2     * (rotation.w * rotation.y + rotation.z * rotation.x);
	float cosy_cosp = 1 - 2 * (rotation.x * rotation.x + rotation.y * rotation.y);
	euler.y = std::atan2(siny_cosp, cosy_cosp);
	euler.y = euler.y < 0.0f ? glm::two_pi<float>() + euler.y : euler.y;

	return euler;
}


} // End of namespace Raven

