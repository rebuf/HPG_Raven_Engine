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
#include "EntryPoint.h"
#include "Editor.h"
#include "SceneWindow.h"
#include "HierarchyWindow.h"
#include "PropertiesWindow.h"
#include "AssetsWindow.h"

#include "ImportWindow.h"
#include "ResourceWindow.h"

#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "Scene/Component/Component.h"
#include "Scene/Component/Transform.h"
#include "Scene/Component/Light.h"
#include "Scene/Component/RigidBody.h"
#include "Scene/Component/MeshComponent.h"
#include "Scene/Component/SkinnedMeshComponent.h"
#include "Scene/Entity/Entity.h"
#include "Scripts/LuaComponent.h"

#include "ResourceManager/ResourceManager.h"
#include "ResourceManager/MeshFactory.h"

#include "Devices/Input.h"
#include "Utilities/StringUtils.h"

#include "Core/Camera.h"
#include "Window/Window.h"
#include "NodeWindow.h"
#include "ImGui/ImGuiHelpers.h"
#include <ImGuizmo.h>
#include <imgui_internal.h>
#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Raven 
{
#define addWindow(T) editorWindows.emplace(typeid(T).hash_code(),std::make_shared<T>())

	void Editor::Initialize()
	{
		Engine::Initialize();

		addWindow(SceneWindow);
		addWindow(HierarchyWindow);
		addWindow(PropertiesWindow);
		addWindow(AssetsWindow);
		addWindow(ResourceWindow);
		addWindow(NodeWindow);
		addWindow(ImportWindow);


		iconMap[typeid(Transform).hash_code()] = ICON_MDI_VECTOR_LINE;
		iconMap[typeid(Editor).hash_code()] = ICON_MDI_SQUARE;
		iconMap[typeid(Light).hash_code()] = ICON_MDI_LIGHTBULB;
		iconMap[typeid(Camera).hash_code()] = ICON_MDI_CAMERA;
		iconMap[typeid(MeshComponent).hash_code()] = ICON_MDI_SHAPE;
		iconMap[typeid(SkinnedMeshComponent).hash_code()] = ICON_MDI_BONE;
		iconMap[typeid(LuaComponent).hash_code()] = ICON_MDI_SCRIPT;
		iconMap[typeid(RigidBody).hash_code()] = ICON_MDI_APPLE;

		ImGuizmo::SetGizmoSizeClipSpace(0.25f);
		auto winSize = Engine::Get().GetModule<Window>()->GetWindowSize();

		camera = std::make_unique<Camera>(
			45, 1.0, 3200000,winSize.x / winSize.y);

		SetEditorState(EditorState::Preview);

		OnSceneCreated(Engine::GetModule<SceneManager>()->GetCurrentScene());
	}

	void Editor::OnImGui()
	{
		DrawMenu();
		BeginDockSpace();
		for (auto& win : editorWindows)
		{
			win.second->OnImGui();
		}
		EndDockSpace();
		Engine::OnImGui();
	}

	void Editor::OnUpdate(float dt)
	{
		Engine::OnUpdate(dt);

		auto currentScene = GetModule<SceneManager>()->GetCurrentScene();

		if (GetEditorState() == EditorState::Preview)
		{
			auto& registry = GetModule<SceneManager>()->GetCurrentScene()->GetRegistry();

			if (IsSceneActive() && !currentScene->IsPreviewMainCamera())
			{
				const auto mousePos = Input::GetInput()->GetMousePosition();

				editorCameraController.HandleMouse(editorCameraTransform, dt, mousePos.x, mousePos.y);
				editorCameraController.HandleKeyboard(editorCameraTransform, dt);

			}

			if (!Input::GetInput()->IsMouseHeld(KeyCode::MouseKey::ButtonRight)
				&& !ImGuizmo::IsUsing() && IsSceneActive()
				&& selectedNode != entt::null
				&& !currentScene->IsPreviewMainCamera())
			{
				if (Input::GetInput()->IsKeyPressed(KeyCode::Id::Q))
				{
					SetImGuizmoOperation(4);
				}

				if (Input::GetInput()->IsKeyPressed(KeyCode::Id::W))
				{
					SetImGuizmoOperation(ImGuizmo::OPERATION::TRANSLATE);
				}

				if (Input::GetInput()->IsKeyPressed(KeyCode::Id::E))
				{
					SetImGuizmoOperation(ImGuizmo::OPERATION::ROTATE);
				}

				if (Input::GetInput()->IsKeyPressed(KeyCode::Id::R))
				{
					SetImGuizmoOperation(ImGuizmo::OPERATION::SCALE);
				}

				if (Input::GetInput()->IsKeyPressed(KeyCode::Id::T))
				{
					SetImGuizmoOperation(ImGuizmo::OPERATION::BOUNDS);
				}
			}
			
			//editorCameraTransform.SetWorldMatrixTransform(glm::mat4(1.f));
		}

	}

	void Editor::DrawPlayButtons()
	{

		auto x = (ImGui::GetWindowContentRegionMax().x / 2.0f) - (1.5f * (ImGui::GetFontSize() + ImGui::GetStyle().ItemSpacing.x));
		//ImGui::SameLine();
		
		ImGui::Dummy({ x,0 });

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.2f, 0.7f, 0.0f));

		if (Engine::Get().GetEditorState() == EditorState::Next)
			Engine::Get().SetEditorState(EditorState::Paused);


		bool isStartState = Engine::Get().GetEditorState() == EditorState::Play 
			|| Engine::Get().GetEditorState() == EditorState::Paused;

		bool isPausedState = Engine::Get().GetEditorState() == EditorState::Paused;

		// --- -- - --- -- -- - - -- - 
		// Play/Stop Button...
		{
			if (isStartState)
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.28f, 0.56f, 0.9f, 1.0f));

			if (ImGui::Button(isStartState ? ICON_MDI_STOP : ICON_MDI_PLAY))
			{
				if (isStartState)
				{
					StopPlay();
				}
				else
				{
					StartPlay();
				}
			}


			ImGuiHelper::Tooltip("Play");


			if (isStartState)
				ImGui::PopStyleColor();
		}

		// --- -- - --- -- -- - - -- - 
		// Play/Paused Button...
		{

			if (isPausedState)
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.28f, 0.56f, 0.9f, 1.0f));

			if (ImGui::Button(ICON_MDI_PAUSE))
			{
				PausePlay();
			}

			ImGuiHelper::Tooltip("Pause");

			if (isPausedState)
				ImGui::PopStyleColor();
		}

		// --- -- - --- -- -- - - -- - 
		// Next Button...
#if 0  // Not Sure what next does to the sceen. purpose in here.
		{
			selected = Engine::Get().GetEditorState() == EditorState::Next;
			if (selected)
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.28f, 0.56f, 0.9f, 1.0f));

			if (ImGui::Button(ICON_MDI_STEP_FORWARD))
				Engine::Get().SetEditorState(EditorState::Next);


			ImGuiHelper::Tooltip("Next");

			if (selected)
				ImGui::PopStyleColor();
		}
#endif

		//ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 240.0f);
		ImGui::PopStyleColor();

	}

	void Editor::SetSelected(const entt::entity& node)
	{
		prevSelectedNode = selectedNode;
		selectedNode = node;
		showGlobalSettings = false;
	}

	void Editor::SelectGlobalSettings()
	{
		prevSelectedNode = selectedNode;
		selectedNode = entt::null;
		showGlobalSettings = true;
	}

	void Editor::SetCopiedEntity(const entt::entity& selectedNode,bool cut) 
	{
		copiedNode = selectedNode;
	}

	void Editor::OnSceneCreated(Scene* scene)
	{
		for (auto & w : editorWindows)
		{
			w.second->OnSceneCreated(scene);
		}
	}

	void Editor::OnImGuizmo()
	{
		auto view = glm::inverse(editorCameraTransform.GetWorldMatrix());
		auto proj = camera->GetProjectionMatrix();

		if (selectedNode == entt::null || imGuizmoOperation == ImGuizmo::SELECT)
			return;

		if (showGizmos)
		{
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetOrthographic(camera->IsOrthographic());

			auto& registry = Editor::GetModule<SceneManager>()->GetCurrentScene()->GetRegistry();
			auto transform = registry.try_get<Transform>(selectedNode);
			if (transform != nullptr)
			{
				auto model = transform->GetWorldMatrix();
				float delta[16];

				ImGuizmo::Manipulate(
					glm::value_ptr(view),
					glm::value_ptr(proj),
					static_cast<ImGuizmo::OPERATION>(imGuizmoOperation),
					ImGuizmo::LOCAL,
					glm::value_ptr(model),
					delta,
					nullptr);

				if (ImGuizmo::IsUsing())
				{
					if (static_cast<ImGuizmo::OPERATION>(imGuizmoOperation) == ImGuizmo::OPERATION::SCALE)
					{
						auto mat = glm::make_mat4(delta);
						
						transform->SetScale(gizmoOrigScale * Transform::ExtractScale(mat));
					}
					else if (static_cast<ImGuizmo::OPERATION>(imGuizmoOperation) == ImGuizmo::OPERATION::ROTATE)
					{
						auto mat = glm::make_mat4(delta) * transform->GetWorldMatrix();
						transform->SetWorldMatrixTransform(mat);
					}
					else
					{
						auto mat = glm::make_mat4(delta) * transform->GetLocalMatrix();
						transform->SetMatrixTransform(mat);
					}
				}
				else
				{
					gizmoOrigScale = transform->GetScale();
				}
			}
		}
	}

	void Editor::SelectObject(const Ray& ray)
	{
		/*auto& registry = GetModule<SceneManager>()->GetCurrentScene()->GetRegistry();
		float closestEntityDist = std::numeric_limits<float>::infinity();

		entt::entity currentClosestEntity = entt::null;

		auto group = registry.group<Model>(entt::get<Transform>);

		for (auto entity : group)
		{
			const auto& [model, trans] = group.get<Model, Transform>(entity);

			auto& meshes = model.GetMeshes();

			for (auto mesh : meshes)
			{
				if (mesh->IsActive())
				{
					auto& worldTransform = trans.GetWorldMatrix();

					auto bbCopy = mesh->GetBoundingBox()->Transformed(worldTransform);
					float dist = ray.HitDistance(bbCopy);

					if (dist < std::numeric_limits<float>::infinity())
					{
						if (dist < closestEntityDist)
						{
							closestEntityDist = dist;
							currentClosestEntity = entity;
						}
					}
				}
			}
		}

		if (selectedNode != entt::null)
		{
			if (selectedNode == currentClosestEntity)
			{
				
				auto& trans = registry.get<Transform>(selectedNode);
				auto& model = registry.get<Model>(selectedNode);
				auto bb = model.GetMeshes().front()->GetBoundingBox()->Transformed(trans.GetWorldMatrix());

				FocusCamera(trans.GetWorldPosition(), (bb.max_ - bb.min_).Length());
			}

			selectedNode = currentClosestEntity;
			return;
		}*/
	}

	void Editor::OpenFile(const std::string& filePath)
	{
		EResourceType rscType = Engine::GetModule<ResourceManager>()->GetResourceType(filePath);

		switch (rscType)
		{
		case Raven::RT_AnimationController:
			GetWindow<NodeWindow>()->OpenFile(filePath);
			break;

		case Raven::RT_Texture2D:
		case Raven::RT_MaterialShader:
		case Raven::RT_Material:
		case Raven::RT_Mesh:
		case Raven::RT_SkinnedMesh:
		{
			GetWindow<ResourceWindow>()->SetResource(filePath);
		}
			break;

		case Raven::RT_Scene:
			OpenScene(filePath);
			break;

		case Raven::RT_Skeleton:
			LOGW("Editor - Open Skeleton Not Supported.");
			break;

		case Raven::RT_AnimationClip:
			LOGW("Editor - Open AnimationClip Not Supported.");
			break;
		}

	}

	const char* Editor::GetIconFontIcon(const std::string& filePath)
	{
		if (StringUtils::IsTextFile(filePath))
		{
			return ICON_MDI_FILE_XML;
		}
		else if (StringUtils::IsModelFile(filePath))
		{
			return ICON_MDI_SHAPE;
		}
		else if (StringUtils::IsAudioFile(filePath))
		{
			return ICON_MDI_FILE_MUSIC;
		}
		else if (StringUtils::IsTextureFile(filePath))
		{
			return ICON_MDI_FILE_IMAGE;
		}

		return ICON_MDI_FILE;
	}

	Ray Editor::SendScreenRay(int32_t x, int32_t y, Camera* camera, int32_t width, int32_t height)
	{
		if (!camera)
			return Ray();

		float screenX = (float)x / (float)width;
		float screenY = (float)y / (float)height;

		bool flipY = true;

		return camera->GetScreenRay(screenX, screenY, glm::inverse(editorCameraTransform.GetWorldMatrix()), flipY);
	}

	void Editor::DrawMenu()
	{
		//ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {0,10});
		if (ImGui::BeginMainMenuBar())
		{

			if (ImGui::BeginMenu("File"))
			{
				uint32_t size = Engine::GetModule<ResourceManager>()->GetNumPendingSave();
				std::string save = "Save Resources - Pending(" +std::to_string(size) + ")";
				if (ImGui::MenuItem(save.c_str()))
				{
					Engine::GetModule<ResourceManager>()->SavePending();
				}


				if (ImGui::MenuItem("Save Scene"))
				{
					Engine::GetModule<SceneManager>()->SaveCurrentScene();
				}

				ImGui::Spacing();
				ImGui::Separator();
				ImGui::Spacing();

				if (ImGui::MenuItem("New Procedural Scene"))
				{
					Engine::Get().NewProceduralScene();
				}

				ImGui::Spacing();
				ImGui::Separator();
				ImGui::Spacing();


				if (ImGui::MenuItem("Exit"))
				{
					Engine::Get().Exit();
				}

				ImGui::EndMenu();
			}


			DrawPlayButtons();
			ImGui::EndMainMenuBar();
		
		}
		//ImGui::PopStyleVar();
	
	}

	void Editor::BeginDockSpace()
	{
		static bool p_open = true;
		static bool opt_fullscreen_persistant = true;
		static ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
		bool opt_fullscreen = opt_fullscreen_persistant;

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();

			auto pos = viewport->Pos;
			auto size = viewport->Size;
			bool menuBar = true;
			if (menuBar)
			{
				const float infoBarSize = ImGui::GetFrameHeight();
				pos.y += infoBarSize;
				size.y -= infoBarSize;
			}

			ImGui::SetNextWindowPos(pos);
			ImGui::SetNextWindowSize(size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize
				| ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		if (opt_flags & ImGuiDockNodeFlags_DockSpace)
			window_flags |= ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("MyDockspace", &p_open, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		ImGuiID DockspaceID = ImGui::GetID("MyDockspace");

		if (!ImGui::DockBuilderGetNode(DockspaceID))
		{
			ImGui::DockBuilderRemoveNode(DockspaceID); // Clear out existing layout
			ImGui::DockBuilderAddNode(DockspaceID); // Add empty node
			ImGui::DockBuilderSetNodeSize(DockspaceID, ImGui::GetIO().DisplaySize);

			ImGuiID dock_main_id = DockspaceID;
			ImGuiID DockBottom = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.3f, nullptr, &dock_main_id);
			ImGuiID DockLeft = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.2f, nullptr, &dock_main_id);
			ImGuiID DockRight = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.20f, nullptr, &dock_main_id);

			ImGuiID DockLeftChild = ImGui::DockBuilderSplitNode(DockLeft, ImGuiDir_Down, 0.875f, nullptr, &DockLeft);
			ImGuiID DockRightChild = ImGui::DockBuilderSplitNode(DockRight, ImGuiDir_Down, 0.875f, nullptr, &DockRight);
			ImGuiID DockingLeftDownChild = ImGui::DockBuilderSplitNode(DockLeftChild, ImGuiDir_Down, 0.06f, nullptr, &DockLeftChild);
			ImGuiID DockingRightDownChild = ImGui::DockBuilderSplitNode(DockRightChild, ImGuiDir_Down, 0.06f, nullptr, &DockRightChild);

			ImGuiID DockBottomChild = ImGui::DockBuilderSplitNode(DockBottom, ImGuiDir_Down, 0.2f, nullptr, &DockBottom);
			ImGuiID DockingBottomLeftChild = ImGui::DockBuilderSplitNode(DockBottomChild, ImGuiDir_Left, 0.5f, nullptr, &DockBottomChild);
			ImGuiID DockingBottomRightChild = ImGui::DockBuilderSplitNode(DockBottomChild, ImGuiDir_Right, 0.5f, nullptr, &DockBottomChild);

			ImGuiID DockMiddle = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.8f, nullptr, &dock_main_id);

			ImGui::DockBuilderDockWindow("Scene", DockMiddle);
			ImGui::DockBuilderDockWindow("Animator", DockMiddle);

			ImGui::DockBuilderDockWindow("Properties", DockRight);

			ImGui::DockBuilderDockWindow("Assets", DockingBottomRightChild);
			ImGui::DockBuilderDockWindow("GraphicsInfo", DockLeft);
			ImGui::DockBuilderDockWindow("ApplicationInfo", DockLeft);
			ImGui::DockBuilderDockWindow("Hierarchy", DockLeft);

			ImGui::DockBuilderFinish(DockspaceID);
		}

		// Dockspace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGui::DockSpace(DockspaceID, ImVec2(0.0f, 0.0f), opt_flags);
		}
	}

	void Editor::EndDockSpace()
	{
		ImGui::End();
	}

	void Editor::NewSceneForStartPlay()
	{
		// Saven Origianl as cache.
		std::stringstream scene_cache;
		originalScene->SaveToStream(scene_cache);

		auto newPlayScene = GetModule<SceneManager>()->AddScene<Scene>("Play_Scene");
		newPlayScene->LoadFromStream(scene_cache);
		newPlayScene->SetName("Play_Scene");

		// Switch to play scene.
		GetModule<SceneManager>()->SwitchToScene(newPlayScene.get());
		GetModule<SceneManager>()->Apply();

		auto rbView = newPlayScene->GetRegistry().view<RigidBody, Transform>();


		for (auto& e : rbView)
		{
			auto initT = rbView.get<Transform>(e);

			initT.SetScale(glm::vec3(1.0f, 1.0f, 1.0f));

			auto& rb = rbView.get<RigidBody>(e);

			LOGC(rb.GetBodyIndex());
			rb.SetInitTransform(initT);
			rb.SetTransform(initT);
		}

		OnSceneCreated(newPlayScene.get());

		playScene = newPlayScene;
		LOGI(newPlayScene->GetName().c_str());
	}

	void Editor::ReloadOriginalScene()
	{
		// Serialize the scene
		for (auto & win : editorWindows)
		{
			win.second->SaveWorkspace(); // ???
		}

		// Switch to play scene.
		GetModule<SceneManager>()->SwitchToScene(originalScene);
		GetModule<SceneManager>()->Apply();

		auto* rmScene = playScene.lock().get();
		GetModule<SceneManager>()->RemoveScene(rmScene);
	}

	void Editor::StartPlay()
	{
		Engine::Get().SetEditorState(EditorState::Play);

		// The original scene we want to revert to when we stop playing.
		originalScene = Engine::Get().GetModule<SceneManager>()->GetCurrentScene();

		// we reload the scene, so destroy the rigid bodies in the physics engine
		auto& registry = originalScene->GetRegistry();

		// Create a copy of the original scene to start playing...
		NewSceneForStartPlay();
	}

	void Editor::StopPlay()
	{
		Engine::Get().SetEditorState(EditorState::Preview);
		ReloadOriginalScene();
	}

	void Editor::PausePlay()
	{
		if (Engine::Get().GetEditorState() == EditorState::Play)
			Engine::Get().SetEditorState(EditorState::Paused);
		else
			Engine::Get().SetEditorState(EditorState::Play);
	}

	void Editor::OpenScene(const std::string& file)
	{
		SetSelected(entt::null);

		// First unload all opend scenes...
		Engine::GetModule<SceneManager>()->UnloadScenes();

		auto& newScene = Engine::GetModule<SceneManager>()->LoadScene(file);
		Engine::GetModule<SceneManager>()->SwitchToScene(newScene.get());
		Engine::GetModule<SceneManager>()->Apply();

		OnSceneCreated(Engine::GetModule<SceneManager>()->GetCurrentScene());
	}
};

Raven::Engine* CreateEngine() {
	return new Raven::Editor();
}
