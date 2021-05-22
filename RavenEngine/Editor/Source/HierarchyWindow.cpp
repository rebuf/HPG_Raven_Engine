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

#include "HierarchyWindow.h"
#include "Scene/Component/Component.h"
#include "Scene/SceneManager.h"
#include "Scene/Scene.h"
#include "Scene/Entity/EntityManager.h"
#include "Scene/Entity/Entity.h"
#include "Scene/Component/Light.h"
#include "Scene/Component/MeshComponent.h"
#include "Scene/Component/SkinnedMeshComponent.h"
#include "Scripts/LuaComponent.h"
#include "Core/Camera.h"
#include "Scene/Component/Transform.h"
#include "ImGui/ImGuiHelpers.h"
#include "IconsMaterialDesignIcons.h"
#include "Editor.h"
#include "ResourceManager/Resources/Mesh.h"
#include "ResourceManager/MeshFactory.h"


#include <imgui_internal.h>

namespace Raven
{
	constexpr size_t INPUT_BUFFER = 256;

	HierarchyWindow::HierarchyWindow()
	{
		title = "Hierarchy";
	}

	void HierarchyWindow::OnImGui() 
	{
		const auto flags = ImGuiWindowFlags_NoCollapse;

		ImGui::Begin(title.c_str(), &active, flags);
		{
		
			if (ImGui::IsMouseClicked(1)) {
				ImGui::OpenPopup("HierarchyWindow::PopupWindow");
			}
			PopupWindow();
		
			DrawName();
			DrawGlobalSettings();
			

			DrawFilter();
			DragEntity();

		
		}
		ImGui::End();
	}



	void HierarchyWindow::DrawName()
	{
		auto scene = Editor::Get().GetModule<SceneManager>()->GetCurrentScene();
		auto& registry = scene->GetRegistry();

		const auto & sceneName = scene->GetName();

		static char objName[INPUT_BUFFER];
		strcpy(objName, sceneName.c_str());

		ImGui::PushItemWidth(-1);
		if (ImGui::InputText("##Name", objName, IM_ARRAYSIZE(objName), 0))
			scene->SetName(objName);
		ImGui::Separator();
	}

	void HierarchyWindow::DrawGlobalSettings()
	{
		ImGui::Spacing();

		if (ImGui::Button("Global Settings", ImVec2(-1.0f, 0.0f)))
		{
			static_cast<Editor&>(Engine::Get()).SelectGlobalSettings();
		}

		ImGui::Spacing();
	}

	void HierarchyWindow::PopupWindow()
	{
		auto scene = Editor::Get().GetModule<SceneManager>()->GetCurrentScene();
		auto& registry = scene->GetRegistry();
		if (ImGui::BeginPopupContextWindow("HierarchyWindow::PopupWindow"))
		{
			if (ImGui::Selectable("Add Empty Entity"))
			{
				scene->CreateEntity("Empty Entity");
			}

			if (ImGui::Selectable("Add Light"))
			{
				auto entity = scene->CreateEntity("Light");
				entity.AddComponent<Light>();
				entity.GetOrAddComponent<Transform>();
			}

			const char* shapes[] = { "Sphere", "Cube", "Pyramid", "Capsule", "Cylinder", "Quad" };

			if (ImGui::BeginMenu("Add 3D Object"))
			{
				for (auto name : shapes)
				{
					if (ImGui::MenuItem(name))
					{
						auto entity = scene->CreateEntity(name);
						auto & model = entity.AddComponent<MeshComponent>();
						auto& tr = entity.GetOrAddComponent<Transform>();
						
						auto mesh = Ptr<Mesh>(MeshFactory::GetBasicShape(EBasicShape::GetPrimativeName(name)));
						model.SetMesh(mesh);
					}
				}
				ImGui::EndMenu();
			}
			if (ImGui::Selectable("Add Camera"))
			{
				auto entity = scene->CreateEntity("Camera");
				auto& camera = entity.AddComponent<Camera>();
				camera.SetFov(45.f);
				camera.SetFar(320000.0);
				camera.SetNear(1.0);
				camera.SetAspectRatio(4 / 3.f);
				entity.GetOrAddComponent<Transform>();
			}


			ImGui::EndPopup();
		}
	}

	void HierarchyWindow::DrawFilter()
	{

		ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImGui::GetStyleColorVec4(ImGuiCol_TabActive));
		ImGui::TextUnformatted(ICON_MDI_MAGNIFY);
		ImGui::SameLine();
		hierarchyFilter.Draw("##HierarchyFilter", ImGui::GetContentRegionAvail().x - ImGui::GetStyle().IndentSpacing);
		ImGui::PopStyleColor();
		ImGui::Unindent();
	}

	void HierarchyWindow::DragEntity()
	{
		auto scene = Editor::Get().GetModule<SceneManager>()->GetCurrentScene();
		auto& registry = scene->GetRegistry();

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Drag_Entity"))
			{
				RAVEN_ASSERT(payload->DataSize == sizeof(entt::entity*), "Error ImGUI drag entity");
				auto entity = *reinterpret_cast<entt::entity*>(payload->Data);
				auto hierarchyComponent = registry.try_get<Hierarchy>(entity);
				if (hierarchyComponent)
				{
					Hierarchy::Reparent(entity, entt::null, registry, *hierarchyComponent);
				}
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::Indent();

		registry.each([&](auto entity) {
			if (registry.valid(entity))
			{
				auto hierarchyComponent = registry.try_get<Hierarchy>(entity);

				if (!hierarchyComponent|| hierarchyComponent->Parent() == entt::null)
					DrawNode(entity, registry);
			}
		});

		//Only supports one scene
		ImVec2 minSpace = ImGui::GetWindowContentRegionMin();
		ImVec2 maxSpace = ImGui::GetWindowContentRegionMax();

		float yOffset = std::max(45.0f, ImGui::GetScrollY()); 
		minSpace.x += ImGui::GetWindowPos().x + 1.0f;
		minSpace.y += ImGui::GetWindowPos().y + 1.0f + yOffset;
		minSpace.x += ImGui::GetWindowPos().x - 1.0f;
		minSpace.y += ImGui::GetWindowPos().y - 1.0f + ImGui::GetScrollY();
		ImRect bb{ minSpace, maxSpace };

		const ImGuiPayload* payload = ImGui::GetDragDropPayload();
		if (payload != NULL && payload->IsDataType("Drag_Entity"))
		{
			bool acceptable = false;

			RAVEN_ASSERT(payload->DataSize == sizeof(entt::entity*), "Error ImGUI drag entity");
			auto entity = *reinterpret_cast<entt::entity*>(payload->Data);
			auto hierarchyComponent = registry.try_get<Hierarchy>(entity);
			if (hierarchyComponent)
			{
				acceptable = hierarchyComponent->Parent() != entt::null;
			}

			if (acceptable && ImGui::BeginDragDropTargetCustom(bb, ImGui::GetID("Panel Hierarchy")))
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Drag_Entity"))
				{
					RAVEN_ASSERT(payload->DataSize == sizeof(entt::entity*), "Error ImGUI drag entity");
					auto entity = *reinterpret_cast<entt::entity*>(payload->Data);
					auto hierarchyComponent = registry.try_get<Hierarchy>(entity);
					if (hierarchyComponent)
					{
						Transform* tr = registry.try_get<Transform>(entity);
						glm::mat4 worldMtx = tr ? tr->GetWorldMatrix() : glm::mat4(1.0f);

						Hierarchy::Reparent(entity, entt::null, registry, *hierarchyComponent);
						Entity e(entity, scene);
						e.RemoveComponent<Hierarchy>();

						if (tr)
						{
							tr->SetWorldMatrixTransform(worldMtx);
						}
					}
				}
				ImGui::EndDragDropTarget();
			}
		}
	}

	void HierarchyWindow::DrawNode(const entt::entity& node, entt::registry& registry)
	{
		bool show = true;

		if (!registry.valid(node))
			return;

		auto& editor = static_cast<Editor&>(Engine::Get());
		auto scene = editor.GetModule<SceneManager>()->GetCurrentScene();


		const auto nameComponent = registry.try_get<NameComponent>(node);
		std::string name = nameComponent ? nameComponent->name : std::to_string(entt::to_integral(node));

		if (hierarchyFilter.IsActive())
		{
			if (!hierarchyFilter.PassFilter(name.c_str()))
			{
				show = false;
			}
		}

		if (show)
		{
			auto hierarchyComponent = registry.try_get<Hierarchy>(node);
			bool noChildren = true;

			if (hierarchyComponent != nullptr && hierarchyComponent->First() != entt::null)
				noChildren = false;

			ImGuiTreeNodeFlags nodeFlags = ((editor.GetSelected() == node) ? ImGuiTreeNodeFlags_Selected : 0);

			nodeFlags |= ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_FramePadding;

			if (noChildren)
			{
				nodeFlags |= ImGuiTreeNodeFlags_Leaf;
			}

			auto activeComponent = registry.try_get<ActiveComponent>(node);
			bool active = activeComponent ? activeComponent->active : true;

			if (!active)
				ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));

			bool doubleClicked = false;
			if (node == doubleClickedEntity)
			{
				doubleClicked = true;
			}

			if (doubleClicked)
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 1.0f, 2.0f });

			if (recentDroppedEntity == node)
			{
				ImGui::SetNextItemOpen(true);
				recentDroppedEntity = entt::null;
			}

			std::string icon = ICON_MDI_CUBE_OUTLINE;
			auto& iconMap = editor.GetComponentIconMap();

			if (registry.has<Light>(node))
			{
				if (iconMap.find(typeid(Light).hash_code()) != iconMap.end())
					icon = iconMap[typeid(Light).hash_code()];
			}
			else if (registry.has<Camera>(node))
			{
				if (iconMap.find(typeid(Camera).hash_code()) != iconMap.end())
					icon = iconMap[typeid(Camera).hash_code()];
			}



			bool nodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)entt::to_integral(node), nodeFlags, (icon + " %s").c_str(), doubleClicked ? "" : (name).c_str());


			if (doubleClicked)
			{
				ImGui::SameLine();
				static char objName[INPUT_BUFFER];
				strcpy(objName, name.c_str());

				ImGui::PushItemWidth(-1);
				if (ImGui::InputText("##Name", objName, IM_ARRAYSIZE(objName), 0))
					registry.get_or_emplace<NameComponent>(node).name = objName;
				ImGui::PopStyleVar();
			}


			if (!active)
				ImGui::PopStyleColor();

			bool deleteEntity = false;
			if (ImGui::BeginPopupContextItem(name.c_str()))
			{
				if (ImGui::Selectable("Copy"))
					editor.SetCopiedEntity(node);

				if (ImGui::Selectable("Cut"))
					editor.SetCopiedEntity(node, true);

				if (editor.GetCopiedEntity() != entt::null && registry.valid(editor.GetCopiedEntity()))
				{
					if (ImGui::Selectable("Paste"))
					{
						Entity copiedEntity = { editor.GetCopiedEntity(), scene };
						if (!copiedEntity.Valid())
						{
							editor.SetCopiedEntity(entt::null);
						}
						else
						{
							scene->DuplicateEntity(copiedEntity, { node, scene });

							if (editor.IsCutCopyEntity())
								deleteEntity = true;
						}
					}
				}
				else
				{
					ImGui::TextDisabled("Paste");
				}

				ImGui::Separator();

				if (ImGui::Selectable("Duplicate"))
				{
					scene->DuplicateEntity({ node , scene });
				}
				if (ImGui::Selectable("Delete"))
					deleteEntity = true;
				if (editor.GetSelected() == node)
					editor.SetSelected(entt::null);
				ImGui::Separator();
				if (ImGui::Selectable("Rename"))
					doubleClickedEntity = node;
				ImGui::Separator();

				if (ImGui::Selectable("Add Child"))
				{
					auto child = scene->GetEntityManager()->Create();

					child.SetParent({ node, scene });
				}
				ImGui::EndPopup();
			}
			
			draging = false;
			if (!doubleClicked && ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
			{
				auto ptr = node;
				ImGui::SetDragDropPayload("Drag_Entity", &ptr, sizeof(entt::entity*));
				ImGui::Text(ICON_MDI_ARROW_UP);
				ImGui::EndDragDropSource();
				draging = true;
			}

			const ImGuiPayload* payload = ImGui::GetDragDropPayload();
			if (payload != NULL && payload->IsDataType("Drag_Entity"))
			{
				draging = true;
				bool acceptable;
				RAVEN_ASSERT(payload->DataSize == sizeof(entt::entity*), "Error ImGUI drag entity");
				auto entity = *reinterpret_cast<entt::entity*>(payload->Data);
				auto hierarchyComponent = registry.try_get<Hierarchy>(entity);
				if (hierarchyComponent != nullptr)
				{
					acceptable = entity != node && (!IsParentOfEntity(entity, node, registry)) && (hierarchyComponent->Parent() != node);
				}
				else
					acceptable = entity != node;

				if (ImGui::BeginDragDropTarget())
				{
					// Drop directly on to node and append to the end of it's children list.
					if (ImGui::AcceptDragDropPayload("Drag_Entity"))
					{
						if (acceptable)
						{
							Transform* tr = registry.try_get<Transform>(entity);
							glm::mat4 worldMtx = tr ? tr->GetWorldMatrix() : glm::mat4(1.0f);

							if (hierarchyComponent)
								Hierarchy::Reparent(entity, node, registry, *hierarchyComponent);
							else
							{
								registry.emplace<Hierarchy>(entity, node);
							}

							if (tr)
							{
								tr->SetWorldMatrixTransform(worldMtx);
							}

							droppedEntity = node;
						}
					}
					ImGui::EndDragDropTarget();
				}
			}

			if (ImGui::IsMouseReleased(0) && ImGui::IsItemHovered(ImGuiHoveredFlags_None) && !deleteEntity)
				editor.SetSelected(node);
			else if (doubleClickedEntity == node && ImGui::IsMouseClicked(0) && !ImGui::IsItemHovered(ImGuiHoveredFlags_None))
				doubleClickedEntity = entt::null;

			if (ImGui::IsMouseDoubleClicked(0) && ImGui::IsItemHovered(ImGuiHoveredFlags_None))
			{
				doubleClickedEntity = node;
				/*if (Application::Get().GetEditorState() == EditorState::Preview)
				{
					auto transform = registry.try_get<Maths::Transform>(node);
					if (transform)
						editor.FocusCamera(transform->GetWorldPosition(), 2.0f, 2.0f);
				}*/
			}

			if (deleteEntity)
			{
				DestroyEntity(node, registry);
				if (nodeOpen)
					ImGui::TreePop();
				return;
			}

			/*if (m_SelectUp)
			{
				if (editor.GetSelected() == node && registry.valid(m_CurrentPrevious))
				{
					m_SelectUp = false;
					editor.SetSelected(m_CurrentPrevious);
				}
			}

			if (m_SelectDown)
			{
				if (registry.valid(m_CurrentPrevious) && m_CurrentPrevious == editor.GetSelected())
				{
					m_SelectDown = false;
					editor.SetSelected(node);
				}
			}

			m_CurrentPrevious = node;*/

			if (nodeOpen == false)
			{
				return;
			}

			const ImColor TreeLineColor = ImColor(128, 128, 128, 128);
			const float SmallOffsetX = 6.0f;
			ImDrawList* drawList = ImGui::GetWindowDrawList();

			ImVec2 verticalLineStart = ImGui::GetCursorScreenPos();
			verticalLineStart.x += SmallOffsetX; //to nicely line up with the arrow symbol
			ImVec2 verticalLineEnd = verticalLineStart;

			if (!noChildren)
			{
				entt::entity child = hierarchyComponent->First();
				while (child != entt::null && registry.valid(child))
				{
					float HorizontalTreeLineSize = 16.0f; //chosen arbitrarily
					auto currentPos = ImGui::GetCursorScreenPos();
					ImGui::Indent(10.0f);

					auto childHerarchyComponent = registry.try_get<Hierarchy>(child);

					if (childHerarchyComponent)
					{
						entt::entity firstChild = childHerarchyComponent->First();
						if (firstChild != entt::null && registry.valid(firstChild))
						{
							HorizontalTreeLineSize *= 0.5f;
						}
					}
					DrawNode(child, registry);
					ImGui::Unindent(10.0f);

					const ImRect childRect = ImRect(currentPos, currentPos + ImVec2(0.0f, ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y));

					const float midpoint = (childRect.Min.y + childRect.Max.y) / 2.0f;
					drawList->AddLine(ImVec2(verticalLineStart.x, midpoint), ImVec2(verticalLineStart.x + HorizontalTreeLineSize, midpoint), TreeLineColor);
					verticalLineEnd.y = midpoint;

					if (registry.valid(child))
					{
						auto hierarchyComponent = registry.try_get<Hierarchy>(child);
						child = hierarchyComponent ? hierarchyComponent->Next() : entt::null;
					}
				}
			}

			drawList->AddLine(verticalLineStart, verticalLineEnd, TreeLineColor);

			ImGui::TreePop();
		}
	}

	bool HierarchyWindow::IsParentOfEntity(entt::entity entity, entt::entity child, entt::registry& registry)
	{
		auto nodeHierarchyComponent = registry.try_get<Hierarchy>(child);
		if (nodeHierarchyComponent)
		{
			auto parent = nodeHierarchyComponent->Parent();
			while (parent != entt::null)
			{
				if (parent == entity)
				{
					return true;
				}
				else
				{
					nodeHierarchyComponent = registry.try_get<Hierarchy>(parent);
					parent = nodeHierarchyComponent ? nodeHierarchyComponent->Parent() : entt::null;
				}
			}
		}
		return false;
	}

	void HierarchyWindow::DestroyEntity(entt::entity entity, entt::registry& registry)
	{
		auto hierarchyComponent = registry.try_get<Hierarchy>(entity);
		if (hierarchyComponent)
		{
			entt::entity child = hierarchyComponent->First();
			while (child != entt::null)
			{
				auto hierarchyComponent = registry.try_get<Hierarchy>(child);
				auto next = hierarchyComponent ? hierarchyComponent->Next() : entt::null;
				DestroyEntity(child, registry);//
				child = next;
			}
		}
		registry.destroy(entity);
	}

};

