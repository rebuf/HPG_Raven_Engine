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

#pragma  once
#include <string>
#include <imgui.h>
#include <memory>
#include "EditorWindow.h"
#include <entt/entt.hpp>
namespace Raven 
{
	class HierarchyWindow : public EditorWindow
	{
	public:
		HierarchyWindow();
		virtual void OnImGui()  override;
		inline auto IsDraging() const { return draging; }
	private:
		void DrawName();
		void DrawGlobalSettings();
		void PopupWindow();
		void DrawFilter();
		void DragEntity();
		void DrawNode(const entt::entity & node, entt::registry& registry);
		bool IsParentOfEntity(entt::entity entity, entt::entity child, entt::registry& registry);
		void DestroyEntity(entt::entity entity, entt::registry& registry);
		ImGuiTextFilter hierarchyFilter;
		entt::entity doubleClickedEntity =  entt::null;
		entt::entity droppedEntity = entt::null;
		entt::entity recentDroppedEntity = entt::null;
		bool draging = false;

	};
};