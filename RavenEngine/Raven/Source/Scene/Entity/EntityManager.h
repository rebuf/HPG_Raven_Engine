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

#include <string>
#include "EntityGroup.h"

namespace Raven
{
	class Scene;

	class EntityManager final
	{
	public:
		EntityManager(Scene* initScene) : scene(initScene)
		{
		};

		Entity Create();
		Entity Create(const std::string& name);

		template<typename... Components>
		auto GetEntitiesWithTypes()
		{
			return registry.group<Components...>();
		}

		template<typename Component>
		EntityView<Component> GetEntitiesWithType()
		{
			return EntityView<Component>(scene);
		}


		Entity GetEntityByName(const std::string& name);

		template<typename R, typename T>
		void AddDependency();

		inline auto& GetRegistry(){ return registry; }

		void Clear();

	private:
		Scene* scene = nullptr;
		entt::registry registry;
	};

	template<typename R, typename T>
	void EntityManager::AddDependency()
	{
		registry.on_construct<R>().connect<&entt::registry::get_or_emplace<T>>();
	}

};