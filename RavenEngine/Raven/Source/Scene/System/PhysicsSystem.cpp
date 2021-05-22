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

#include "Engine.h"

#include "Physics/PhysicsModule.h"

#include "Scene/Scene.h"
#include "Scene/System/PhysicsSystem.h"
#include "Scene/Component/CollisionBody.h"
#include "Scene/Component/RigidBody.h"

//
// System for managing all physics components
//

namespace Raven
{
	PhysicsSystem::PhysicsSystem()
	{

	}

	PhysicsSystem::~PhysicsSystem()
	{

	}

	void PhysicsSystem::OnInit()
	{

	}

	// called before advancing the physics simulation
	void PhysicsSystem::OnUpdate(float dt, Scene* scene)
	{
		if (Engine::Get().GetEditorState() == EditorState::Play)
		{
			// Update physics world, generates new transforms for entities with rigid bodies
			Engine::Get().GetModule<PhysicsModule>()->Step(dt);

			//LOGE(Engine::Get().GetModule<PhysicsModule>()->GetCurrentWorld()->getNbRigidBodies());

			// get all entities with collision bodies and tranforms
			/*
			*/
			auto& registry = scene->GetRegistry();
			auto& group = scene->GetRegistry().group<RigidBody, Transform>();
			for (auto entity : group)
			{
				const auto& [rigBod, trans] = group.get<RigidBody, Transform>(entity);
				auto scale = rigBod.initTransform.GetScale();


				trans.SetTransform(Rp3dConvert::ToTransform(rp3d::Transform::interpolateTransforms(rigBod.GetPreviousState(), rigBod.GetCurrentState(), Engine::Get().GetModule<PhysicsModule>()->GetLerpFactor()), scale)); // interpolate states to get transform used in rendering
				rigBod.SetPreviousState(rigBod.GetCurrentState()); // set previous state to current state 
			}
		}
	}

	void PhysicsSystem::OnImGui()
	{

	}
}