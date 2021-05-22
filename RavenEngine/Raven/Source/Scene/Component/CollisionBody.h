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

#include <cereal/types/vector.hpp>

#include "Physics/Collider.h"

#include "Scene/Component/Transform.h"

//
// Wrapper for the react collision body, also a component in the engine's ECS
//

namespace Raven
{
	class PhysicsModule;
	class CollisionBody {
		friend PhysicsModule;
	public:
		// a collision body needs a transform to exist
		CollisionBody(rp3d::PhysicsWorld* physWorld, Transform& transform);
		~CollisionBody();

		// add a collider to the collision body
		void AddCollider(Collider* collider);
		// remove the specified collider from the collision body
		void RemoveCollider(uint32_t index);

		// set the collider at specified index to be a trigger (does not collide, only reports overlap)
		void SetIsTrigger(uint32_t index, bool b);
		bool IsTrigger(uint32_t index);

		void SetTransform(const Transform& t);

		// removes the body and its colliders from the physics world
		void RemoveBodyFromWorld();
		// check that the collision body exists in the physics world
		bool ExistsInWorld();


		// load and save the colliders attached to the body
		template<class Archive>
		void save(Archive& archive) const;

		template<class Archive>
		void load(Archive& archive);

	private:
		// world the body belongs to
		rp3d::PhysicsWorld* world;

		// keep the body as a raw pointer, managed in our deleter to destroy
		// it in the physics world
		rp3d::CollisionBody* body;

		// vector of the colliders attached to the collision body
		std::vector<std::shared_ptr<Collider>> colliders;
	};

	template<class Archive>
	void CollisionBody::save(Archive& archive) const
	{
		archive(cereal::make_nvp("Colliders", colliders));
	}

	template<class Archive>
	void CollisionBody::load(Archive& archive)
	{
		// nb, this only loads shape data and creates instances of the collider objects.
		// we need to initialise them using the 
		archive(cereal::make_nvp("Colliders", colliders));
	}

}