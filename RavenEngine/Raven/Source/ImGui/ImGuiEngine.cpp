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

#include "ImGuiEngine.h"
#include "Engine.h"
#include "Window/Window.h"
#include <IconsMaterialDesignIcons.h>
#include <MaterialDesign.inl>
#include <imgui.h>
#include <ImGuizmo.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>


namespace Raven 
{
	void ImGuiEngine::Initialize()
	{
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		ImGui::StyleColorsDark();

		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		ImGuiStyle& style = ImGui::GetStyle();

		//auto scale = Engine::Get().GetModule<Window>()->GetWindowScale();

		//style.ScaleAllSizes(std::max(scale.first, scale.second));
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}
		ImFontConfig config;
		//config.SizePixels = 13.f * std::max(scale.first, scale.second);

		io.Fonts->AddFontDefault(&config);
		AddIcon();
		ImGui_ImplGlfw_InitForOpenGL(Engine::Get().GetModule<Window>()->GetNativeWindow(), true);
		ImGui_ImplOpenGL3_Init("#version 330");
	}

	void ImGuiEngine::Destroy()
	{

	}

	void ImGuiEngine::Render()
	{
	
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void ImGuiEngine::Prepare()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
	}

	void ImGuiEngine::AddIcon()
	{
		ImGuiIO& io = ImGui::GetIO();
		auto scale = Engine::Get().GetModule<Window>()->GetWindowScale();
		static const ImWchar icons_ranges[] = { ICON_MIN_MDI, ICON_MAX_MDI, 0 };
		ImFontConfig icons_config;
		// merge in icons from Font Awesome
		icons_config.MergeMode = true;
		icons_config.PixelSnapH = true;
		icons_config.GlyphOffset.y = 1.0f;
		icons_config.OversampleH = icons_config.OversampleV = 1;
		icons_config.PixelSnapH = true;
		icons_config.SizePixels = 18.f;

		io.Fonts->AddFontFromMemoryCompressedTTF(MaterialDesign_compressed_data, MaterialDesign_compressed_size, 16, &icons_config, icons_ranges);
	}

};
