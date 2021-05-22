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
#pragma once


#include <string>
#include <queue>
#include <memory>
#include "Logger/Console.h"
#include "Utilities/Core.h"
#include "IModule.h"






namespace Raven
{
	class Scene;


	// SceneManager:
	//    - 
	class SceneManager final : public IModule
	{
		NOCOPYABLE(SceneManager);

	public:
		// Construct.
		SceneManager();

		// Destruct.
		~SceneManager();

		// Switch to another scene that exist/added to the scene manager.
		void SwitchToScene(const Scene* scene);
		void SwitchToScene(int32_t index);

		// Apply any pending changes.
		void Apply();

		inline Scene* GetCurrentScene() const { return currentScene; }
		inline int32_t GetCurrentIndex() const { return currentSceneIndex; }

		//...
		inline int32_t GetSceneCount() const { return static_cast<int32_t>(scenes.size()); }
		inline auto& GetScenes() const { return scenes; }
		std::vector<std::string> GetSceneNames();

		// Return true if we have pending scene switch.
		inline auto IsSwitchingScene() const  { return switchingScenes; }
		
		// Get Scene Index, return -1 if not found.
		int32_t GetSceneIndex(const Scene* inScene);
		int32_t GetSceneIndex(const std::string& sceneName);

		// Return a scene in the manager with scene name.
		Scene* GetSceneByName(const std::string& sceneName);

		// Return a scene at index.
		Scene* GetScene(int32_t index);

		// Add new scene to be managed, the scene pointer will be managed by this class.
		int32_t AddScene(Scene* scene);

		// Create/Add new scene to the manager.
		template<class TScene>
		Ptr<TScene> AddScene(const std::string& name);

		// Remove a scene with this name.
		void RemoveScene(const Scene* scene);

		// Save the current active scene.
		void SaveCurrentScene();

		// Load a saved scene from the resource manager and add it to the scene
		// @param path: relative resource path.
		Ptr<Scene> LoadScene(const std::string& path);

		// Unload all scenes.
		void UnloadScenes();

		// ~IModule
		virtual void Initialize() override;
		virtual void Destroy() override;
		static auto GetModuleType() { return MT_SceneManager; }

	private:
		// Set switch flag.
		inline auto SetSwitchScene(bool switching) { switchingScenes = switching; }

		// Create/Load Engine's default scene.
		void CreateOrLoadDefaultScene();

	protected:
		// The index of the current active scene.
		int32_t currentSceneIndex = 0;

		// The Current active scene.
		Scene* currentScene = nullptr;

		// all scenes in the Engine.
		std::vector< Ptr<Scene> > scenes;

	private:
		// If true there is a pending scene waiting to be applied.
		bool switchingScenes = false;

		// Pending scene waiting to be applied.
		int32_t queuedSceneIndex = -1;
	};


	template<class TScene>
	Ptr<TScene> SceneManager::AddScene(const std::string& name)
	{
		RAVEN_ASSERT(GetSceneIndex(name) == -1, "Scene name already exist.");

		LOGI("[{0}] - Enqueued scene : {1}", __FILE__, name.c_str());
		Ptr<Scene> newScene = scenes.emplace_back( std::make_shared<TScene>(name) );
		return std::static_pointer_cast<TScene, Scene>(newScene);
	}

}
