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


#include "Light.h"
#include <imgui.h>
#include "ImGui/ImGuiHelpers.h"

namespace Raven
{

	std::string LightTypeToString(LightType type)
	{
		switch (type)
		{
		case LightType::DirectionalLight:
			return "Directional Light";
		case LightType::SpotLight:
			return "Spot Light";
		case LightType::PointLight:
			return "Point Light";
		default:
			return "ERROR";
		}
	}

	int32_t StringToLightType(const std::string& type)
	{
		if (type == "Directional")
			return int32_t(LightType::DirectionalLight);

		if (type == "Point")
			return int32_t(LightType::PointLight);

		if (type == "Spot")
			return int32_t(LightType::SpotLight);

		return 0.0f;
	}


	Light::Light(const glm::vec3& direction, const glm::vec4& color, float intensity, const LightType& type, const glm::vec3& position, float radius, float angle)
		: direction(direction)
		, color(color)
		, position(position)
		, intensity(intensity)
		, radius(radius)
		, type(int32_t(type))
		, outerAngle(angle)
		, clipDistance(750.0f)
	{

	}
	
	void Light::OnImGui()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
		ImGui::Columns(2);
		ImGui::Separator();

		//if (type != 0)
		//	ImGuiHelper::Property("Position", position);

		if (type != 2)
			ImGuiHelper::Property("Direction", direction);

		if (type != 0)
			ImGuiHelper::Property("Radius", radius, 0.0f, 100.0f);

		ImGuiHelper::Property("Color", color, true, ImGuiHelper::PropertyFlag::ColorProperty);
		ImGuiHelper::Property("Intensity", intensity, 0.0f, 100.0f);

		if (type == 1)
			ImGuiHelper::Property("Inner Angle", innerAngle, 0.0f, 1.0f);

		if (type == 1)
			ImGuiHelper::Property("Outer Angle", outerAngle, 0.0f, 1.0f);

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted("Light Type");
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);
	


		const char* lights[] = {"Directional Light","Spot Light","Point Light" };
		auto currLight = LightTypeToString(LightType(int32_t(type)));

		if (ImGui::BeginCombo("LightType", currLight.c_str(), 0))
		{
			for (auto n = 0; n < 3; n++)
			{
				if (ImGui::Selectable(lights[n]))
				{
					type = n;
				}
	
			}
			ImGui::EndCombo();
		}


/*
		if (ImGui::BeginMenu(LightTypeToString(LightType(int32_t(type))).c_str()))
		{
			if (ImGui::MenuItem("Directional Light", "", static_cast<int>(type) == 0, true))
			{
				type = int32_t(LightType::DirectionalLight);
			}
			if (ImGui::MenuItem("Spot Light", "", static_cast<int>(type) == 1, true))
			{
				type = int32_t(LightType::SpotLight);
			}
			if (ImGui::MenuItem("Point Light", "", static_cast<int>(type) == 2, true))
			{
				type = int32_t(LightType::PointLight);
			}
			ImGui::EndMenu();
		}
*/


		ImGui::PopItemWidth();
		ImGui::NextColumn();

		ImGui::Columns(1);
		ImGui::Separator();
		ImGui::PopStyleVar();
	}
};