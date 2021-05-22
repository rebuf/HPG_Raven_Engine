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

#include "Scene.h"
#include "Entity/Entity.h"
#include "Entity/EntityManager.h"
#include "SceneGraph.h"
#include "Scene/Component/Transform.h"
#include "Scene/Component/Light.h"
#include "Scene/Component/CameraControllerComponent.h"
#include "Scene/Component/MeshComponent.h"
#include "Scene/Component/SkinnedMeshComponent.h"
#include "Scene/Component/RigidBody.h"
#include "Scene/Component/SoundComponent.h"
#include "Audio/AudioSource.h"


#include "Scripts/LuaComponent.h"
#include "Core/CameraController.h"

#include "Render/RenderModule.h"
#include "Render/RenderTarget.h"

#include "Utilities/StringUtils.h"
#include "Core/Camera.h"
#include "Devices/Input.h"

#include "Animation/Animator.h"

#include <fstream>
#include "Engine.h"
#include "Utilities/Serialization.h"
// for serialization
#include "cereal/archives/json.hpp"
#include "cereal/archives/binary.hpp"

#include <glm/gtx/string_cast.hpp>



namespace Raven { 


	// --- -- - --- - --- -- ---- - - - -- --- - - --- --- -- ---- -- --

	// Allow us to iterate on components of an entity.
	struct IEnttCompGetter
	{
		virtual Component* GetComp(entt::registry& r, entt::entity entity) = 0;
	};

	// Allow us to iterate on this component type.
	template<class CompType>
	struct EnttCompGetter : public IEnttCompGetter
	{
		virtual Component* GetComp(entt::registry& r, entt::entity entity) override { return r.try_get<CompType>(entity); }
	};

	// Store all components getter.
	static std::vector< Ptr<IEnttCompGetter> > entt_comp_getters;

	// Register getter for a component type.
	template<class ...Args>
	struct EnttCompGetterRegister;

	// Register getter for a component type.
	template<>
	struct EnttCompGetterRegister<>
	{
		static void Register()
		{

		}
	};

	// Register a gtter for an argument of components.
	template<class CompType, class ...CompArgs>
	struct EnttCompGetterRegister<CompType, CompArgs...>
	{
		static void Register()
		{
			entt_comp_getters.emplace_back(new EnttCompGetter<CompType>());
			EnttCompGetterRegister<CompArgs...>::Register();
		}
	};

	// --- -- - --- - --- -- ---- - - - -- --- - - --- --- -- ---- -- --



	Scene::Scene(const std::string& initName)
		: IResource()
		, name(initName)
	{
		type = Scene::StaticGetType();

		LOGV("{0} {1}", __FUNCTION__,initName);
		entityManager = std::make_shared<EntityManager>(this);

		entityManager->AddDependency<Camera, Transform>();
		entityManager->AddDependency<MeshComponent, Transform>();
		entityManager->AddDependency<SkinnedMeshComponent, Transform>();
		entityManager->AddDependency<Light, Transform>();
		entityManager->AddDependency<RigidBody, Transform>();

		sceneGraph = std::make_shared<SceneGraph>();
		sceneGraph->Init(entityManager->GetRegistry());

		// Register Enttity Components Getter...
		if (entt_comp_getters.empty())
		{
			EnttCompGetterRegister<ALL_COMPONENTS>::Register();
		}
	}

	entt::registry& Scene::GetRegistry()
	{
		return entityManager->GetRegistry();
	}

	void Scene::SetSize(uint32_t w, uint32_t h)
	{
		width = w;
		height = h;
	}


	void Scene::ValidateEntityComponents(entt::registry& reg, entt::entity entity)
	{
		for (auto getter : entt_comp_getters)
		{
			auto* comp = getter->GetComp(reg, entity);
			if (comp)
			{
				comp->SetEntity_Evil(entity, this);
			}
		}
	}


	void Scene::SaveToStream(std::stringstream& storage)
	{
		PRINT_FUNC();

		cereal::JSONOutputArchive output{ storage };
		output(*this);
		entt::snapshot{ entityManager->GetRegistry() }.entities(output).component<ALL_COMPONENTS>(output);
	}


	void Scene::SaveToFile(const std::string& filePath)
	{
		// First save to stream.
		std::stringstream storage;
		SaveToStream(storage);
		std::ofstream file(filePath, std::ios::binary);

		file << storage.str();
		file.flush();
		file.close();
	}


	void Scene::LoadFromStream(std::istream& storage)
	{
		PRINT_FUNC();

		entityManager->Clear();
		sceneGraph->DisconnectOnConstruct(true, entityManager->GetRegistry());

		cereal::JSONInputArchive input(storage);
		input(*this);
		entt::snapshot_loader{ entityManager->GetRegistry() }.entities(input).component<ALL_COMPONENTS>(input);

		sceneGraph->DisconnectOnConstruct(false, entityManager->GetRegistry());

		// Make sure all loaded components have valid scene/entity.
		{
			auto* scene = this;

			scene->GetRegistry().each([scene](auto entity) {
				scene->ValidateEntityComponents(scene->GetRegistry(), entity);
			});
		}
	
	}


	void Scene::LoadFromFile(const std::string& filePath)
	{
		std::ifstream in(filePath);
		if (in.good())
		{
			std::string data;
			in.seekg(0, std::ios::end);
			auto len = in.tellg();
			in.seekg(0, std::ios::beg);
			data.resize(len);
			in.read(data.data(), len);
			in.close();

			std::istringstream istr;
			istr.str(data);

			LoadFromStream(istr);
		}
		else
		{
			LOGE("No saved scene file found {0}", filePath);
			in.close();
		}
	}


	Raven::Entity Scene::CreateEntity()
	{
		return entityManager->Create();
	}

	Raven::Entity Scene::CreateEntity(const std::string& name)
	{
		return entityManager->Create(name);
	}

	void Scene::DuplicateEntity(const Entity& entity, const Entity& parent)
	{
		Entity newEntity = entityManager->Create();
		
		if (parent)
			newEntity.SetParent(parent);

		CopyComponents(entity, newEntity);
	}

	void Scene::DuplicateEntity(const Entity& entity)
	{
		Entity newEntity = entityManager->Create();
		//COPY
		CopyComponents(entity,newEntity);
	}

	Camera* Scene::GetTargetCamera()
	{
		auto camsEttView = entityManager->GetEntitiesWithType<Camera>();
		if ((!camsEttView.Empty() && Engine::Get().GetEditorState() == EditorState::Play) || forceShow)
		{
			Camera& sceneCam = camsEttView.Front().GetComponent<Camera>();
			return &sceneCam;
		}

		return overrideCamera;
	}

	Raven::Transform* Scene::GetCameraTransform()
	{
		auto camsEttView = entityManager->GetEntitiesWithType<Camera>();
		if ((!camsEttView.Empty() && Engine::Get().GetEditorState() == EditorState::Play) || forceShow)
		{
			Transform& sceneCamTr = camsEttView[0].GetComponent<Transform>();
			return &sceneCamTr;
		}

		return overrideTransform;
	}


	void Scene::CopyComponents(const Entity& from, const Entity& to)
	{
		LOGW("Not implementation {0}", __FUNCTION__);
	}


	void Scene::OnInit()
	{
		if (initCallback != nullptr) 
		{
			initCallback(this);
		}

		// initialise all rigid bodies in the scene
		int rbIdx = 0;
		auto view = GetRegistry().view<RigidBody>();
		for (auto v : view)
		{
			auto& rb = view.get<RigidBody>(v);
			// set the rigid body index
			rb.SetBodyIndex(rbIdx);

			Engine::Get().GetModule<PhysicsModule>()->AddRigidBody();

			// when creating a rigid body, we need to set some data... 
			rb.InitRigidBody(rb.GetBodyIndex());

			// also initialise the colliders
			for (auto& collider : *rb.GetAllColliders())
			{
				collider->SetBody(Engine::Get().GetModule<PhysicsModule>()->GetRigidBody(rbIdx++));
				collider->InitShape(Engine::Get().GetModule<PhysicsModule>()->GetPhysicsCommon());
			}
		}

		auto luaView = GetRegistry().view<LuaComponent>();
		for (auto v : luaView)
		{
			auto& lua = GetRegistry().get<LuaComponent>(v);
			lua.OnInit();
		}
	}


	void Scene::OnClean()
	{
		// on scene clean, we need to destroy the physics engine
		auto view = GetRegistry().view<RigidBody>();
		for (auto v : view)
		{
			auto& rb = GetRegistry().get<RigidBody>(v);
			Engine::Get().GetModule<PhysicsModule>()->DestroyRigidBody(rb.GetBodyIndex());
		}
		Engine::Get().GetModule<PhysicsModule>()->ClearRigidBodies();
	}


	auto Scene::UpdateCameraController(float dt)
	{
		// Update AspectRatio of the camera to match render target.
		if (GetTargetCamera())
		{
			float rtRatio = Engine::GetModule<RenderModule>()->GetSceneRT()->GetAspectRatio();
			GetTargetCamera()->SetAspectRatio(rtRatio);
		}

		auto controller = entityManager->GetRegistry().group<CameraControllerComponent>(entt::get<Transform>);
		for (auto entity : controller)
		{
			const auto mousePos = Input::GetInput()->GetMousePosition();
			auto& [con, trans] = controller.get<CameraControllerComponent, Transform>(entity);
			if (Engine::Get().IsSceneActive() && 
				Engine::Get().GetEditorState() == EditorState::Play&&
				con.GetController() )
			{
				con.GetController()->HandleMouse(trans, dt, mousePos.x, mousePos.y);
				con.GetController()->HandleKeyboard(trans, dt);
			}
		}
	}

	void Scene::OnUpdate(float dt)
	{
		UpdateCameraController(dt);
		sceneGraph->Update(entityManager->GetRegistry());
	}


	glm::vec3 SceneGlobalSettings::GetSunDir()
	{
		glm::quat rotation =
				glm::angleAxis(glm::radians(sunAngles.x), UP)
			* glm::angleAxis(glm::radians(glm::clamp(sunAngles.y, 0.0f, 89.9f)), RIGHT);

		return rotation * FORWARD;
	}

};
