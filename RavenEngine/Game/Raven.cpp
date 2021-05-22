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
#include "Raven.h"
#include "EntryPoint.h"
#include "Engine.h"
#include "Logger/Console.h"
#include "GUI/GUIModule.h"

#include "ResourceManager/ResourceManager.h"

#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "Scene/Component/Component.h"
#include "Scene/Component/Transform.h"
#include "Scene/Component/Light.h"
#include "Scene/Entity/Entity.h"
#include "Scene/System/SystemManager.h"
#include "Scene/System/GUISystem.h"

#include "Scene/Component/GUIComponent.h"
#include "ResourceManager/Resources/Texture2D.h"


namespace Raven
{
	void RavenGame::Initialize()
	{
		Raven::Engine::Initialize();
		//auto scene = new Scene("controller_test.raven");

		//GetModule<SceneManager>()->AddScene(scene);

		//scene->SetInitCallback([](Scene* _scene) {

		//	});


	}

	void RavenGame::OnImGui() 
	{
		Engine::OnImGui();
	}

	void RavenGame::OnUpdate(float dt)
	{
		Engine::OnUpdate(dt);
	}

}

Raven::Engine* CreateEngine() 
{
	return new Raven::RavenGame();
}