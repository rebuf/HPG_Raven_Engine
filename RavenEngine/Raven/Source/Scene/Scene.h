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


#include "ResourceManager/Resources/IResource.h"
#include "Utilities/Core.h"


#include <string>
#include <sstream>
#include <memory>
#include <entt/entt.hpp>
#include <cereal/cereal.hpp>




namespace Raven
{
	class EntityManager;
	class Entity;
	class SceneGraph;
	class Camera;
	class Transform;


	// SceneGlobalSettings:
	//  - 
	struct SceneGlobalSettings
	{
		// Sun Angles in degrees to compute the directoin.
		glm::vec2 sunAngles;

		// Enable/Disable sun.
		bool isSun;

		// Enable/Disable Sky.
		bool isSky;

		// Sun power
		float sunPower;

		// Sun Color
		glm::vec3 sunColor;

		// Construct.
		SceneGlobalSettings()
			: sunAngles(0.0f, 45.0f)
			, isSun(true)
			, isSky(true)
			, sunPower(1.0f)
			, sunColor(1.0f)
		{

		}

		// Compute sun direction from angles.
		glm::vec3 GetSunDir();


		// Saving.
		template<typename Archive>
		void save(Archive& archive) const
		{
			archive(
				sunAngles,
				isSun,
				isSky,
				sunPower,
				sunColor
			);

		}

		// Loading.
		template<typename Archive>
		void load(Archive& archive)
		{
			archive(
				sunAngles,
				isSun,
				isSky,
				sunPower,
				sunColor
			);

		}


	};




	// Scene:
	//   - 
	//
	class Scene : public IResource
	{
	public:
		Scene(const std::string& name);

		// Destruct.
		virtual ~Scene()
		{

		}

		// return the resource type
		inline static EResourceType StaticGetType() noexcept { return EResourceType::RT_Scene; }


		virtual void OnInit();
		virtual void OnClean();
		virtual void OnUpdate(float dt);

		inline auto& GetEntityManager() { return entityManager; }

		inline auto& GetName() const { return name; };

		inline void SetName(const std::string& name) { this->name = name; }

		inline void SetInitCallback(const std::function<void(Scene* scene)>& call) { initCallback = call; }

		void SetSize(uint32_t w, uint32_t h);
		entt::registry& GetRegistry();

		// -- -- -- ---- -- - --- --- -
		// Scene Saving Operations.
		virtual void SaveToStream(std::stringstream& storage);
		virtual void LoadFromStream(std::istream& storage);
		virtual void SaveToFile(const std::string& inFilePath);
		virtual void LoadFromFile(const std::string& inFilePath);

		Entity CreateEntity();
		Entity CreateEntity(const std::string & name);

		void DuplicateEntity(const Entity& entity, const Entity& parent);
		void DuplicateEntity(const Entity& entity);


		Camera* GetTargetCamera();
		Transform* GetCameraTransform();

		inline auto SetOverrideCamera(Camera* overrideCamera) { this->overrideCamera = overrideCamera; }
		inline auto SetOverrideTransform(Transform* overrideTransform) { this->overrideTransform = overrideTransform; }
		inline auto SetForceCamera(bool forceShow) { this->forceShow = forceShow; }
		inline auto IsPreviewMainCamera() const { return forceShow; }

		// Make sure that all components in this entity has a valid reference to their entity id and scene.
		void ValidateEntityComponents(entt::registry& reg, entt::entity entity);

		// Get Scene Global Settings.
		SceneGlobalSettings& GetGlobalSettings() { return globalSettings; }
		const SceneGlobalSettings& GetGlobalSettings() const { return globalSettings; }


		template<typename Archive>
		void save(Archive& archive) const
		{
			archive(cereal::make_nvp("SceneName", name));

			// Start Archiving Global Settngs...
			if (RavenVersionGlobals::SCENE_ARCHIVE_VERSION >= 10002)
			{
				archive(globalSettings);
			}

		}


		template<typename Archive>
		void load(Archive& archive)
		{
			archive(cereal::make_nvp("SceneName", name));

			// Start Archiving Global Settngs...
			if (RavenVersionGlobals::SCENE_ARCHIVE_VERSION >= 10002)
			{
				archive(globalSettings);
			}

		}



	private:
		auto UpdateCameraController(float dt);


		bool forceShow = false;
		std::shared_ptr<SceneGraph> sceneGraph;
		std::shared_ptr<EntityManager> entityManager;
		std::string name;
		std::string loadName;
		uint32_t width = 0;
		uint32_t height = 0;
		SceneGlobalSettings globalSettings;

		NOCOPYABLE(Scene);

		void CopyComponents(const Entity& from, const Entity& to );
		bool inited = false;
		Camera* overrideCamera = nullptr;
		Transform* overrideTransform = nullptr;

		std::function<void(Scene *scene)> initCallback;
	};
};