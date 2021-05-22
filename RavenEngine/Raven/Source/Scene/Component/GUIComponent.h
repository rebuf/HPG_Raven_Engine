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
#include "GUI/GUIModule.h"
#include "GUI/GUILayout.h"
#include "Engine.h"
#include <functional>
#include "Component.h"

namespace Raven
{
	class GUIComponent : public Component
	{
	public:
		GUIComponent();
		void setLayout(GUILayout* _layout);
		void UpdateGUI();

	protected:
		// Reference to the GUIModule
		GUIModule* regi;
		GUILayout* layout;
	};
};