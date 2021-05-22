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
#include "SoundComponent.h"
#include "Audio/AudioSource.h"
#include <glm/gtc/type_ptr.hpp>
#include "ImGui/ImGuiHelpers.h"

namespace Raven 
{
	SoundComponent::SoundComponent()
	{
		soundNode.reset(AudioSource::Create());
	}

	SoundComponent::~SoundComponent()
	{
	
	}

	void SoundComponent::OnImGui()
	{
		auto loop = soundNode->GetLooping();
		auto paused = soundNode->GetPaused();
		auto pitch =  soundNode->GetPitch();

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
		ImGui::Columns(2);
		ImGui::Separator();



//un-implemented, because of implementation the space audio is difficult.
//no time to do that.

		if (ImGuiHelper::Property("Loop", loop))
		{
			soundNode->SetLooping(loop);
		}

		if (ImGuiHelper::Property("Paused", paused))
		{
			soundNode->SetPaused(paused);
		}

		std::string path = "";
		

		ImGui::Columns(1);
		ImGui::Separator();
		ImGui::PopStyleVar();


	}
};


