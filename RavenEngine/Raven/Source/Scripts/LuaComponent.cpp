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
#include "IconsMaterialDesignIcons.h"
#include "LuaComponent.h"
#include "LuaVirtualMachine.h"
#include "Engine.h"
#include "Utilities/StringUtils.h"
#include "ImGui/ImGuiHelpers.h"
#include "Scene/Entity/Entity.h"
#include "Scene/Component/MeshComponent.h"
#include "Scene/Component/SkinnedMeshComponent.h"
#include "Scene/Component/Light.h"
#include "Core/Camera.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <fstream>
#include <filesystem>

#include "Logger/Console.h"

#include "Scene/Component/Transform.h"
#include "Scene/Component/Component.h"
#include "Animation/Animator.h"

namespace Raven 
{
	LuaComponent::LuaComponent( const std::string& file, Scene* initScene)
		:scene(initScene),file(file)
	{
		table = nullptr;

		Init();
	}


	void LuaComponent::SaveNewFile(const std::string& clazzName)
	{
		std::string newPath = "scripts/";

		std::string defaultScript =
R"(

#name = {}

function #name:OnInit()
end

function #name:OnUpdate(dt)
end

return #name

)";

		StringUtils::Replace(defaultScript, "#name", clazzName);

		std::ofstream out(newPath + clazzName + ".lua");
		out << defaultScript;
		out.flush();
		out.close();
		className = clazzName;

		SetFilePath(newPath + clazzName + ".lua");
		Reload();
	}


	void LuaComponent::SetFilePath(const std::string& fileName)
	{
		file = fileName;
		className = StringUtils::RemoveExtension(StringUtils::GetFileName(file));
	}


	void LuaComponent::Init()
	{
		className = StringUtils::RemoveExtension(StringUtils::GetFileName(file));
		auto vm = Engine::GetModule<LuaVirtualMachine>();
		LoadScript();
	}

	LuaComponent::LuaComponent()
	{

	}

	LuaComponent::~LuaComponent()
	{
	
	}

	void LuaComponent::OnInit()
	{
		if (table) 
		{
			(*table)["entity"] = GetEntity();
		}

		//load prev values
		if (onInitFunc && onInitFunc->isFunction())
		{
			try
			{
				(*onInitFunc)(*table);
			}
			catch (const std::exception& e)
			{
				LOGE("{0}",e.what());
			}
			metaFile.Load(this, file + ".meta", scene);
		}
	}

	void LuaComponent::OnUpdate(float dt)
	{
		if (onUpdateFunc && onUpdateFunc->isFunction())
		{
			try
			{
				(*onUpdateFunc)(*table, dt);
			}
			catch (const std::exception& e) 
			{
				LOGE("{0}", e.what());
			}
		}
	}

	void LuaComponent::Reload()
	{	
		LoadScript();
	}

	void LuaComponent::LoadScript()
	{
		auto vm = Engine::GetModule<LuaVirtualMachine>();
		luaL_dofile(vm->GetState(), file.c_str());
		try
		{
			table = std::make_shared<luabridge::LuaRef>(luabridge::LuaRef::fromStack(vm->GetState()));
			
			table = std::make_shared<luabridge::LuaRef>((*table)["new"]());
			
			onInitFunc = std::make_shared<luabridge::LuaRef>((*table)["OnInit"]);
			onUpdateFunc = std::make_shared<luabridge::LuaRef>((*table)["OnUpdate"]);
			(*table)["entity"] = GetEntity();
		}
		catch (const std::exception& e)
		{
			LOGE("{0}", e.what());
		}
		//OnInit();
	}

	void LuaComponent::OnImGui()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
		ImGui::Columns(2);
		ImGui::Separator();
		static char modelName[255] = {};
		if (table && table->isTable()) 
		{
			for (auto&& pair : luabridge::pairs(*table))
			{
				auto name = pair.first.tostring();

				if (name == "__cname" || name == "__index") {
					continue;
				}

				if (pair.second.isNumber()) 
				{
					float number = pair.second;
					if (ImGuiHelper::Property(pair.first.tostring(), number))
					{
						(*table)[pair.first.tostring()] = number;
					}
				}
				else if (pair.second.isString()) 
				{
					std::string str = pair.second;
					if (ImGuiHelper::Property(pair.first.tostring(), str))
					{
						(*table)[pair.first.tostring()] = str;
					}
				}
				else if (pair.second.isFunction())
				{
					ImGui::Columns(1);
					ImGui::TextUnformatted(name.c_str());
				}
				else if (pair.second.isBool())
				{
					bool value = pair.second;
					if (ImGuiHelper::Property(pair.first.tostring(), value))
					{
						(*table)[pair.first.tostring()] = value;
					}
				}
				else if (pair.second.isTable()) 
				{
					ImGui::TextUnformatted(name.c_str());
					ImGui::NextColumn();
					ImGui::PushItemWidth(-1);

					std::string id =  name + " not support now";

					try
					{
						std::string cname = pair.second["__cname"];
						memcpy(modelName, cname.c_str(), cname.size() + 1);
					}
					catch (...)
					{
						
					}
					
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(ImColor(200, 200, 200)));
					ImGui::InputText(id.c_str(), modelName, 255, ImGuiInputTextFlags_ReadOnly);
					ImGui::PopStyleColor();

					ImGui::PopItemWidth();
					ImGui::NextColumn();

				}
				else if (pair.second.isUserdata())
				{
					if (pair.second.isInstance<glm::vec2>()) 
					{
						glm::vec2* v = pair.second;

						if (ImGuiHelper::Property(pair.first.tostring(), *v))
						{
						}
					}
					else if (pair.second.isInstance<glm::vec3>())
					{
						glm::vec3* v = pair.second;

						if (ImGuiHelper::Property(pair.first.tostring(), *v))
						{
						}
					}
					else if (pair.second.isInstance<glm::vec4>())
					{
						glm::vec4* v = pair.second;

						if (ImGuiHelper::Property(pair.first.tostring(), *v))
						{
						}
					}
					else if ((pair.second.isInstance<Entity>() && name != "entity"))
					{
						Entity* v = pair.second;
						if (v != nullptr && v->GetHandle() != entt::null)
						{
							auto icon = 
								v->HasComponent<Camera>() ?
								ICON_MDI_CAMERA : 
								v->HasComponent<MeshComponent>() ?
								ICON_MDI_SQUARE : 
								v->HasComponent<SkinnedMeshComponent>() ?
								ICON_MDI_SQUARE :
								v->HasComponent<Light>() ?
								ICON_MDI_LIGHTBULB :
								v->HasComponent<LuaComponent>() ?
								ICON_MDI_SCRIPT : ICON_MDI_CUBE
								;

							auto entityName = icon + v->GetComponent<NameComponent>().name;
							memcpy(modelName, entityName.c_str(), entityName.length() + 1);
						}

			
						ImGui::TextUnformatted(name.c_str());
						ImGui::NextColumn();
						ImGui::PushItemWidth(-1);

						std::string id = "##" + name;
			
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(ImColor(200, 200, 200)));
						ImGui::InputText(id.c_str(), modelName, 255, ImGuiInputTextFlags_ReadOnly);
						ImGui::PopStyleColor();

						ImGui::PopItemWidth();
						ImGui::NextColumn();
					
						const ImGuiPayload* payload = ImGui::GetDragDropPayload();
						if (payload != NULL && payload->IsDataType("Drag_Entity"))
						{
							auto& entity = *reinterpret_cast<entt::entity*>(payload->Data);

							if (ImGui::BeginDragDropTarget())
							{
								if (ImGui::AcceptDragDropPayload("Drag_Entity"))
								{
									v->SetHandle(entity);
									v->SetScene(Engine::Get().GetModule<SceneManager>()->GetCurrentScene());
								}
								ImGui::EndDragDropTarget();
							}
						}
					}
				}
			}
		}

		ImGui::Columns(1);
		ImGui::Separator();
		ImGui::PopStyleVar();

	}

	bool LuaComponent::IsLoaded()
	{
		return table != nullptr && !table->isNil();
	}

};