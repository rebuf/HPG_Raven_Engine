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

#pragma once
#include <glm/glm.hpp>
#include "Scene/Component/Transform.h"

namespace Raven
{
    class Camera;

	class CameraController
	{
	public:
        CameraController() = default;
		virtual ~CameraController() = default;

		virtual void HandleMouse(Transform& transform, float dt, float xpos, float ypos) {};
		virtual void HandleKeyboard(Transform& transform, float dt){};
		virtual void UpdateScroll(Transform& transform, float offset, float dt){};

		virtual void OnImGui() {};

		inline void SetMouseSensitivity(float value) { mouseSensitivity = value; }
		inline auto & GetVelocity() const { return velocity; }

	protected:
    
		glm::vec3 velocity = {0,0,0};
		glm::vec2 rotateVelocity = { 0,0 };
		glm::vec3 focalPoint = {0,0,0};

		float zoomVelocity = 0.0f;
		float cameraSpeed = 0.0f;
		float distance = 0.0f;
		float zoom = 1.0f;

		glm::vec2 projectionOffset = glm::vec2(0.0f, 0.0f);
		glm::vec2 previousCurserPos = glm::vec2(0.0f, 0.0f);

		float mouseSensitivity = 0.1f;
		float zoomDampeningFactor = 0.00001f;
		float dampeningFactor = 0.00001f;
		float rotateDampeningFactor = 0.001f;
	};

}
