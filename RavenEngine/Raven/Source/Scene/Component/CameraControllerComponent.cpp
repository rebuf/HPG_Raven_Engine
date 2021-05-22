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
#include "CameraControllerComponent.h"
#include "Core/CameraController.h"
#include "Core/EditorCamera.h"
#include "Logger/Console.h"

namespace Raven 
{
	CameraControllerComponent::ControllerType CameraControllerComponent::StringToType(const std::string& type)
	{
		if (type == "FPS")
			return ControllerType::FPS;
		if (type == "Editor")
			return ControllerType::EditorCamera;
		return ControllerType::Custom;
	}

	CameraControllerComponent::CameraControllerComponent(ControllerType type)
	{
		SetControllerType(type);
	}

	void CameraControllerComponent::SetControllerType(CameraControllerComponent::ControllerType type)
	{
		this->type = type;
		switch (type)
		{
		case ControllerType::FPS:
			LOGW("{0} does not implement", __FUNCTION__);
			break;
		case ControllerType::EditorCamera:
			cameraController = std::make_shared<EditorCameraController>();
			break;
		}
	}

	std::string CameraControllerComponent::TypeToString(ControllerType type)
	{
		switch (type)
		{
		case ControllerType::FPS:
			return "FPS";
		case ControllerType::EditorCamera:
			return "Editor";
		}
		return "Custom";
	}

};

