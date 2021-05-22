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
#pragma once
#include "CameraController.h"

namespace Raven
{
	class EditorCameraController : public CameraController
	{
	public:
		EditorCameraController();
		~EditorCameraController();

		void HandleMouse(Transform& transform, float dt, float xpos, float ypos) override;
		void HandleKeyboard(Transform& transform, float dt) override;
		void UpdateScroll(Transform& transform, float offset, float dt) override;


	private:
		float pitch;
		float yaw;

	};
}
