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

#pragma  once
#include <string>
#include <imgui.h>
#include <memory>

#include "EditorWindow.h"
#include <ImGuiEnttEditor.hpp>

namespace Raven 
{
	
	class AnimationController;
	class PropertiesWindow : public EditorWindow
	{
	public:
		PropertiesWindow();
		virtual void OnImGui() override;
		virtual void OnSceneCreated(Scene* scene) override;
		inline void SetController(Ptr<AnimationController> con) { controller = con; }

		static std::string ImGuiDragDropAssetsTarget();

		//
		void OnImGUIGlobalSettings();

	private:
		bool init = false;
		MM::EntityEditor<entt::entity> enttEditor;
		WeakPtr<AnimationController> controller;
	};
};