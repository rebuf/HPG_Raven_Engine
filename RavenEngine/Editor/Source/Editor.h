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
#include <entt/entt.hpp>
#include "Engine.h"
#include "EditorWindow.h"
#include "Scene/Component/Transform.h"
#include "Core/EditorCamera.h"
#include "Core/Ray.h"


#include "glm/vec3.hpp"



namespace Raven
{
	class Camera;

	class Editor : public Engine
	{
	public:
		void Initialize() override;
		void OnImGui() override;
		void OnUpdate(float dt) override;
		void DrawPlayButtons();

		void SetSelected(const entt::entity& selectedNode);
		void SelectGlobalSettings();
		void SetCopiedEntity(const entt::entity& selectedNode,bool cut = false);

		inline auto& GetSelected() const { return selectedNode; }
		inline auto& GetPrevSelected() const { return prevSelectedNode; }
		inline auto& GetCopiedEntity() const  { return copiedNode; }
		inline auto IsCutCopyEntity() const { return cutCopyEntity; }
		inline auto& GetComponentIconMap() const { return iconMap; }
		inline auto& GetComponentIconMap(){ return iconMap; }

		void OnSceneCreated(Scene* scene) override;

		inline auto GetImGuizmoOperation() const { return imGuizmoOperation; }
		inline auto SetImGuizmoOperation(uint32_t imGuizmoOperation) { this->imGuizmoOperation = imGuizmoOperation; }

		inline auto& GetCamera() { return camera; }
		inline auto& GetEditorCameraTransform()	{return editorCameraTransform;}

		inline auto& GetEditorCameraController() { return editorCameraController; }

		void OnImGuizmo();

		Ray SendScreenRay(int32_t x, int32_t y, Camera* camera, int32_t width, int32_t height);
		void SelectObject(const Ray& ray);

		void OpenFile(const std::string& file);

		const char* GetIconFontIcon(const std::string& filePath);


		void OpenScene(const std::string& file);

		template<class T>
		inline auto GetWindow() { return
			std::static_pointer_cast<T>(editorWindows[typeid(T).hash_code()]);
		}

		//
		inline bool GetShowGlobalSettings() const{ return showGlobalSettings; }

	private:
		void DrawMenu();
		void BeginDockSpace();
		void EndDockSpace();

		// Starting playing current scene.
		void StartPlay();

		// Stop playing current scene.
		void StopPlay();

		// Pause playing current scene.
		void PausePlay();

		// Create new play scene and start playing it.
		void NewSceneForStartPlay();

		// Reload original scene and destroy cached scene.
		void ReloadOriginalScene();


		std::unordered_map<size_t, std::shared_ptr<EditorWindow>> editorWindows;

		entt::entity selectedNode = entt::null;
		entt::entity prevSelectedNode = entt::null;
		entt::entity copiedNode = entt::null;
		bool cutCopyEntity = false;
		std::unordered_map<size_t, const char*> iconMap;
		// ImGuizmo::OPERATION::  
		//ImGuizmo::SELECT == 4
		uint32_t imGuizmoOperation = 4;

		bool showGizmos = true;
		glm::vec3 gizmoOrigScale = glm::vec3(1.0f);

		std::unique_ptr<Camera> camera;
		Transform editorCameraTransform;
		EditorCameraController editorCameraController;

		// The original scene before loading or any modifcations.
		Scene* originalScene = nullptr;

		// The dummy scene we created just to play on.
		WeakPtr<Scene> playScene;

		// If enable show scene global settings.
		bool showGlobalSettings = false;
	};

};
