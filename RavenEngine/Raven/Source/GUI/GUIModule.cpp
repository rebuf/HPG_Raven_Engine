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
#include "GUIModule.h"
#include "ResourceManager/ResourceManager.h"
#include "ResourceManager/Resources/Texture2D.h"

#include "Render/RenderResource/RenderRscTexture.h"
#include "Render/OpenGL/GLTexture.h"

namespace Raven {

	GUIModule::GUIModule()
	{

	}

	GUIModule::~GUIModule()
	{
	}

	/** Initialise the module, inluding ImGui
	*	SetWindow must be called before the initialisation will work
	*/
	void GUIModule::Initialize()
	{
		// Get a list of all the fonts loaded by the GUI
		ImGuiIO& io = ImGui::GetIO();
		loadedFonts.push_back(io.Fonts->AddFontDefault());
	}

	/** Responsible for cleaning up the ImGui context and any other memory used by the module
	*	Must be called before destruction of the glfwWindow();
	*/
	void GUIModule::Destroy()
	{
	}

	size_t GUIModule::loadFont(const char* file_name, int pixel_size) {
		ImGuiIO& io = ImGui::GetIO();
		loadedFonts.push_back(io.Fonts->AddFontFromFileTTF(file_name, pixel_size));
		return loadedFonts.size() - 1;
	}

	ImFont* GUIModule::getFont(size_t font_i) {
		// Could handle errors here
		if (loadedFonts.size() <= font_i) return NULL;

		return loadedFonts[font_i];
	}
	
	//Functionality is in ImGui module
	
	/** Frame Begining Function
	*   Should be called at the begining of every frame before any widget specification code
	*/
	void GUIModule::BeginFrame()
	{
		panelCount = 0;
	}
    
	/** Frame Ending Function
	*   Should be called after all widget specification functions are called, before displaying to screen
	*/
	void GUIModule::EndFrame()
	{

	}
    


	void GUIModule::BeginPanel(GUIWidgetConfig config)
	{
		int winW, winH;
		glfwGetWindowSize(Engine::GetModule<Raven::Window>()->GetNativeWindow(), &winW, &winH);

		auto pixSize = PercentToPix(config.size, ImVec2(winW, winH));
		auto pixPadding = PercentToPix(config.padding, pixSize);
		auto pixPos = PercentToPix(config.position, ImVec2(winW, winH));

		// Style the Window
		// Using push/pop style will allow us to not affect the style in the rest of the programs ImGui usage
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, config.rounding);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, pixPadding);
		ImGui::PushStyleColor(ImGuiCol_Border, config.border_col);
		ImGui::PushStyleColor(ImGuiCol_WindowBg, config.bg_col);

		ImGui::SetNextWindowPos(pixPos);
		ImGui::SetNextWindowSize(pixSize);

		char panelName[33];
		char* panelText = "Panel ";
		sprintf(panelName, "%s %d", panelText, panelCount++);

		// Define the window
		ImGui::Begin(panelName, NULL, DefaultFlags);

	}

	void GUIModule::EndPanel()
	{
		ImGui::End();

		// Revert the panel styles
		ImGui::PopStyleColor(2);
		ImGui::PopStyleVar(2);
	}

	void GUIModule::Text(GUIWidgetConfig config, size_t fontID, const char* fmt, ...)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, config.text_col);
		ImGui::PushFont(getFont(fontID));

		// Horizontal Text Alignment Logic
		if (config.text_align.x != 0) {
			ImGui::Spacing();
			// Calculate the position of the right alligned button
			float b_width = ImGui::CalcTextSize(fmt).x;
			ImGui::SameLine(config.text_align.x * (ImGui::GetWindowWidth() - b_width));
		}

		va_list args;
		va_start(args, fmt);
		// Pass the format string and the args to the va_list compatible text
		ImGui::TextV(fmt, args);
		va_end(args);

		ImGui::PopFont();
		ImGui::PopStyleColor();
	}

	bool GUIModule::Button(GUIWidgetConfig config, size_t fontID, const char* text)
	{
		auto pixSize = PercentToPix(config.size, ImGui::GetWindowSize());
		auto pixPos = PercentToPix(config.position, ImGui::GetWindowSize());

		// Configure the button
		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, config.text_align);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, config.rounding);
		ImGui::PushStyleColor(ImGuiCol_Button, config.bg_col);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, config.hover_col);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, config.active_col);
		ImGui::PushStyleColor(ImGuiCol_Text, config.text_col);

		// Configure the button text
		ImGui::PushFont(getFont(fontID));

		ImGui::SetCursorPos(pixPos);

		bool ret = ImGui::Button(text, pixSize);

		ImGui::PopFont();
		ImGui::PopStyleColor(4);
		ImGui::PopStyleVar(2);

		return ret;
	}

	void GUIModule::Image(GUIWidgetConfig config, Ptr<Texture2D> img)
	{
		if (img == NULL)
		{
			LOGC("No image loaded");
			return;
		}

		auto pixPos = PercentToPix(config.position, ImGui::GetWindowSize());
		auto pixSize = PercentToPix(config.size, ImGui::GetWindowSize());

		ImGui::SetCursorPos(pixPos);

		// TODO: Refactor this code when Ammar has fully implemented
		// the textures

		auto imgPointer = img->GetRenderRsc()->GetTexture()->GetID();
		auto imgW = img->GetRenderRsc()->GetTexture()->GetWidth();
		auto imgH = img->GetRenderRsc()->GetTexture()->GetHeight();
		ImGui::Image((void*)(intptr_t)imgPointer, pixSize);
	}

	ImDrawData* GUIModule::GetDrawData() {
		return ImGui::GetDrawData();
	}
}