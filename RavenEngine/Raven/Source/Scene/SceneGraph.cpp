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

#include "SceneGraph.h"
#include "Component/Component.h"
#include "Component/Transform.h"
#include "Component/GUIComponent.h"

namespace Raven 
{
	void SceneGraph::Init(entt::registry& registry)
	{
		registry.on_construct<Hierarchy>().connect<&Hierarchy::OnConstruct>();
		registry.on_update<Hierarchy>().connect<&Hierarchy::OnUpdate>();
		registry.on_destroy<Hierarchy>().connect<&Hierarchy::OnDestroy>();
	}

	void SceneGraph::DisconnectOnConstruct(bool disable, entt::registry& registry)
	{
		if (disable)
			registry.on_construct<Hierarchy>().disconnect<&Hierarchy::OnConstruct>();
		else
			registry.on_construct<Hierarchy>().connect<&Hierarchy::OnConstruct>();
	}

	void SceneGraph::Update(entt::registry& registry)
	{
		auto nonHierarchyView = registry.view<Transform>(entt::exclude<Hierarchy>);

		for (auto entity : nonHierarchyView)
		{
			// Only update if dirty, update this transform and all its children.
			registry.get<Transform>(entity).UpdateDirty();
		}

#if 0
		auto view = registry.view<Transform, Hierarchy>();
		for (auto entity : view)
		{
			const auto hierarchy = registry.try_get<Hierarchy>(entity);
			if (hierarchy && hierarchy->Parent() == entt::null)
			{
				//Recursively update children
				UpdateTransform(entity, registry);
			}
		}
#endif

	}


	void SceneGraph::UpdateTransform(entt::entity entity, entt::registry& registry)
	{
#if 0
		auto hierarchyComponent = registry.try_get<Hierarchy>(entity);
		if (hierarchyComponent)
		{
			auto transform = registry.try_get<Transform>(entity);
			if (transform)
			{
				if (hierarchyComponent->Parent() != entt::null)
				{
					auto parentTransform = registry.try_get<Transform>(hierarchyComponent->Parent());
					if (parentTransform)
					{
						transform->SetWorldMatrix(parentTransform->GetWorldMatrix());
					}
				}
				else
				{
					transform->SetWorldMatrix(glm::mat4{1.f});
				}
			}

			entt::entity child = hierarchyComponent->First();
			while (child != entt::null)
			{
				auto hierarchyComponent = registry.try_get<Hierarchy>(child);
				auto next = hierarchyComponent ? hierarchyComponent->Next() : entt::null;
				UpdateTransform(child, registry);
				child = next;
			}
		}
#endif
	}


};



