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

#include "Physics/PhysicsModule.h"
#include "Physics/Collider.h"

#include "Scene/Component/Transform.h"
#include "Scene/Component/Component.h"

#include "Scene/System/PhysicsSystem.h"

#include <glm/gtc/quaternion.hpp> 

//
// Wrapper component class for rp3d rigid bodies
//

namespace Raven
{
	// types of rigid body:
	// Static -> does not move and only collides with Dynamic bodies (eg: a floor)
	// Kinematic -> moves and only collides with Dynamic bodies (eg: a moving platform)
	// Dynamic -> moves and collides with all body types (eg: a player character)
	enum class RigidBodyType
	{
		Static,
		Kinematic,
		Dynamic
	};

	class PhysicsModule;

	class RigidBody : public Component
	{
		friend PhysicsModule;
		friend PhysicsSystem;
	public:
		// a collision body needs a transform to exist
		RigidBody();
		//RigidBody(Transform& transform = Transform::Identity(), RigidBodyType initType = RigidBodyType::Static);
		~RigidBody();

		void InitRigidBody(int index);

		//rp3d::RigidBody* GetBody() { return Engine::Get().GetModule<PhysicsModule>()->GetRigidBody(bodyIdx); }

		// add a collider to the collision body
		void AddCollider(Collider* collider);
		// remove the specified collider from the collision body
		void RemoveCollider(uint32_t index);

		// get the specified collider from the collision body colliders
		std::shared_ptr<Collider> GetCollider(uint32_t index);
		std::vector<std::shared_ptr<Collider>>* GetAllColliders();

		// set the collider at specified index to be a trigger (does not collide, only reports overlap)
		void SetIsTrigger(uint32_t index, bool b);
		bool IsTrigger(uint32_t index);

		void SetPreviousState(const rp3d::Transform& t);
		rp3d::Transform GetPreviousState();
		rp3d::Transform GetCurrentState();

		void SetInitTransform(const Transform& t);
		Transform GetInitTransform();
		void InitTransform(); // initialises the rigid body's transform
		void SetTransform(const Transform& t);
		Transform GetTransform();

		// Get Information About the body's transform
		glm::quat GetOrientation();
		glm::vec3 GetForwardVector();
		glm::vec3 GetRightVector();
		glm::vec3 GetUpVector();

		uint32_t GetNumColliders();

		// set if the body is affected by gravity or not
		void EnableGravity(bool b);
		bool GravityEnabled();

		void EnableTopple(bool t);
		bool ToppleEnabled();

		void SetMass(float m);
		float GetMass();

		void SetLinearDamping(float d);
		void SetAngularDamping(float d);
		void SetIsAllowedToSleep(bool b);
		void SetBodyType(RigidBodyType t);
		void SetBodyIndex(int idx);
		int GetBodyIndex();


		float GetLinearDamping();
		float GetAngularDamping();
		bool CanSleep();
		RigidBodyType GetBodyType();

		// Applies forces to the body
		void ApplyForce(const glm::vec3& f);
		void ApplyForceAtLocalPos(const glm::vec3& f, const glm::vec3& pos);
		void ApplyForceAtWorldPos(const glm::vec3& f, const glm::vec3& pos);
		void ApplyTorque(const glm::vec3& t);

		void RotateBody(const glm::vec3 axis, float deg);

		// load and save the colliders attached to the body
		template<typename Archive>
		void save(Archive& archive) const
		{
			archive(cereal::make_nvp("Type", static_cast<int>(type)),
					cereal::make_nvp("Can sleep", canSleep),
					cereal::make_nvp("Gravity enabled", gravityEnabled),
					cereal::make_nvp("Can Topple", canTopple),
					cereal::make_nvp("Mass", mass),
					cereal::make_nvp("Body Index", bodyIdx),
					cereal::make_nvp("Linear Damping", linearDamping),
					cereal::make_nvp("Angular Damping", angularDamping),
					cereal::make_nvp("Colliders", colliders));
		}

		template<typename Archive>
		void load(Archive& archive)
		{
			// nb, this only loads body and shape data. They still need to be initialised (done on scene init)
			archive(cereal::make_nvp("Type", static_cast<RigidBodyType>(type)), 
					cereal::make_nvp("Can sleep", canSleep),
					cereal::make_nvp("Gravity enabled", gravityEnabled),
					cereal::make_nvp("Can Topple", canTopple),
					cereal::make_nvp("Mass", mass),
					cereal::make_nvp("Body Index", bodyIdx),
					cereal::make_nvp("Linear Damping", linearDamping),
					cereal::make_nvp("Angular Damping", angularDamping),
					cereal::make_nvp("Colliders", colliders));
		}

	private:
		// world the body belongs to
		rp3d::PhysicsWorld* world;

		// keep the body as a raw pointer, managed in our deleter to destroy
		// it in the physics world

		float mass;
		float linearDamping;
		float angularDamping;

		bool canSleep;
		bool gravityEnabled;
		bool canTopple;

		RigidBodyType type;

		Transform initTransform;

		int bodyIdx;

		// a copy of the previous state, used for slerping 
		rp3d::Transform previousState;

		// vector of the colliders attached to the collision body
		std::vector<std::shared_ptr<Collider>> colliders;
	};


}