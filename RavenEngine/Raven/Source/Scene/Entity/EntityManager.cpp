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

#include "EntityManager.h"

namespace Raven
{
	Entity EntityManager::Create()
	{
		return Entity(registry.create(), scene);
	}

	Entity EntityManager::Create(const std::string& name)
	{
		auto e = registry.create();
		Entity entity(e, scene);
		entity.AddComponent<NameComponent>(name);
		return entity;
	}

	void EntityManager::Clear()
	{
		registry.each([&](auto entity) {
			registry.destroy(entity);
		});
		registry.clear();
	}

	Entity EntityManager::GetEntityByName(const std::string& name)
	{
		auto views = registry.view<NameComponent>();
		for (auto & view : views) 
		{
			auto & comp = registry.get<NameComponent>(view);
			if (comp.name == name) {
				return { view,scene };
			}
		}
		return { entt::null,nullptr };
	}

};
