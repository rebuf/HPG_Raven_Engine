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
#include "AnimationController.h"
#include "Logger/Console.h"
#include "Utilities/StringUtils.h"
#include "cereal/archives/json.hpp"
#include <fstream>
#include <imgui.h>
#include <IconsMaterialDesignIcons.h>
#include "Animation.h"
#include "Logger/Console.h"

#include "Engine.h"
#include "ResourceManager/ResourceManager.h"
#include "Scene/Component/SkinnedMeshComponent.h"


namespace Raven
{
	AnimationController::AnimationController()
	{
		type = AnimationController::StaticGetType();
		iterId = linkInfo.begin();
	}

	AnimationController::AnimationController(const AnimationController& other)
		: focusedLink(nullptr)
		, animatorNodes(other.animatorNodes)
		, linkInfo(other.linkInfo)
		, conditions(other.conditions)
		, currentNodeId(other.currentNodeId)
		, currentLink(0)
	{
		type = AnimationController::StaticGetType();
		iterId = linkInfo.begin();
	}

	const std::string AnimationController::GetCurrentAnimationName() const
	{
		if (auto anim = animatorNodes.find(currentNodeId); anim != animatorNodes.end()) {
			return anim->second.name;
		}
		return "";
	}

 
//could have bug.

	void AnimationController::SetWrapMode(const std::string& name, AnimationWrapMode mode)
	{
		for (auto & node : animatorNodes)
		{

			auto animName = StringUtils::GetFileNameWithoutExtension(node.second.name);
			if (animName == name)
			{
				Ptr<AnimationClip> clip = node.second.animClip.FindOrLoad<AnimationClip>();
				clip->wrapMode = mode;
				break;
			}
		}
	}

	void AnimationController::RemoveCondition(const std::string& key)
	{
		conditions.erase(key);
	}

	void AnimationController::ChangeConditionName(const std::string& old, const std::string& newName)
	{
		conditions[newName] = conditions[old];
		conditions.erase(old);
	}

	void AnimationController::SetValue(const std::string& name, const std::string& value)
	{
		if(conditions.count(name) > 0)
			conditions[name].value = value;
	}

	void AnimationController::OnImGui()
	{
		if (focusedLink != nullptr)
		{
			if (focusedLink->from == EntryNodeId)
			{
				ImGui::TextUnformatted("Default Entry transitions are not previewable");
			}
		}

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
		ImGui::Columns(2);
		ImGui::Separator();

		if (focusedLink != nullptr) 
		{
			if (focusedLink->from != EntryNodeId)
			{
				ImGui::Text("%s ->",
					StringUtils::GetFileNameWithoutExtension(animatorNodes[focusedLink->from].name)
					.c_str()
				);
				ImGui::NextColumn();
				ImGui::TextUnformatted(
					StringUtils::GetFileNameWithoutExtension(animatorNodes[focusedLink->to].name)
					.c_str());
			}
		}

		ImGui::Columns(1);
		ImGui::Separator();
		ImGui::PopStyleVar();

		if (focusedLink != nullptr)
		{
			ImGui::BeginChild("Conditions_Child");

			if (ImGui::CollapsingHeader("Conditions", ImGuiTreeNodeFlags_DefaultOpen)) {
				if (focusedLink != nullptr && focusedLink->from != 101 && conditions.size() > 0)
				{
					ImGui::Columns(3);
					ImGui::Separator();

					static std::string lastClick = "";

					for (auto& c : focusedLink->conditions)
					{
						if (ImGui::Selectable(c.first.c_str())) {
							ImGui::OpenPopup("Choose Condition");
							lastClick = c.first;
						}

						ImGui::NextColumn();


						switch (c.second.type)
						{
						case Condition::Type::Bool:
						{
							static bool active = false;
							active = c.second;
							if (ImGui::Checkbox(("##" + c.first).c_str(), &active))
							{
								c.second.value = std::to_string(active);
							}
						}

						break;
						case Condition::Type::Float:
						{
							static float value = false;
							value = c.second;
							if (ImGui::InputFloat(("##" + c.first).c_str(), &value))
							{
								c.second.value = std::to_string(value);
							}
						}

						break;
						case Condition::Type::Int:
						{
							static int32_t value2 = false;
							value2 = c.second;
							if (ImGui::InputInt(("##" + c.first).c_str(), &value2))
							{
								c.second.value = std::to_string(value2);
							}
						}
						break;
						}
						ImGui::NextColumn();

						ImGui::PushID(("-" + c.first).c_str());
						if (ImGui::Button("del-" ICON_MDI_DELETE))
						{
							focusedLink->conditions.erase(c.first);
							ImGui::PopID();
							break;
						}
						ImGui::PopID();

						ImGui::NextColumn();
					}

					if (ImGui::BeginPopup("Choose Condition")) {
						for (auto& name : conditions) {
							ImGui::PushID(&name);
							if (ImGui::Selectable(name.first.c_str())) {
								focusedLink->conditions[name.first] = name.second;
								focusedLink->conditions.erase(lastClick);
								lastClick = "";
								ImGui::PopID();
								break;
							}
							ImGui::PopID();
						}
						ImGui::EndPopup();
					}

					ImGui::Columns(1);


					if (ImGui::Button("+"))
					{
						auto& beg = conditions.begin();
						focusedLink->conditions[beg->first] = beg->second;
					}

				}
			}

			ImGui::EndChild();
		}
	}

	void AnimationController::AddCondition(Condition::Type type)
	{
		auto& condition = conditions["Conditions_" + std::to_string(conditions.size())];
		condition.type = type;
		condition.value = "0";
	}


	void AnimationController::Connect(const ResourceRef& fromAnimClip,
		int32_t fromId, int32_t fromIn, int32_t fromOut,
		const ResourceRef& toAnimClip, int32_t toId, int32_t toIn, int32_t toOut, int32_t linkId)
	{
		auto& first = animatorNodes[fromId];
		first.name = fromAnimClip.GetPath();
		first.animClip = fromAnimClip;
		first.nodeId = fromId;
		first.in = fromIn;
		first.out = fromOut;

		auto& second = animatorNodes[toId];
		second.name = toAnimClip.GetPath();
		second.animClip = toAnimClip;
		second.nodeId = toId;

		second.in =  toIn;
		second.out = toOut;

		auto& link = linkInfo[linkId];
		link.from = fromId;
		link.to = toId;
		link.id = linkId;
	}

	void AnimationController::RemoveLink(int32_t link)
	{
		linkInfo.erase(link);
	}


	void AnimationController::OnUpdate(float dt, SkinnedMeshComponent* skinnedComp)
	{
		if (currentAnimation)
		{
			if (currentAnimation->GetClipCount() > 0)
			{
				if (!currentAnimation->IsStarted())
				{
					currentAnimation->Play(0, skinnedComp->GetSkeleton());
				}
				currentAnimation->OnUpdate(dt);
			}
		}
		else
		{
			LoadAnimation();
		}

		
		if (iterId != linkInfo.end()) 
		{
			if (iterId->second.from == currentNodeId) {

				int32_t passCount = iterId->second.conditions.size();

				for (auto& condition : iterId->second.conditions)
				{
					if (condition.second != conditions[condition.first])
					{
						passCount--;
						break;
					}
				}
				
				//pass all conditions.. executue next animation
				if (passCount != 0 && passCount == iterId->second.conditions.size()) 
				{
					currentNodeId = iterId->second.to;
					LoadAnimation();
					currentLink = iterId->second.id;
				}
			}
			iterId++;
		}
		else 
		{
			iterId = linkInfo.begin();
		}
	}

	void AnimationController::LoadAnimation()
	{
		Ptr<AnimationClip> clip = animatorNodes[currentNodeId].animClip.FindOrLoad<AnimationClip>();
		currentAnimation = Ptr<Animation>(new Animation());
		currentAnimation->AddClip(clip);
	}

	//----- --- ------ - -- - -- -- - ----- - -- -- -- - -- --  -- - -- -- --- - --- 


	AnimationControllerInstance::AnimationControllerInstance(Ptr<AnimationController> parentController)
	{
		controller = parentController;
		instance = Ptr<AnimationController>(new AnimationController(*controller)); // Copy.
	}

	void AnimationControllerInstance::OnUpdate(float dt, SkinnedMeshComponent* skinnedComp)
	{
		instance->OnUpdate(dt, skinnedComp);
	}


};