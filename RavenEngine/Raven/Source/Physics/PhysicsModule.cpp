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

#include "Physics/PhysicsModule.h"
#include "Physics/Collider.h"

#include "Scene/Component/CollisionBody.h"
#include "Scene/Component/RigidBody.h"

namespace Raven
{
	PhysicsModule::PhysicsModule() :
		accumulator(0.0f),
		lerpFactor(0.0f),
		world(nullptr)
	{

	}

	void PhysicsModule::Initialize()
	{
		// instantiate the physics world
		world = physicsCommon.createPhysicsWorld();
	}

	void PhysicsModule::Destroy()
	{
		// physics common is destroyed here, takes care of destroying everything in physics engine
	}

	void PhysicsModule::Step(float deltaTime)
	{
		// Add the time difference in the accumulator 
		accumulator += deltaTime;

		// While there is enough accumulated time to take one or several physics steps 
		while (accumulator >= timeStep) {

			// Update the Dynamics world with a constant time step, updates the transforms in the bodies
			world->update(timeStep);

			// Decrease the accumulated time 
			accumulator -= timeStep;
		}

		// time left in accumulator is smaller than delta time, so we normalise to get a value between 0 and 1 for lerping
		lerpFactor = accumulator / timeStep;
	}

	float PhysicsModule::GetLerpFactor()
	{
		return lerpFactor;
	}

	rp3d::PhysicsCommon* PhysicsModule::GetPhysicsCommon()
	{
		return &physicsCommon;
	}

	rp3d::PhysicsWorld* PhysicsModule::GetCurrentWorld()
	{
		return world;
	}

	// call these methods when loading in a new scene to initialise the physics world
	void PhysicsModule::ClearRigidBodies()
	{
		rigidBodies.clear();
	}

	void PhysicsModule::AddRigidBody()
	{
		// initialise a rigid body at the origin and place it in the vector of bodies
		rigidBodies.push_back(world->createRigidBody(rp3d::Transform::identity()));
	}

	rp3d::RigidBody* PhysicsModule::GetRigidBody(int index)
	{
		return rigidBodies[index];
	}

	void PhysicsModule::DestroyRigidBody(int index)
	{
		if (index < rigidBodies.size())
			world->destroyRigidBody(rigidBodies[index]);
	}
}