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

#include "Editor.h"
#include "PropertiesWindow.h"
#include "HierarchyWindow.h"
#include "Render/RenderModule.h"

#include "Scene/SceneManager.h"
#include "Scene/Scene.h"
#include "Scene/Entity/Entity.h"
#include "Scene/Component/Component.h"
#include "Scene/Component/Light.h"
#include "Scene/Component/Transform.h"
#include "Scene/Component/MeshComponent.h"
#include "Scene/Component/SkinnedMeshComponent.h"
#include "Scene/Component/CameraControllerComponent.h"
#include "Scene/Component/RigidBody.h"
#include "Scene/Component/SoundComponent.h"

#include "Scripts/LuaComponent.h"

#include "ImGui/ImGuiHelpers.h"

#include "Core/Camera.h"
#include "Utilities/StringUtils.h"

#include "ResourceManager/ResourceManager.h"
#include "ResourceManager/MeshFactory.h"
#include "ResourceManager/Resources/Mesh.h"

#include "Animation/Animator.h"
#include "Animation/AnimationController.h"


#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <filesystem>
#include <functional>
#include <vector>
#include <memory>



namespace MM 
{
	using namespace Raven;

	template<>
	void ComponentEditorWidget<SoundComponent>(entt::registry& reg, entt::registry::entity_type e)
	{
		auto& sound = reg.get<SoundComponent>(e);
		sound.OnImGui();
	}

	template<>
	void ComponentEditorWidget<Transform>(entt::registry& reg, entt::registry::entity_type e)
	{
		auto& transform = reg.get<Transform>(e);

		auto rotation = glm::degrees(transform.GetRotationEuler());
		auto position = transform.GetPosition();
		auto scale = transform.GetScale();

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
		ImGui::Columns(2);
		ImGui::Separator();

		ImGui::TextUnformatted("Position");
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);

		if (ImGui::DragFloat3("##Position", glm::value_ptr(position), 0.05))
		{
			transform.SetPosition(position);
		}

		ImGui::PopItemWidth();
		ImGui::NextColumn();

		ImGui::TextUnformatted("Rotation");
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);
		

		if (ImGui::DragFloat3("##Rotation", glm::value_ptr(rotation),0.1))
		{
			rotation = glm::radians(rotation);
			transform.SetRotationEuler(rotation.x, rotation.y, rotation.z);
		}

		ImGui::PopItemWidth();
		ImGui::NextColumn();

		ImGui::TextUnformatted("Scale");
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);
		if (ImGui::DragFloat3("##Scale", glm::value_ptr(scale), 0.05))
		{
			transform.SetScale(scale);
		}

		ImGui::PopItemWidth();
		ImGui::NextColumn();

		ImGui::Columns(1);
		ImGui::Separator();
		ImGui::PopStyleVar();

		// also update the rigid body if available
		auto rb = reg.try_get<RigidBody>(e);
		if (rb)
		{
			rb->SetInitTransform(transform);
		}

	}

	template<>
	void ComponentEditorWidget<Light>(entt::registry& reg, entt::registry::entity_type e)
	{
		auto& light = reg.get<Light>(e);
	

		light.OnImGui();
	}

	template<>
	void ComponentEditorWidget<LuaComponent>(entt::registry& reg, entt::registry::entity_type e)
	{
		auto& lua = reg.get<LuaComponent>(e);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
		ImGui::Columns(2);
		ImGui::PushItemWidth(-1);
		
		if (ImGui::BeginCombo("", lua.GetFileName().c_str(), 0))
		{
			for (const auto& entry : std::filesystem::directory_iterator("./scripts/"))
			{
				if (!StringUtils::IsLuaFile(entry.path().string()))
				{
					continue;
				}

				bool isDir = std::filesystem::is_directory(entry);
				auto isSelected = StringUtils::GetFileName(lua.GetFileName()) == StringUtils::GetFileName(entry.path().string());

				if (!isDir && !isSelected)
				{
					if (ImGui::Selectable(entry.path().string().c_str()))
					{
						lua.SetFilePath(entry.path().string());
					}
				}
				if (isSelected)
					ImGui::SetItemDefaultFocus();

			}
			ImGui::EndCombo();
		}
		ImGui::NextColumn();
		if (ImGui::Button(lua.IsLoaded() ? "Reload" : "Load"))
		{
			lua.Reload();
		}
		ImGui::PopItemWidth();
		ImGui::Columns(1);
		ImGui::PopStyleVar();
		lua.OnImGui();
		
	}

	template<>
	void ComponentEditorWidget<Camera>(entt::registry& reg, entt::registry::entity_type e)
	{
		auto& camera = reg.get<Camera>(e);

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
		ImGui::Columns(2);
		ImGui::Separator();


		float aspect = camera.GetAspectRatio();
		
		if (ImGuiHelper::Property("Aspect", aspect, 0.0f, 10.0f))
			camera.SetAspectRatio(aspect);

		float fov = camera.GetFov();
		if (ImGuiHelper::Property("Fov", fov, 1.0f, 120.0f))
			camera.SetFov(fov);

		float near = camera.GetNear();
		if (ImGuiHelper::Property("Near", near, 0.01, 10.f))
			camera.SetNear(near);

		float far = camera.GetFar();
		if (ImGuiHelper::Property("Far", far, 10.0f, 10000.0f))
			camera.SetFar(far);

		float scale = camera.GetScale();
		if (ImGuiHelper::Property("Scale", scale, 0.0f, 1000.0f))
			camera.SetScale(scale);

		bool ortho = camera.IsOrthographic();
		if (ImGuiHelper::Property("Orthographic", ortho))
			camera.SetOrthographic(ortho);


		ImGui::Columns(1);
		ImGui::Separator();
		ImGui::PopStyleVar();

	}

	template<>
	void ComponentEditorWidget<CameraControllerComponent>(entt::registry& reg, entt::registry::entity_type e)
	{
		auto& controllerComp = reg.get<CameraControllerComponent>(e);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
		ImGui::Columns(2);
		ImGui::Separator();

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted("Controller Type");
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);

		const std::array<std::string,2> controllerTypes = { "FPS" ,"Editor"};
		std::string currentController = CameraControllerComponent::TypeToString(controllerComp.GetType());
		if (ImGui::BeginCombo("", currentController.c_str(), 0)) // The second parameter is the label previewed before opening the combo.
		{
			for (auto n = 0; n < controllerTypes.size(); n++)
			{
				bool isSelected = currentController == controllerTypes[n];
				if (ImGui::Selectable(controllerTypes[n].c_str(), currentController.c_str()))
				{
					controllerComp.SetControllerType(CameraControllerComponent::StringToType(controllerTypes[n]));
				}
				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		if (controllerComp.GetController())
			controllerComp.GetController()->OnImGui();

		ImGui::Columns(1);
		ImGui::Separator();
		ImGui::PopStyleVar();
	}

	template<>
	void ComponentEditorWidget<MeshComponent>(entt::registry& reg, entt::registry::entity_type e)
	{
		auto& meshComp = reg.get<MeshComponent>(e);

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
		ImGui::Columns(2);
		ImGui::Separator();



		// The Mesh Resoruce.
		{
			ImGui::Text("Mesh");
			ImGui::NextColumn();

			if (!meshComp.GetMesh())
			{
				ImGui::InputText("###meshresource", "", 0);
			}
			else
			{
				std::string editTextValue;
				editTextValue.append(meshComp.GetMesh()->GetResourcePath());
				editTextValue.resize(260);
				ImGui::InputText("###meshresource", editTextValue.data(), editTextValue.size());
			}

			// Drag and Drop mesh file...
			std::string shaderAsset = PropertiesWindow::ImGuiDragDropAssetsTarget();
			if (Engine::GetModule<ResourceManager>()->GetResourceType(shaderAsset) == RT_Mesh)
			{
				Ptr<Mesh> meshRsc = Engine::GetModule<ResourceManager>()->GetResource<Mesh>(shaderAsset);
				meshComp.SetMesh(meshRsc);
			}
		}

		
		// Iterate on mesh sections.
		if (meshComp.GetMesh())
		{
			auto meshRsc = meshComp.GetMesh();

			for (int32_t i = 0; i < meshRsc->GetMeshLOD(0).sections.size(); ++i)
			{
				ImGui::PushID(500);

				// Materials...
				auto mat = meshComp.GetMaterial(i);
				ImGui::NextColumn();
				ImGui::Text("Material (%d)", i);
				ImGui::NextColumn();

				if (!mat)
				{
					ImGui::InputText("###meshmaterial", "", 0);
				}
				else
				{
					std::string editTextValue;
					editTextValue.append(mat->GetResourcePath());
					editTextValue.resize(260);
					ImGui::InputText("###meshmaterial", editTextValue.data(), editTextValue.size());
				}

				// Drag and Drop materail file...
				std::string shaderAsset = PropertiesWindow::ImGuiDragDropAssetsTarget();
				if (Engine::GetModule<ResourceManager>()->GetResourceType(shaderAsset) == RT_Material)
				{
					Ptr<Material> matRsc = Engine::GetModule<ResourceManager>()->GetResource<Material>(shaderAsset);
					meshComp.SetMaterial(i, matRsc);
				}

				ImGui::PopID();
			}
		}

		ImGui::NextColumn();

		float ClipDistance = meshComp.GetClipDistance();
		if (ImGuiHelper::Property("ClipDistance", ClipDistance))
		{
			meshComp.SetClipDistance(ClipDistance);
		}

		bool castShadow = meshComp.IsCastShadow();
		if (ImGuiHelper::Property("CastShadow", castShadow))
		{
			meshComp.SetCastShadow(castShadow);
		}


		ImGui::Columns(1);
		ImGui::Separator();
		ImGui::PopStyleVar();

	}


	template<>
	void ComponentEditorWidget<SkinnedMeshComponent>(entt::registry& reg, entt::registry::entity_type e)
	{
		auto& skinnedComp = reg.get<SkinnedMeshComponent>(e);

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
		ImGui::Columns(2);
		ImGui::Separator();



		// The Mesh Resoruce.
		{
			ImGui::Text("Mesh");
			ImGui::NextColumn();

			if (!skinnedComp.GetMesh())
			{
				ImGui::InputText("###meshresource", "", 0);
			}
			else
			{
				std::string editTextValue;
				editTextValue.append(skinnedComp.GetMesh()->GetResourcePath());
				editTextValue.resize(260);
				ImGui::InputText("###meshresource", editTextValue.data(), editTextValue.size());
			}

			// Drag and Drop mesh file...
			std::string shaderAsset = PropertiesWindow::ImGuiDragDropAssetsTarget();
			if (Engine::GetModule<ResourceManager>()->GetResourceType(shaderAsset) == RT_SkinnedMesh)
			{
				Ptr<SkinnedMesh> meshRsc = Engine::GetModule<ResourceManager>()->GetResource<SkinnedMesh>(shaderAsset);
				skinnedComp.SetMesh(meshRsc);
			}
		}


		// Iterate on mesh sections.
		if (skinnedComp.GetMesh())
		{
			auto meshRsc = skinnedComp.GetMesh();

			for (int32_t i = 0; i < meshRsc->GetMeshSections().size(); ++i)
			{
				ImGui::PushID(500);

				// Materials...
				auto mat = skinnedComp.GetMaterial(i);
				ImGui::NextColumn();
				ImGui::Text("Material (%d)", i);
				ImGui::NextColumn();

				if (!mat)
				{
					ImGui::InputText("###meshmaterial", "", 0);
				}
				else
				{
					std::string editTextValue;
					editTextValue.append(mat->GetResourcePath());
					editTextValue.resize(260);
					ImGui::InputText("###meshmaterial", editTextValue.data(), editTextValue.size());
				}

				// Drag and Drop materail file...
				std::string shaderAsset = PropertiesWindow::ImGuiDragDropAssetsTarget();
				if (Engine::GetModule<ResourceManager>()->GetResourceType(shaderAsset) == RT_Material)
				{
					Ptr<Material> matRsc = Engine::GetModule<ResourceManager>()->GetResource<Material>(shaderAsset);
					skinnedComp.SetMaterial(i, matRsc);
				}

				ImGui::PopID();
			}
		}



		ImGui::NextColumn();
		ImGui::TextUnformatted("ClipDistance");
		ImGui::NextColumn();

		float ClipDistance = skinnedComp.GetClipDistance();
		if (ImGui::InputFloat("###clipdistance", &ClipDistance))
		{
			skinnedComp.SetClipDistance(ClipDistance);
		}


		ImGui::Columns(1);
		ImGui::Separator();
		ImGui::PopStyleVar();
	}


	template<>
	void ComponentEditorWidget<Animator>(entt::registry& reg, entt::registry::entity_type e)
	{
		auto& model = reg.get<Animator>(e);

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
		ImGui::Columns(2);
		ImGui::Separator();

		model.OnImGui();

		ImGui::Columns(1);
		ImGui::Separator();
		ImGui::PopStyleVar();
	}

	template<>
	void ComponentEditorWidget<RigidBody>(entt::registry& reg, entt::registry::entity_type e)
	{
		//
		// Edit rigid body properties
		//
		auto& rigidBody = reg.get<RigidBody>(e);

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
		ImGui::Columns(2);
		ImGui::Separator();

		bool canSleep = rigidBody.CanSleep();

		ImGui::TextUnformatted("Can sleep");
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);
		if (ImGui::Checkbox("##Can sleep", &canSleep))
		{
			rigidBody.SetIsAllowedToSleep(canSleep);
		}

		ImGui::PopItemWidth();
		ImGui::NextColumn();

		bool gravityEnabled = rigidBody.GravityEnabled();

		ImGui::TextUnformatted("Enable gravity");
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);
		if (ImGui::Checkbox("##Enable gravity", &gravityEnabled))
		{
			rigidBody.EnableGravity(gravityEnabled);
		}

		ImGui::PopItemWidth();
		ImGui::NextColumn();

		bool toppleEnabled = rigidBody.ToppleEnabled();

		ImGui::TextUnformatted("Enable toppling");
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);
		if (ImGui::Checkbox("##Enable toppling", &toppleEnabled))
		{
			rigidBody.EnableTopple(toppleEnabled);
		}

		ImGui::PopItemWidth();
		ImGui::NextColumn();

		float mass = rigidBody.GetMass();

		ImGui::TextUnformatted("Set mass");
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);
		if (ImGui::DragFloat("##Set mass", &mass, 0.05f, 0.0f, 100000.0f))
		{
			rigidBody.SetMass(mass);
		}
		
		ImGui::PopItemWidth();
		ImGui::NextColumn();

		float lDamping = rigidBody.GetLinearDamping();

		ImGui::TextUnformatted("Linear damping");
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);
		if (ImGui::DragFloat("##Linear damping", &lDamping, 0.05f, 0.0f, 1.0f))
		{
			rigidBody.SetLinearDamping(lDamping);
		}

		ImGui::PopItemWidth();
		ImGui::NextColumn();

		float aDamping = rigidBody.GetAngularDamping();

		ImGui::TextUnformatted("Angular damping");
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);
		if (ImGui::DragFloat("##Angular damping", &aDamping, 0.05f, 0.0f, 1.0f))
		{
			rigidBody.SetAngularDamping(aDamping);
		}

		ImGui::PopItemWidth();
		ImGui::NextColumn();

		static const char* types[3] = { "Static", "Kinematic", "Dynamic"};
		int selected = static_cast<int>(rigidBody.GetBodyType());

		ImGui::TextUnformatted("Set body type");
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);
		ImGui::ListBox("##Set body type", &selected, types, IM_ARRAYSIZE(types), 3);
		if (selected != static_cast<int>(rigidBody.GetBodyType()))
		{
			rigidBody.SetBodyType(static_cast<RigidBodyType>(selected));
		}

		ImGui::PopItemWidth();
		ImGui::NextColumn();

				
		//
		// Edit rigid body's colliders
		//


		if (rigidBody.GetNumColliders() != 0)
		{
			ImGui::Separator();
			ImGui::TextUnformatted("Select collider");
			ImGui::NextColumn();

			ImGui::PushItemWidth(-1);
			ImGui::ListBoxHeader("##Select collider", rigidBody.GetNumColliders(), 1);

			static int colliderIdx  = 0;
			auto colliders = rigidBody.GetAllColliders(); // ptr to the body's colliders
			// loop over all the body's colliders
			for (int i = 0; i < rigidBody.GetNumColliders(); i++)
			{
				const bool isSelected = colliderIdx == i;
				if (ImGui::Selectable(ColliderPrimitive::GetColliderPrimitiveName((*colliders)[i]->GetColliderType()).c_str(), isSelected))
				{
					colliderIdx = i;
				}
				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}

			}
			ImGui::ListBoxFooter();

			ImGui::PopItemWidth();
			ImGui::NextColumn();

			auto& collider = (*colliders)[colliderIdx];

			// ui for editing the type of collider
			collider->OnImGui();

			// ui for editing the collider's relative transform to the body it belongs to
			auto& transform = collider->GetRelativeTransform();

			auto rotation = glm::degrees(transform.GetRotationEuler());
			auto position = transform.GetPosition();
			// no scaling

			ImGui::TextUnformatted("Position");
			ImGui::NextColumn();
			ImGui::PushItemWidth(-1);

			if (ImGui::DragFloat3("##Position", glm::value_ptr(position), 0.05))
			{
				// update the tansform
				transform.SetPosition(position);
				// in the physics engine as well
				collider->SetTransform(transform);

			}

			ImGui::PopItemWidth();
			ImGui::NextColumn();

			ImGui::TextUnformatted("Rotation");
			ImGui::NextColumn();
			ImGui::PushItemWidth(-1);


			if (ImGui::DragFloat3("##Rotation", glm::value_ptr(rotation), 0.1))
			{
				transform.SetRotationEuler(glm::radians(rotation));
				// in the physics engine as well
				collider->SetTransform(transform);
			}

			ImGui::PopItemWidth();
			ImGui::NextColumn();

			ImGui::Columns(1);
			// button for removing the collider 
			if (ImGui::Button("Remove collider"))
			{
				rigidBody.RemoveCollider(colliderIdx);
			}
		}

		// 
		// Add colliders to the rigid body
		// 

		ImGui::Columns(1);
		ImGui::Separator();

		if (ImGui::BeginMenu("Add new collider"))
		{
			for (auto& name : ColliderPrimitive::NAMES)
			{
				if (ImGui::MenuItem(name.c_str()))
				{
					// add a collider to the rigid body
					rigidBody.AddCollider(ColliderFactory::CreateCollider(ColliderPrimitive::GetColliderPrimitiveType(name)));
				}
			}
			ImGui::EndMenu();
		}
		
		ImGui::Columns(2);
		ImGui::Separator();

		ImGui::Columns(1);
		ImGui::Separator();
		ImGui::PopStyleVar();
	}
};

namespace Raven
{

	constexpr size_t INPUT_BUFFER = 256;
	PropertiesWindow::PropertiesWindow()
	{
		title = "Properties";
	}

	void PropertiesWindow::OnImGui()
	{
		auto& editor = static_cast<Editor&>(Engine::Get());

		auto& registry = editor.GetModule<SceneManager>()->GetCurrentScene()->GetRegistry();


		auto selected = editor.GetSelected();

		if (ImGui::Begin(title.c_str(), &active))
		{
			auto ctrlPtr = controller.lock();
			if (ctrlPtr != nullptr)
			{
				ctrlPtr->OnImGui();
				ImGui::End();
				return;
			}

			if (selected == entt::null)
			{
				if (editor.GetShowGlobalSettings())
				{
					OnImGUIGlobalSettings();
				}

				ImGui::End();
				return;
			}

			auto scene = editor.GetModule<SceneManager>()->GetCurrentScene();

			auto activeComponent = registry.try_get<ActiveComponent>(selected);
			bool active = activeComponent ? activeComponent->active : true;
			if (ImGui::Checkbox("##ActiveCheckbox", &active))
			{
				if (!activeComponent)
					registry.emplace<ActiveComponent>(selected, active);
				else
					activeComponent->active = active;
			}
			ImGui::SameLine();
			ImGui::TextUnformatted(ICON_MDI_CUBE);
			ImGui::SameLine();

			bool hasName = registry.has<NameComponent>(selected);
			std::string name;
			if (hasName)
				name = registry.get<NameComponent>(selected).name;
			else
				name = std::to_string(entt::to_integral(selected));

			static char objName[INPUT_BUFFER];
			strcpy(objName, name.c_str());
		
			if (ImGui::InputText("##Name", objName, IM_ARRAYSIZE(objName)))
				registry.get_or_emplace<NameComponent>(selected).name = objName;

			ImGui::Separator();

			//enttEditor.RenderImGui(registry, selected);
			enttEditor.renderEditor(registry, selected);

			if (ImGui::BeginDragDropTarget())
			{
				auto data = ImGui::AcceptDragDropPayload("AssetFile", ImGuiDragDropFlags_None);
				if (data)
				{
					std::string file = (char*)data->Data;
					if (StringUtils::IsLuaFile(file))
					{
						auto scene = editor.GetModule<SceneManager>()->GetCurrentScene();
						auto name = StringUtils::GetFileName(file);
						registry.emplace<LuaComponent>(selected, file, scene).SetEntity_Evil(selected, scene);
					}
				}
				ImGui::EndDragDropTarget();
			}
		}
		ImGui::End();
	}


	void PropertiesWindow::OnImGUIGlobalSettings()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
		ImGui::Columns(2);
		ImGui::Separator();


		auto scene = Engine::GetModule<SceneManager>()->GetCurrentScene();
		auto& globalSettings = scene->GetGlobalSettings();

		// Sun & Sky Settings...
		bool dirtySky = ImGuiHelper::Property("Enable Sun", globalSettings.isSun);

		if (globalSettings.isSun)
		{
			dirtySky = ImGuiHelper::Property("Enable Sky", globalSettings.isSky) || dirtySky;

			dirtySky = ImGuiHelper::Property("Sun Lambda", globalSettings.sunAngles.x, 0.0f, 360.0f) || dirtySky;
			dirtySky = ImGuiHelper::Property("Sun Delta", globalSettings.sunAngles.y, 0.0f, 90.0f) || dirtySky;
			dirtySky = ImGuiHelper::Property("Sun Power", globalSettings.sunPower, 0.0f, 20.0f) || dirtySky;
			dirtySky = ImGuiHelper::Property("Sun Color", globalSettings.sunColor, 0.0f, 20.0f, ImGuiHelper::PropertyFlag::ColorProperty) || dirtySky;

		}


		if (dirtySky)
		{
			Engine::GetModule<RenderModule>()->RequestUpdateSky();
		}



		ImGui::Columns(1);
		ImGui::Separator();
		ImGui::PopStyleVar();
	}



	void PropertiesWindow::OnSceneCreated(Scene* scene)
	{
		auto& editor = static_cast<Editor&>(Engine::Get());
		auto& iconMap = editor.GetComponentIconMap();

		if (!init)
		{

#define TRIVIAL_COMPONENT(ComponentType,show) \
	{ \
		std::string name; \
		if(iconMap.find(typeid(ComponentType).hash_code()) != iconMap.end()) \
			name += iconMap[typeid(ComponentType).hash_code()]; \
        else \
            name += iconMap[typeid(Editor).hash_code()]; \
		name += "\t"; \
		name += ###ComponentType; \
		enttEditor.registerComponent<ComponentType>(name,show); \
	}

		TRIVIAL_COMPONENT(Transform,true);
		TRIVIAL_COMPONENT(Light, true);
		TRIVIAL_COMPONENT(Camera, true);
		TRIVIAL_COMPONENT(CameraControllerComponent, true);
		TRIVIAL_COMPONENT(MeshComponent, true);
		TRIVIAL_COMPONENT(SkinnedMeshComponent, true);
		TRIVIAL_COMPONENT(LuaComponent, true);
		TRIVIAL_COMPONENT(Animator, true);
		TRIVIAL_COMPONENT(RigidBody, true);
		TRIVIAL_COMPONENT(SoundComponent, true);

		init = true;
		}

		enttEditor.addCreateCallback([&](entt::registry & r, entt::entity entity) {
			// The Current Scene.
			Scene* scene = editor.GetModule<SceneManager>()->GetCurrentScene();

			auto lua = r.try_get<LuaComponent>(entity);

			if (lua) 
			{
				lua->SetScene(scene);
			}
			// Validate Components.
			if (scene)
			{
				scene->ValidateEntityComponents(r, entity);
			}
		});
	}


	std::string PropertiesWindow::ImGuiDragDropAssetsTarget()
	{
		if (ImGui::BeginDragDropTarget())
		{
			auto data = ImGui::AcceptDragDropPayload("AssetFile");
			if (data)
			{
				std::string file = (char*)data->Data;
				return file;
			}

			ImGui::EndDragDropTarget();
		}

		return "";
	}

};

