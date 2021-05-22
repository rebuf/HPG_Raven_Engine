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

#include "IModule.h"

#include "Scene/Scene.h"

#include "Utilities/Core.h"

namespace Raven
{
	// ideal time step of 1 / 60 s
	constexpr float timeStep = 1.0f / 60.0f;

	class PhysicsModule : public IModule
	{
	public:
		PhysicsModule();
		virtual ~PhysicsModule() override {};

		// inherited from IModule, must be overridden
		virtual void Initialize() override;
		virtual void Destroy() override;

		static EModuleType GetModuleType() { return EModuleType::MT_Physics; }
		
		// update the physics world
		void Step(float deltaTime);

		float GetLerpFactor();

		rp3d::PhysicsCommon* GetPhysicsCommon();
		rp3d::PhysicsWorld* GetCurrentWorld();

		// call these methods when loading in a new scene to initialise the physics world
		void ClearRigidBodies();

		void AddRigidBody();
		rp3d::RigidBody* GetRigidBody(int index);
		void DestroyRigidBody(int index);

	private:
		// singleton class for physics library's memory allocation etc
		rp3d::PhysicsCommon physicsCommon;
		// physics world where collisions and simulations are run
		rp3d::PhysicsWorld* world;

		// a list of pointers to rigid bodies
		std::vector<rp3d::RigidBody*> rigidBodies;

		// where we accumulate time between frames
		float accumulator;

		float lerpFactor;
	};
}
