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


#include "IModule.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "Logger/Console.h"


#include <condition_variable>
#include <GLFW/glfw3.h>
#include "Engine.h"
#include "Window/Window.h"

#include "Utilities/Serialization.h"

#include <string>
#include <vector>

#include <ResourceManager/Resources/Texture2D.h>

namespace Raven {

	class GUIModule : public IModule
	{
	public:
		enum EWidgetType {
			WT_None = 0,
			WT_Panel = 1,
			WT_PanelEnd = 2, // Special widget type that signifies the end of a panel
			WT_Text = 3,
			WT_Button = 4,
			WT_Image = 5
		};

		// Widget Configuration Struct
		struct GUIWidgetConfig
		{
			EWidgetType type = WT_None;

			// General Configuration variables
			ImVec4 bg_col = ImVec4(0., 0., 0., 0.);
			ImVec4 border_col = ImVec4(0., 0., 0., 0.);
			ImVec2 padding = ImVec2(0., 0.); // Internal padding of panel given in percentage of glfw window
			ImVec2 position = ImVec2(0., 0.); // Position in percentage relative to top left of parent
			ImVec2 size = ImVec2(0., 0.); // Size in percent of parent
										  // 0 in a dimention for a panel will make autosize
			                              // 0 in a dimention for a button will resize to text
			float rounding = 0; // Rounding of the element

			// Text Config
			ImVec4 text_col = ImVec4(0., 0., 0., 0.); // Color of the text of a widget
			ImVec2 text_align = ImVec2(0., 0.); // Text align given as percentages in x, and y direction
			std::string font_path = ""; // Pointer ot the ImGui font to be used
			float font_size = 0;

			// Buton Config
			ImVec4 hover_col = ImVec4(0., 0., 0., 0.);
			ImVec4 active_col = ImVec4(0., 0., 0., 0.);

			// Image COnfig
			std::string image_path = "";

			GUIWidgetConfig(EWidgetType _type = WT_None) {
				type = _type;

				bg_col = ImGui::GetStyleColorVec4(ImGuiCol_WindowBg);
				border_col = ImGui::GetStyleColorVec4(ImGuiCol_Border);
				padding = ImVec2(0,0);
				rounding = ImGui::GetStyle().WindowRounding;

				text_col = ImGui::GetStyleColorVec4(ImGuiCol_Text);
				font_size = ImGui::GetFontSize();

				if (_type == WT_Button) bg_col = ImGui::GetStyleColorVec4(ImGuiCol_Button);
				hover_col = ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered);
				active_col = ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive);
			}

			template<typename Archive>
			void save(Archive& archive) const
			{
				archive(cereal::make_nvp("Widget Type", (int)type),
					cereal::make_nvp("Background Colour", bg_col),
					cereal::make_nvp("Border Colour", border_col),
					cereal::make_nvp("Padding Width", padding),
					cereal::make_nvp("Position", position),
					cereal::make_nvp("Size", size),
					cereal::make_nvp("Rounding", rounding),
					cereal::make_nvp("Text Colour", text_col),
					cereal::make_nvp("Font Size", font_size),
					cereal::make_nvp("Text Alignment", text_align),
					cereal::make_nvp("Button Hover Colour", hover_col),
					cereal::make_nvp("Button Active Color", active_col),
					cereal::make_nvp("Image File Path", image_path),
					cereal::make_nvp("Font File Path", font_path));
			}

			template<typename Archive>
			void load(Archive& archive)
			{
				archive(cereal::make_nvp("Widget Type", (EWidgetType)type),
					cereal::make_nvp("Background Colour", bg_col),
					cereal::make_nvp("Border Colour", border_col),
					cereal::make_nvp("Padding Width", padding),
					cereal::make_nvp("Position", position),
					cereal::make_nvp("Size", size),
					cereal::make_nvp("Rounding", rounding),
					cereal::make_nvp("Text Colour", text_col),
					cereal::make_nvp("Font Size", font_size),
					cereal::make_nvp("Text Alignment", text_align),
					cereal::make_nvp("Button Hover Colour", hover_col),
					cereal::make_nvp("Button Active Color", active_col),
					cereal::make_nvp("Image File Path", image_path),
					cereal::make_nvp("Font File Path", font_path));
			}

			GUIWidgetConfig End()
			{
				GUIWidgetConfig tmp = *this;
				tmp.type = WT_PanelEnd;
				return tmp;
			}
		};

	public:
		/** Constuctor */
		GUIModule();

		/** Destructor */
		~GUIModule();

		/** Return the type of the module. */
		static EModuleType GetModuleType() { return MT_GUI; }
	
	private:
		/** Module Initialize. */
		virtual void Initialize() override;

		/** Module Destroy. */
		virtual void Destroy() override;

		ImVec2 PixToPercent(ImVec2 value, ImVec2 max)
		{
			if (max.x * max.y == 0)
			{
				LOGE("Divide by zero error while converting pixels to percentages");
				return ImVec2();
			}

			return ImVec2(value.x / max.x,
				value.y / max.y);
		}
		ImVec2 PercentToPix(ImVec2 value, ImVec2 max)
		{
			return ImVec2(value.x * max.x,
				value.y * max.y);
		}


	public:

		void BeginFrame();

		void EndFrame();

		void BeginPanel(GUIWidgetConfig config);

		void EndPanel();

		void Text(GUIWidgetConfig config, size_t fontID, const char* fmt, ...);

		bool Button(GUIWidgetConfig config, size_t fontID, const char* text);

		void Image(GUIWidgetConfig config, Ptr<Texture2D> img);

		ImDrawData* GetDrawData();

		ImFont* getFont(size_t font_i);

		size_t loadFont(const char* file_name, int pixel_size);

	private:

		GLFWwindow* window = NULL;

		/** A default window/panel configuration
		*	This panel will have no title, scrollbar
		*	It will be unmovable and non resizable*/
		const ImGuiWindowFlags DefaultFlags = 0
			| ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_NoScrollbar
			| ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoNav
			| ImGuiWindowFlags_AlwaysAutoResize;

		int panelCount = 0;

		std::vector<ImFont*> loadedFonts;
	};


}