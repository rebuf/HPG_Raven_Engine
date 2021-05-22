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
#include "Engine.h"
#include "Utilities/StringUtils.h"
#include "Window/Window.h"


#include "IModule.h"
#include "Render/RenderModule.h"
#include "Physics/PhysicsModule.h"
#include "ResourceManager/ResourceManager.h"
#include "Scene/SceneManager.h"
#include "ImGui/ImGuiEngine.h"
#include "GUI/GUIModule.h"
#include "ProceduralGenerator/TerrainGeneration.h"
#include "ProceduralGenerator/ProceduralGenerator.h"




#include "Render/RenderDebug.h"
#include "Render/RenderTarget.h"



#include "Scene/Scene.h"
#include "Scene/Entity/Entity.h"
#include "Scene/Component/TerrainComponent.h"
#include "Scene/Component/RigidBody.h"
#include "Scene/Component/MeshComponent.h"
#include "Scene/Component/SkinnedMeshComponent.h"
#include "Scene/Component/Transform.h"
#include "Scene/Component/Light.h"
#include "Scene/System/SystemManager.h"
#include "Scene/System/PhysicsSystem.h"
#include "Scene/System/GUISystem.h"
#include "Core/Camera.h"



#include "ResourceManager/Resources/Mesh.h"
#include "ResourceManager/Resources/MaterialShader.h"
#include "ResourceManager/Resources/Material.h"
#include "ResourceManager/Resources/Terrain.h"
#include "ResourceManager/MeshFactory.h"

#include "ResourceManager/Importers/FBXImporter.h"
#include "Animation/Skeleton.h"
#include "Animation/Animator.h"



#include "Devices/Input.h"
#include "Scripts/LuaSystem.h"
#include "Scripts/LuaComponent.h"
#include "Animation/AnimationSystem.h"
#include "Scripts/LuaVirtualMachine.h"

#include "Audio/AudioSystem.h"


#include <memory>
#include <GLFW/glfw3.h>
#include <glm/gtx/string_cast.hpp>




namespace Raven {


Engine* Engine::instance = CreateEngine();




Engine::Engine()
	: engineTime(0.0f)
{

}


Engine::~Engine()
{

}


void Engine::Initialize()
{
	// Module
	LoadModules();
}


int Engine::Run()
{
	auto win = GetModule<Raven::Window>();



	// Main Loop...
	while (!win->ShouldClose())
	{
		//need to be refactored
		win->PollEvent();
		Input::GetInput()->ResetPressed();

		GetModule<Raven::ImGuiEngine>()->Prepare();

		// Draw..
		glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//time could be refactor with chrono
		double dt = glfwGetTime() - engineTime;
		engineTime = glfwGetTime();

		GetModule<Raven::SceneManager>()->Apply();

		OnUpdate(dt);
		OnRender();//To be modified by Renderer.
		OnImGui();
		GetModule<Raven::ImGuiEngine>()->Render();

		win->SwapBuffers();
	}


	// Cleanup...
	DestoryModules();

	return 0;
}


void Engine::OnImGui()
{

}


void Engine::OnUpdate(float dt)
{
	// Dispatch events
	eventDispatcher.DispatchEvents();

	// Update the system managers
	systemManager->OnUpdate(dt, GetModule<Raven::SceneManager>()->GetCurrentScene());

	// Update Current Scene.
	GetModule<Raven::SceneManager>()->GetCurrentScene()->OnUpdate(dt);

	// Update Render...
	GetModule<RenderModule>()->Update(dt);
}


void Engine::OnRender()
{
	Raven::RenderModule* renderer = GetModule<Raven::RenderModule>();

	// The Scene To Render.
	Scene* scene = GetModule<Raven::SceneManager>()->GetCurrentScene();

	// TODO: Better Sync between render, window and targets.
	glm::ivec2 extent = GetModule<Window>()->GetFramebufferSize();

	// Prepare Rendering...
	renderer->BeginRender(scene, extent);

	// Render...
	renderer->Render();

	// Render Cleanup...
	renderer->EndRender();
}


void Engine::LoadModules()
{
	// Create...
	systemManager = std::make_unique<SystemManager>();

	CreateModule<Raven::ResourceManager>();
	CreateModule<Raven::LuaVirtualMachine>();
	CreateModule<Raven::RenderModule>();
	CreateModule<Raven::ImGuiEngine>();
	CreateModule<Raven::Window>("Raven");
	CreateModule<Raven::SceneManager>();
	CreateModule<Raven::GUIModule>();
	CreateModule<Raven::ProceduralGenerator>();
	CreateModule<Raven::PhysicsModule>();
	


	Input::Create();

	// Initialize - Here order matter.
	InitializeModule<Raven::Window>();
	InitializeModule<Raven::LuaVirtualMachine>();
	InitializeModule<Raven::ResourceManager>();
	InitializeModule<Raven::RenderModule>();
	InitializeModule<Raven::ImGuiEngine>();
	InitializeModule<Raven::SceneManager>();
	InitializeModule<Raven::GUIModule>();
	InitializeModule<Raven::ProceduralGenerator>();
	InitializeModule<Raven::PhysicsModule>();

	//############	Register your system here ######################

	auto guiSystem = GetSystemManager()->AddSystem<GUISystem>();
	GetSystemManager()->AddSystem<PhysicsSystem>(); // register the physics system
	GetSystemManager()->AddSystem<LuaSystem>();
	GetSystemManager()->AddSystem<AnimationSystem>()->OnInit();

	auto audio = AudioSystem::Create();
	GetSystemManager()->AddSystem<AudioSystem>(audio);
	audio->OnInit();
	audio->SetPaused(true);


	guiSystem->OnInit();

	//############	Register your system here ######################

}


void Engine::DestoryModules()
{
	// Destroy - Here order matter.
	DestroyModule<ProceduralGenerator>();
	DestroyModule<RenderModule>();
	DestroyModule<SceneManager>();
	DestroyModule<GUIModule>();
	DestroyModule<ImGuiEngine>();
	DestroyModule<ResourceManager>();
	DestroyModule<Window>();
	DestroyModule<LuaVirtualMachine>();
}


std::future<bool> Engine::Post(const std::function<bool()>& callback)
{
	std::promise<bool> promise;
	std::future<bool> future = promise.get_future();
	std::lock_guard<std::mutex> locker(executeMutex);
	executeQueue.emplace(std::move(promise), callback);
	return future;
}


void Engine::OnSceneCreated(Scene* scene)
{

}


void Engine::Exit()
{
	// Tell the main window to close.
	GetModule<Window>()->Close();

}


void Engine::NewGameScene()
{
	Scene* newScene = new Scene("Physics Test World");

	// delete previous physics world
	auto physics = GetModule<PhysicsModule>();

	// now create a new one for the new scene

	LOGV(physics->GetCurrentWorld()->getNbRigidBodies());

	// creating a rigid body box
	auto entity1 = newScene->CreateEntity("Rigid Box 0");

	// give it a unit cube model
	auto& model1 = entity1.AddComponent<MeshComponent>();
	model1.SetMesh( MeshFactory::GetBasicShape(EBasicShape::Cube) );
	auto& luaScript = entity1.AddComponent<LuaComponent>();
	luaScript.SetFilePath("./scripts/PlayerController.lua");
	luaScript.LoadScript();

	// a transform
	auto& transform1 = entity1.GetOrAddComponent<Transform>();

	// and a rigidbody component
	auto& rb1 = entity1.AddComponent<RigidBody>();
	rb1.EnableGravity(false);
	rb1.SetMass(1.0);
	// configure its shapes
	//rb1.AddCollider(new BoxCollider());

	LOGV(physics->GetCurrentWorld()->getNbRigidBodies());

	{
		auto& lightEntity = newScene->CreateEntity("THE_SUN");
		auto& lightComp = lightEntity.GetOrAddComponent<Light>();
		auto& tr = lightEntity.GetOrAddComponent<Transform>();

		lightComp.type = (int32_t)LightType::DirectionalLight;
		lightComp.color = glm::vec4(1.0f, 0.95f, 0.8f, 1.0f);
		lightComp.intensity = 0.0f;
		lightComp.direction = glm::normalize(glm::vec3(-1.0f));
	}

	{
		Ptr<Mesh> plane = MeshFactory::GetBasicShape(EBasicShape::Quad);

		auto meshEntity = newScene->CreateEntity("Plane");
		auto& tr = meshEntity.GetOrAddComponent<Transform>();
		tr.SetPosition(glm::vec3(0.0f, -4.3f, 0.0f));
		tr.SetScale(glm::vec3(3000.0f));
		tr.SetRotationEuler(glm::vec3(-glm::half_pi<float>(), 0.0f, 0.0f));

		auto& model = meshEntity.GetOrAddComponent<MeshComponent>();
		model.SetMesh(plane);
	}

	{
		auto camEntity = newScene->CreateEntity("Camera");
		auto& tr = camEntity.GetOrAddComponent<Transform>();
		tr.SetPosition(glm::vec3(0.0f, 10.f, 0.0f));
		tr.SetScale(glm::vec3(1.0f));
		tr.SetRotation(glm::vec3(-glm::half_pi<float>(), 0.0f, 0.0f));

		auto& camera = camEntity.AddComponent<Camera>();
		camera.SetFov(90.f);
		camera.SetFar(100);
		camera.SetNear(0.01);
		camera.SetAspectRatio(4 / 3.f);

		// TODO: Use Entity ID, this reference could be deleted when ett::registry reallocate its containers.
		//newScene->SetOverrideCamera(&camera);
		//newScene->SetOverrideTransform(&tr);
	}


	{
		Ptr<SkinnedMesh> skMs = Engine::GetModule<ResourceManager>()->GetResource<SkinnedMesh>(
			"./assets/Meshes/YBot/SkMs_YBot.raven");

		auto yBot = newScene->CreateEntity("YBot_SkMesh");
		auto& skinnedMesh = yBot.GetOrAddComponent<SkinnedMeshComponent>();
		skinnedMesh.SetMesh(
			skMs
		);

		auto& tr = yBot.GetOrAddComponent<Transform>();
		tr.SetPosition(glm::vec3(0.0f, 10.f, 0.0f));
		tr.SetScale(glm::vec3(0.1f));

	}

	// Switch the scene....
	int32_t sceneIdx = GetModule<SceneManager>()->AddScene(newScene);

	// load scene using its id
	GetModule<SceneManager>()->SwitchToScene(sceneIdx);

}


void Engine::NewProceduralScene()
{
	glm::vec2 sceneSize = glm::vec2(1024.0f, 1024.0f);
	glm::vec2 sceneHeight = glm::vec2(-1.0f, 90.0f);
	Scene* scene = Engine::GetModule<ProceduralGenerator>()->GenerateNewScene(sceneSize, sceneHeight);
	Engine::GetModule<SceneManager>()->AddScene(scene);
	Engine::GetModule<SceneManager>()->SwitchToScene(scene);
}




} // End of namespace Raven
