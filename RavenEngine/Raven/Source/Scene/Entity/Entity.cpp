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
#include "Entity.h"
#include "Utilities/StringUtils.h"

namespace Raven
{
	bool Entity::IsActive()
	{
		if (HasComponent<ActiveComponent>())
			return scene->GetRegistry().get<ActiveComponent>(entityHandle).active;

		return true;
	}

	void Entity::SetActive(bool isActive)
	{
		GetOrAddComponent<ActiveComponent>().active = isActive;
	}

	void Entity::SetParent(const Entity& entity)
	{
		bool acceptable = false;
		auto hierarchyComponent = TryGetComponent<Hierarchy>();
		if (hierarchyComponent != nullptr)
		{
			acceptable = entity.entityHandle != entityHandle && (!entity.IsParent(*this)) && (hierarchyComponent->Parent() != entityHandle);
		}
		else
			acceptable = entity.entityHandle != entityHandle;

		if (!acceptable)
			return;

		if (hierarchyComponent)
			Hierarchy::Reparent(entityHandle, entity.entityHandle, scene->GetRegistry(), *hierarchyComponent);
		else
		{
			//scene->GetRegistry().emplace<Hierarchy>(entityHandle, entity.entityHandle);
			AddComponent<Hierarchy>();
		}
	}

	Entity Entity::FindByPath(const std::string& path)
	{
		if (path == "") {
			return {};
		}
		Entity ent = {};
		auto layers = StringUtils::Split(path,"/");
		auto children = GetChildren();

		for (int32_t i = 0; i < layers.size(); ++i)
		{
			bool findChild = false;

			if (layers[i] == "..")
			{
				ent = GetParent();
			}
			else
			{
				for (auto entt : children)
				{
					auto & nameComp = entt.GetComponent<NameComponent>();
					if (layers[i] == nameComp.name) 
					{
						ent = entt;
						children = ent.GetChildren();
						break;
					}
				}
			}
		}
		return ent;
	}

	Entity Entity::GetChildInChildren(const std::string& name)
	{
		auto children = GetChildren();
		for (auto entt : children)
		{
			auto& nameComp = entt.GetComponent<NameComponent>();
			if (name == nameComp.name)
			{
				return entt;
			}
			auto ret =  entt.GetChildInChildren(name);
			if (ret.Valid()) {
				return ret;
			}
		}
		return {};
	}

	Entity Entity::GetParent()
	{
		auto hierarchyComp = TryGetComponent<Hierarchy>();
		if (hierarchyComp)
			return Entity(hierarchyComp->Parent(), scene);
		else
			return Entity(entt::null, nullptr);
	}

	std::vector<Entity> Entity::GetChildren()
	{
		std::vector<Entity> children;
		auto hierarchyComponent = TryGetComponent<Hierarchy>();
		if (hierarchyComponent)
		{
			entt::entity child = hierarchyComponent->First();
			while (child != entt::null && scene->GetRegistry().valid(child))
			{
				children.emplace_back(child, scene);
				hierarchyComponent = scene->GetRegistry().try_get<Hierarchy>(child);
				if (hierarchyComponent)
					child = hierarchyComponent->Next();
			}
		}
		return children;
	}

	bool Entity::IsParent(const Entity & potentialParent) const 
	{
		auto nodeHierarchyComponent = scene->GetRegistry().try_get<Hierarchy>(entityHandle);
		if (nodeHierarchyComponent)
		{
			auto parent = nodeHierarchyComponent->Parent();
			while (parent != entt::null)
			{
				if (parent == potentialParent.entityHandle)
				{
					return true;
				}
				else
				{
					nodeHierarchyComponent = scene->GetRegistry().try_get<Hierarchy>(parent);
					parent = nodeHierarchyComponent ? nodeHierarchyComponent->Parent() : entt::null;
				}
			}
		}
		return false;
	}

	void Entity::Destroy()
	{
		scene->GetRegistry().destroy(entityHandle);
	}

	bool Entity::Valid()
	{
		return scene && scene->GetRegistry().valid(entityHandle);
	}

};
