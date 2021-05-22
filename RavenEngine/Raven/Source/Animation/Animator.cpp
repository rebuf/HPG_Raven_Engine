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
#include "Animator.h"
#include "Utilities/StringUtils.h"
#include "Engine.h"
#include "Scene/SceneManager.h"
#include "ResourceManager/ResourceManager.h"

#include "Animation.h"
#include "AnimationController.h"

#include "ImGui/ImGuiHelpers.h"




namespace Raven
{
	void Animator::OnImGui()
	{
		std::string animctrlAsset = controllerInstance ? controllerInstance->GetParentController()->GetResourcePath() : "";
		ImGuiHelper::Property("Animation Controller", animctrlAsset);

		if (ImGui::BeginDragDropTarget())
		{
			auto data = ImGui::AcceptDragDropPayload("AssetFile");
			if (data)
			{
				std::string file = (char*)data->Data;

				if (Engine::GetModule<ResourceManager>()->GetResourceType(file) == RT_AnimationController)
				{
					auto controller = Engine::GetModule<ResourceManager>()->GetResource<AnimationController>(file);
					controllerInstance = Ptr<AnimationControllerInstance>(new AnimationControllerInstance(controller));
				}
			}

			ImGui::EndDragDropTarget();
		}

		ImGuiHelper::Property("Apply Root Motion", rootMotion);
	}


	void Animator::SetWrapMode(const std::string& name, int32_t mode)
	{
		if (GetController()) {
			GetController()->Get()->SetWrapMode(name, static_cast<AnimationWrapMode>(mode));
		}
	}

};