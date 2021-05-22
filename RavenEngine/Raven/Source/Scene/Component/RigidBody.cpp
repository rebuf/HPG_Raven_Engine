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

#include "Engine.h"

#include "Scene/Component/RigidBody.h"

#include "Utilities/ToRp3d.h"

#include <glm/gtc/type_ptr.hpp> // make mat from array of values
#include <glm/gtc/quaternion.hpp> 
#include <glm/gtx/quaternion.hpp> 
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/rotate_vector.hpp>

namespace Raven
{
	// default constructor to register as a valid entity
	RigidBody::RigidBody() : 
		world(Engine::Get().GetModule<PhysicsModule>()->GetCurrentWorld()),
		initTransform(Transform::Identity),
		mass(1.0f),
		linearDamping(0.0f),
		angularDamping(0.0f),
		canTopple(true),
		type(RigidBodyType::Static),
		bodyIdx(0)
	{}

	RigidBody::~RigidBody()
	{}

	void RigidBody::InitRigidBody(int index)
	{
		// if body already exists, delete it and replace with a new default body
		auto body = Engine::Get().GetModule<PhysicsModule>()->GetRigidBody(index);

		auto initT = Rp3dConvert::ToRp3dTransform(initTransform);

		previousState = initT;
		body->setMass(mass);
		body->setIsAllowedToSleep(canSleep);
		body->enableGravity(gravityEnabled);
		body->setLinearDamping(linearDamping);
		body->setAngularDamping(angularDamping);
		// set the type after the parameters, otherwise rp3d will change types without telling you >:-((
		body->setType(static_cast<rp3d::BodyType>(type));

		if (!canTopple) body->setLocalInertiaTensor(rp3d::Vector3(0, 0, 0));
	}


	void RigidBody::AddCollider(Collider* collider)
	{
		// update the body's vector containing colliders 
		colliders.emplace_back(collider);
	}

	void RigidBody::RemoveCollider(uint32_t index)
	{
		// NB THIS DOES NOT CHECK IF THE INDEX IS OUT OF BOUNDS
		colliders.erase(colliders.begin() + index); // deletes the pointer
	}

	std::shared_ptr<Collider> RigidBody::GetCollider(uint32_t index)
	{
		return colliders[index];
	}

	std::vector<std::shared_ptr<Collider>>* RigidBody::GetAllColliders()
	{
		return &colliders;
	}

	void RigidBody::SetIsTrigger(uint32_t index, bool b)
	{
		colliders[index]->collider->setIsTrigger(b);
	}

	bool RigidBody::IsTrigger(uint32_t index)
	{
		return colliders[index]->collider->getIsTrigger();
	}

	void RigidBody::SetPreviousState(const rp3d::Transform& t)
	{
		previousState = t;
	}

	rp3d::Transform RigidBody::GetPreviousState()
	{
		return previousState;
	}

	rp3d::Transform RigidBody::GetCurrentState()
	{
		return Engine::Get().GetModule<PhysicsModule>()->GetRigidBody(bodyIdx)->getTransform();
	}

	void RigidBody::SetTransform(const Transform& t)
	{
		// set the underlying rp3d body's transform to specified
		Engine::Get().GetModule<PhysicsModule>()->GetRigidBody(bodyIdx)->setTransform(Rp3dConvert::ToRp3dTransform(t));
	}

	Transform RigidBody::GetTransform()
	{
		float m[16];
		Engine::Get().GetModule<PhysicsModule>()->GetRigidBody(bodyIdx)->getTransform().getOpenGLMatrix(m); // get transform from physics library
		return Transform(glm::make_mat4(m));
	}

	void RigidBody::SetInitTransform(const Transform& t)
	{
		initTransform = t;
	}

	Transform RigidBody::GetInitTransform()
	{
		return initTransform;
	}

	void RigidBody::InitTransform()
	{
		Engine::Get().GetModule<PhysicsModule>()->GetRigidBody(bodyIdx)->setTransform(Rp3dConvert::ToRp3dTransform(initTransform));
	}

	glm::quat RigidBody::GetOrientation()
	{
		auto& o = Engine::Get().GetModule<PhysicsModule>()->GetRigidBody(bodyIdx)->getTransform().getOrientation();
		
		return Rp3dConvert::ToGLMQuat(o);
	}


	glm::vec3 RigidBody::GetForwardVector()
	{
		return GetOrientation() * FORWARD;
	}

	glm::vec3 RigidBody::GetRightVector()
	{
		return GetOrientation() * RIGHT;
	}

	glm::vec3 RigidBody::GetUpVector()
	{
		return GetOrientation() * UP;
	}

	uint32_t RigidBody::GetNumColliders()
	{
		return colliders.size();
	}

	//
	// Body settings
	//

	// set if the body is affected by gravity or not
	void RigidBody::EnableGravity(bool b)
	{
		gravityEnabled = b;
	}
	bool RigidBody::GravityEnabled()
	{
		return gravityEnabled;
	}

	// set if the body is able to toppple over
	// Disable for player entity
	void RigidBody::EnableTopple(bool b)
	{
		canTopple = b;
	}
	bool RigidBody::ToppleEnabled()
	{
		return canTopple;
	}

	void RigidBody::SetMass(float m)
	{
		mass = m;
	}
	float RigidBody::GetMass()
	{
		return mass;
	}

	void RigidBody::SetLinearDamping(float d)
	{
		// d >= 0 
		linearDamping = d;
	}
	float RigidBody::GetLinearDamping()
	{
		return linearDamping;
	}

	void RigidBody::SetAngularDamping(float d)
	{
		// d >= 0 
		angularDamping = d;
	}
	float RigidBody::GetAngularDamping()
	{
		return angularDamping;
	}

	void RigidBody::SetIsAllowedToSleep(bool b)
	{
		canSleep = b;
	}
	bool RigidBody::CanSleep()
	{ 
		return canSleep; 
	}

	void RigidBody::SetBodyType(RigidBodyType t)
	{
		type = t;
	}
	RigidBodyType RigidBody::GetBodyType()
	{
		return type;
	}

	void RigidBody::SetBodyIndex(int idx)
	{
		bodyIdx = idx;
	}
	int RigidBody::GetBodyIndex()
	{
		return bodyIdx;
	}


	//
	// Apply forces to the rigid body (only during one call of update world in simulation)
	//

	// Applies force to the body's centre of mass
	void RigidBody::ApplyForce(const glm::vec3& f)
	{
		//auto b = Engine::Get().GetModule<PhysicsModule>()->GetRigidBody(bodyIdx);
		Engine::Get().GetModule<PhysicsModule>()->GetRigidBody(bodyIdx)->applyForceToCenterOfMass(Rp3dConvert::ToRp3dVector3(f));
	}

	void RigidBody::ApplyForceAtLocalPos(const glm::vec3& f, const glm::vec3& pos)
	{
		Engine::Get().GetModule<PhysicsModule>()->GetRigidBody(bodyIdx)->applyForceAtLocalPosition(Rp3dConvert::ToRp3dVector3(f), Rp3dConvert::ToRp3dVector3(pos));
	}

	void RigidBody::ApplyForceAtWorldPos(const glm::vec3& f, const glm::vec3& pos)
	{
		Engine::Get().GetModule<PhysicsModule>()->GetRigidBody(bodyIdx)->applyForceAtWorldPosition(Rp3dConvert::ToRp3dVector3(f), Rp3dConvert::ToRp3dVector3(pos));
	}

	void RigidBody::ApplyTorque(const glm::vec3& t)
	{
		Engine::Get().GetModule<PhysicsModule>()->GetRigidBody(bodyIdx)->applyTorque(Rp3dConvert::ToRp3dVector3(t));
	}

	void RigidBody::RotateBody(const glm::vec3 axis, float deg)
	{
		float rad = glm::radians(deg);
		// Create the quaternion to rotate the body the desired amount
		rp3d::Quaternion q = rp3d::Quaternion::fromEulerAngles(Rp3dConvert::ToRp3dVector3(axis * rad));

		// Get the tranform of the rigid body and rotate it
		rp3d::Transform t = Engine::Get().GetModule<PhysicsModule>()->GetRigidBody(bodyIdx)->getTransform();
		t.setOrientation(q * t.getOrientation());
		Engine::Get().GetModule<PhysicsModule>()->GetRigidBody(bodyIdx)->setTransform(t);
	}
}