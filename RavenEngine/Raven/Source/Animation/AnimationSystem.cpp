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
#include "AnimationSystem.h"
#include "Animator.h"
#include "Animation.h"
#include "AnimationController.h"
#include "Engine.h"


#include "Scene/Component/SkinnedMeshComponent.h"


namespace Raven
{

	AnimationSystem::AnimationSystem()
	{
	}

	void AnimationSystem::OnInit()
	{
	}

	void AnimationSystem::OnUpdate(float dt, Scene* scene)
	{
		if (Engine::Get().GetEditorState() == EditorState::Play) 
		{
			auto animators = scene->GetRegistry().view<Animator>();

			for (auto e : animators)
			{
				auto& animator = scene->GetRegistry().get<Animator>(e);

				if (!animator.GetController())
					continue;

				Entity skinnedEnttity{ e, scene };
				SkinnedMeshComponent* skinnedComp = skinnedEnttity.TryGetComponent<SkinnedMeshComponent>();

				if (skinnedComp)
				{
					animator.GetController()->OnUpdate(dt, skinnedComp);
				}
			}
		}
	}

	void AnimationSystem::OnImGui()
	{
	}

};