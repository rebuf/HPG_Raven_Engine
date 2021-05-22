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
#include "GUISystem.h"
#include "Scene/Scene.h"
#include "Scene/Component/GUIComponent.h"

namespace Raven 
{
	GUISystem::GUISystem()
	{

	}
	GUISystem::~GUISystem()
	{

	}

	void GUISystem::OnInit()
	{

	}

	void GUISystem::OnUpdate(float dt, Scene* scene)
	{
		// Get a reference to the scene registry
		auto& registry = scene->GetRegistry();

		// Look for all GUIMenu component entities
		auto guiView = registry.view<GUIComponent>();
		for (auto entity : guiView)
		{
			const auto mGUI = registry.try_get<GUIComponent>(entity);
			if (mGUI) mGUI->UpdateGUI();
		}
	}

	void GUISystem::OnImGui()
	{

	}
}