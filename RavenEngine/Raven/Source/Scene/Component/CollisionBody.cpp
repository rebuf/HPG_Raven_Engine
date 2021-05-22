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

#include <rp3d/include/reactphysics3d/reactphysics3d.h> 

#include "Scene/Component/CollisionBody.h"

#include "Utilities/ToRp3d.h"

namespace Raven
{
	CollisionBody::CollisionBody(rp3d::PhysicsWorld* physWorld, Transform& transform) :
		world(physWorld),
		body(nullptr) // initialise the body pointer to nullptr
	{
		// convert our transform component to the transform used in react and put the collision body in the 
		// physics world
		body = physWorld->createCollisionBody(Rp3dConvert::ToRp3dTransform(transform));
	}

	CollisionBody::~CollisionBody()
	{
		// clear the collider shapes first (collider destructors require valid collision body)
		colliders.clear();
		// make sure the body is removed from the world on desruction
		RemoveBodyFromWorld();
	}

	// add a collider to the collision body
	void CollisionBody::AddCollider(Collider* collider)
	{
		if (ExistsInWorld())
		{
			// add it to the vector of colliders
			colliders.emplace_back(collider);
		}
	}

	void CollisionBody::RemoveCollider(uint32_t index)
	{
		// NB THIS DOES NOT CHECK IF THE INDEX IS OUT OF BOUNDS (do this through editor?)
		colliders.erase(colliders.begin() + index); // deletes the pointer
	}

	void CollisionBody::SetIsTrigger(uint32_t index, bool b)
	{
		colliders[index]->collider->setIsTrigger(b);
	}

	bool CollisionBody::IsTrigger(uint32_t index)
	{
		return colliders[index]->collider->getIsTrigger();
	}

	void CollisionBody::SetTransform(const Transform& t)
	{
		// set the underlying rp3d body's transform to specified
		body->setTransform(Rp3dConvert::ToRp3dTransform(t));
	}

	void CollisionBody::RemoveBodyFromWorld()
	{
		// check the body exists
		if (ExistsInWorld())
		{
			world->destroyCollisionBody(body); // takes care of destroying the shapes associated with the body
			body = nullptr; // set to nullptr for future reference
		}
	}

	bool CollisionBody::ExistsInWorld()
	{
		if (body)
			return true;
		return false;
	}
}