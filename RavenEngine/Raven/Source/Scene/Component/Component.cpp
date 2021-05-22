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
#include "Component.h"
#include "Scene/Entity/Entity.h"
#include "Engine.h"
#include "Scene/SceneManager.h"

namespace Raven
{
	Hierarchy::Hierarchy(entt::entity p)
		:parent(p)
	{
		first = entt::null;
		next = entt::null;
		prev = entt::null;
	}
	Hierarchy::Hierarchy()
	{
	}
	bool Hierarchy::Compare(const entt::registry& registry, entt::entity rhs) const
	{
		if (rhs == entt::null || rhs == parent || rhs == prev)
		{
			return true;
		}

		if (parent == entt::null)
		{
			return false;
		}
		else
		{
			auto& thisParent = registry.get<Hierarchy>(parent);
			auto& rhsParent = registry.get<Hierarchy>(rhs).parent;
			if (thisParent.Compare(registry, parent))
			{
				return true;
			}
		}
		return false;
	}

	void Hierarchy::Reset()
	{
		parent = entt::null;
		first = entt::null;
		next = entt::null;
		prev = entt::null;
	}

	void Hierarchy::OnConstruct(entt::registry& registry, entt::entity entity)
	{
		auto& hierarchy = registry.get<Hierarchy>(entity);
		if (hierarchy.parent != entt::null) 
		{
 			auto& parentHierarchy = registry.get_or_emplace<Hierarchy>(hierarchy.parent);

			if (parentHierarchy.first == entt::null)
			{
				parentHierarchy.first = entity;
			}
			else
			{
				auto prevEnt = parentHierarchy.first;
				auto currentHierarchy = registry.try_get<Hierarchy>(prevEnt);

				while (currentHierarchy != nullptr && currentHierarchy->next != entt::null)
				{
					prevEnt = currentHierarchy->next;
					currentHierarchy = registry.try_get<Hierarchy>(prevEnt);
				}
				currentHierarchy->next = entity;
				hierarchy.prev = prevEnt;
			}
		}
	}

	void Hierarchy::OnDestroy(entt::registry& registry, entt::entity entity)
	{
		auto& hierarchy = registry.get<Hierarchy>(entity);
		if (hierarchy.prev == entt::null || !registry.valid(hierarchy.prev))
		{
			if (hierarchy.parent != entt::null && registry.valid(hierarchy.parent))
			{
				auto parentHierarchy = registry.try_get<Hierarchy>(hierarchy.parent);
				if (parentHierarchy != nullptr)
				{
					parentHierarchy->first = hierarchy.next;
					if (hierarchy.next != entt::null)
					{
						auto nextHierarchy = registry.try_get<Hierarchy>(hierarchy.next);
						if (nextHierarchy != nullptr)
						{
							nextHierarchy->prev = entt::null;
						}
					}
				}
			}
		}
		else
		{
			auto prevHierarchy = registry.try_get<Hierarchy>(hierarchy.prev);
			if (prevHierarchy != nullptr)
			{
				prevHierarchy->next = hierarchy.next;
			}
			if (hierarchy.next != entt::null)
			{
				auto nextHierarchy = registry.try_get<Hierarchy>(hierarchy.next);
				if (nextHierarchy != nullptr)
				{
					nextHierarchy->prev = hierarchy.prev;
				}
			}
		}
	}

	void Hierarchy::OnUpdate(entt::registry& registry, entt::entity entity)
	{
		auto& hierarchy = registry.get<Hierarchy>(entity);
		// if is the first child
		if (hierarchy.prev == entt::null)
		{
			if (hierarchy.parent != entt::null)
			{
				auto parent_hierarchy = registry.try_get<Hierarchy>(hierarchy.parent);
				if (parent_hierarchy != nullptr)
				{
					parent_hierarchy->first = hierarchy.next;
					if (hierarchy.next != entt::null)
					{
						auto next_hierarchy = registry.try_get<Hierarchy>(hierarchy.next);
						if (next_hierarchy != nullptr)
						{
							next_hierarchy->prev = entt::null;
						}
					}
				}
			}
		}
		else
		{
			auto prevHierarchy = registry.try_get<Hierarchy>(hierarchy.prev);
			if (prevHierarchy != nullptr)
			{
				prevHierarchy->next = hierarchy.next;
			}
			if (hierarchy.next != entt::null)
			{
				auto next_hierarchy = registry.try_get<Hierarchy>(hierarchy.next);
				if (next_hierarchy != nullptr)
				{
					next_hierarchy->prev = hierarchy.prev;
				}
			}
		}
	}

	void Hierarchy::Reparent(entt::entity ent, entt::entity parent, entt::registry& registry, Hierarchy& hierarchy)
	{
		Hierarchy::OnDestroy(registry, ent);

		hierarchy.parent = entt::null;
		hierarchy.next = entt::null;
		hierarchy.prev = entt::null;

		if (parent != entt::null)
		{
			hierarchy.parent = parent;
			Hierarchy::OnConstruct(registry, ent);
		}
	}

	Raven::Entity Component::GetEntity() const
	{
		return { entity, sceneOwner ? sceneOwner : Engine::GetModule<SceneManager>()->GetCurrentScene() };
	}

	entt::entity Component::GetEntityHandle() const
	{
		return entity;
	}

}

