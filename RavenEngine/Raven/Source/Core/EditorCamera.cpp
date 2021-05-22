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
#include "EditorCamera.h"
#include "Camera.h"
#include "Devices/Input.h"
#include "Math/MathUtils.h"


namespace Raven
{
	EditorCameraController::EditorCameraController()
	{
		focalPoint = Raven::ZERO;
		velocity = glm::vec3(0.0f);
		mouseSensitivity = 0.00001f;
		zoomDampeningFactor = 0.00001f;
		dampeningFactor = 0.00001f;
		rotateDampeningFactor = 0.0000001f;

		pitch = 0.0f;
		yaw = 0.0f;
	}

	EditorCameraController::~EditorCameraController()
	{
	}

	void EditorCameraController::HandleMouse(Transform& transform, float dt, float xpos, float ypos)
	{
        
		if (Input::GetInput()->IsMouseHeld(KeyCode::MouseKey::ButtonRight))
		{
			mouseSensitivity = 2.5f * dt;
			rotateVelocity = rotateVelocity + (glm::vec2{ xpos,ypos } - previousCurserPos) * mouseSensitivity;
		}
  
				
		pitch -= rotateVelocity.y;
		yaw -= rotateVelocity.x;
			
		pitch = std::min(pitch, 89.0f);
		pitch = std::max(pitch, -89.0f);

		previousCurserPos = glm::vec2(xpos, ypos);

		transform.SetRotationEuler(glm::radians(pitch), glm::radians(yaw), 0.0f);

		rotateVelocity = rotateVelocity * std::pow(rotateDampeningFactor, dt);

		UpdateScroll(transform, Input::GetInput()->GetScrollOffset(), dt);
	}


	void EditorCameraController::HandleKeyboard(Transform& transform, float dt)
	{

		float multiplier = 100.0f;

		if (Input::GetInput()->IsKeyHeld(KeyCode::Id::LeftShift))
		{
			multiplier = 1500.0f;
		}

		cameraSpeed = multiplier * dt;

		if (Input::GetInput()->IsMouseHeld(KeyCode::MouseKey::ButtonRight))
		{
			if (Input::GetInput()->IsKeyHeld(KeyCode::Id::W))
			{
				velocity -= transform.GetForwardDirection() * cameraSpeed;
			}

			if (Input::GetInput()->IsKeyHeld(KeyCode::Id::S))
			{
				velocity += transform.GetForwardDirection() * cameraSpeed;
			}

			if (Input::GetInput()->IsKeyHeld(KeyCode::Id::A))
			{
				velocity -= transform.GetRightDirection() * cameraSpeed;
			}

			if (Input::GetInput()->IsKeyHeld(KeyCode::Id::D))
			{
				velocity += transform.GetRightDirection() * cameraSpeed;
			}

			if (Input::GetInput()->IsKeyHeld(KeyCode::Id::Q))
			{
				velocity -= transform.GetUpDirection() * cameraSpeed;
			}

			if (Input::GetInput()->IsKeyHeld(KeyCode::Id::E))
			{
				velocity += transform.GetUpDirection() * cameraSpeed;
			}
		}

		if (!MathUtils::Equals(velocity, Raven::ZERO))
		{
			glm::vec3 position = transform.GetPosition();
			position += velocity * dt;
			transform.SetPosition(position);
			velocity = velocity * std::pow(dampeningFactor, dt);
		}
	}

	void EditorCameraController::UpdateScroll(Transform& transform, float offset, float dt)
	{
		if(offset != 0.0f)
		{
			zoomVelocity -= dt * offset * 10.0f;
		}

		if(!MathUtils::Equals(zoomVelocity, 0.0f))
		{
			glm::vec3 pos = transform.GetPosition();
			pos += transform.GetForwardDirection() * zoomVelocity;
			zoomVelocity = zoomVelocity * std::pow(zoomDampeningFactor, dt);

			transform.SetPosition(pos);
		}
	}
}
