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

#include "LuaSystem.h"
#include "Scene/Scene.h"
#include "LuaComponent.h"
#include "Engine.h"

namespace Raven 
{
	void LuaSystem::OnInit()
	{
		
	}

	void LuaSystem::OnUpdate(float dt, Scene* scene)
	{
		if (Engine::Get().GetEditorState() == EditorState::Play) 
		{
			auto view = scene->GetRegistry().view<LuaComponent>();
			for (auto v : view)
			{
				auto& lua = scene->GetRegistry().get<LuaComponent>(v);
				lua.OnUpdate(dt);
			}
		}
	
	}

	void LuaSystem::OnImGui()
	{

	}
};