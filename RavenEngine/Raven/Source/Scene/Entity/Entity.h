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
#include "Scene/Scene.h"
#include "Scene/SceneGraph.h"
#include "Scene/Component/Component.h"

namespace Raven
{
	class Entity
	{
	public:

		Entity() = default;

		Entity(entt::entity handle, Scene* s)
			: entityHandle(handle)
			, scene(s)
		{
		}

		~Entity()
		{
		}

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
#ifdef RAVEN_DEBUG
			if (HasComponent<T>())
				LOGW("Attempting to add extisting component ");
#endif
			T & t = scene->GetRegistry().emplace<T>(entityHandle, std::forward<Args>(args)...);
			t.entity = entityHandle;
			t.sceneOwner = scene;

			return t;
		}

		template<typename T, typename... Args>
		T& GetOrAddComponent(Args&&... args)
		{
			T & t = scene->GetRegistry().get_or_emplace<T>(entityHandle, std::forward<Args>(args)...);
			t.entity = entityHandle;
			t.sceneOwner = scene;
			return t;
		}

		template<typename T, typename... Args>
		void AddOrReplaceComponent(Args&&... args)
		{
			T & t = scene->GetRegistry().emplace_or_replace<T>(entityHandle, std::forward<Args>(args)...);
			t.entity = entityHandle;
			t.sceneOwner = scene;
		}

		template<typename T>
		T& GetComponent()
		{
			return scene->GetRegistry().get<T>(entityHandle);
		}

		template<typename T>
		T* TryGetComponent()
		{
			return scene->GetRegistry().try_get<T>(entityHandle);
		}

		template<typename T>
		T* TryGetComponentFromParent()
		{
			auto t =  scene->GetRegistry().try_get<T>(entityHandle);
			if (t == nullptr) 
			{
				auto parent = GetParent();
				while (parent.Valid() && t == nullptr) {
					t = parent.TryGetComponent<T>();
					parent = parent.GetParent();
				}
			}
			return t;
		}

		template<typename T>
		bool HasComponent()
		{
			return scene->GetRegistry().has<T>(entityHandle);
		}

		template<typename T>
		void RemoveComponent()
		{
			return scene->GetRegistry().remove<T>(entityHandle);
		}

		bool IsActive();
		void SetActive(bool isActive);
		void SetParent(const Entity& entity);
		Entity FindByPath(const std::string& path);
		Entity GetChildInChildren(const std::string& name);
		Entity GetParent();
		std::vector<Entity> GetChildren();
		bool IsParent(const Entity& potentialParent) const;

		inline operator entt::entity() const { return entityHandle; }
		inline operator uint32_t() const { return (uint32_t)entityHandle; }
		inline operator bool() const { return entityHandle != entt::null && scene; }

		inline auto operator==(const Entity& other) const { return entityHandle == other.entityHandle && scene == other.scene; }
		inline auto operator!=(const Entity& other) const { return !(*this == other); }

		inline auto GetHandle() const { return entityHandle; }
		inline auto SetHandle(entt::entity en) { entityHandle = en; }
		inline auto GetScene() const { return scene; }
		inline auto SetScene(Scene* sc) { scene = sc; }

		void Destroy();
		bool Valid();



	private:
		entt::entity entityHandle = entt::null;
		Scene* scene = nullptr;
		friend class EntityManager;
	};
};
