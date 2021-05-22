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
#include "GUIModule.h"
#include "GUIWidget.h"

#include "ResourceManager/Resources/IResource.h"

#include <vector>


#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <string>
#include <Engine.h>
#include <ResourceManager/Resources/Texture2D.h>
#include <ResourceManager/ResourceManager.h>
#include "Utilities/StringUtils.h"

namespace Raven {

	/// <summary>
	/// This is a layout for a GUI in the scene that should be laoded from file
	/// 
	/// Sample code for generating a layout can be found at the bottom of this file
	/// </summary>
	class GUILayout : public IResource {
	private:
		struct GUITextVar
		{
			enum EVartType
			{
				VT_None = 0,
				VT_Float = 1,
				VT_Int = 2,
				VT_String = 3
			};

			std::string identifier = "";
			EVartType type = VT_None;
			Ptr<void> value = NULL;

			std::string ToString()
			{
				std::string ret = "%";

				if (type == VT_Float) ret += "f";
				if (type == VT_Int) ret += "i";
				if (type == VT_String) ret += "s";

				return ret + "(" + identifier + ")";
			}

			bool IsValid()
			{
				return identifier != "" && type != VT_None && value != NULL;
			}

			void FillString(std::string* str)
			{
				if (type == VT_Float) StringUtils::Replace(*str, ToString(), std::to_string(*(float*)value.get()));
				if (type == VT_Int) StringUtils::Replace(*str, ToString(), std::to_string(*(int*)value.get()));
				else if (type == VT_String)	StringUtils::Replace(*str, ToString(), *static_cast<std::string*>(value.get()));

			}
		};

	public:
		GUILayout();

		// Add a widget to the layout
		void addWidget(std::string wName,
			GUIModule::GUIWidgetConfig pCFG,
			const char* wText = "");

		void ParseTextVars(std::string str);

		/* Pass a callback function to the ith button in the layout */
		void setButtonCallback(const char* bName, const std::function<void(GUILayout* layout)>& bCallback);
		void SetTextVarPointer(std::string varID, Ptr<void> ptr);
		void ToggleWidgetVisible(const char* wName);

		// Draw the layout (Called by the GUIComponent containing the layout)
		void drawLayout();

		// return the resource type
		inline static EResourceType StaticGetType() noexcept { return EResourceType::RT_GuiLayout; }

		std::string name = "TestLayout";

		template<typename Archive>
		void save(Archive& archive) const
		{
			archive(cereal::base_class<IResource>(this));

			archive(cereal::make_nvp("Layout Name", name),
				cereal::make_nvp("Widgets", widgets));
		}

		template<typename Archive>
		void load(Archive& archive)
		{
			archive(cereal::base_class<IResource>(this));

			archive(cereal::make_nvp("Layout Name", name),
				cereal::make_nvp("Widgets", widgets));
		}


		GUIWidget* getWidgetByName(const char* wName);

		void InitLayout();

		static void generateLayout(const char* filename);

		std::vector<GUITextVar> textVars;
	private:
		std::vector<GUIWidget> widgets;
	};
}

/* Sample Layout Generation
	GUILayout tempLayout;

	GUIModule::GUIPanelConfig startPanelCFG;
	startPanelCFG.size = ImVec2(.2, .1);
	startPanelCFG.bg_col = ImVec4(0., 1., 1., 0.);
	startPanelCFG.position = ImVec2(.4, .1);
	startPanelCFG.rounding = 0.;
	startPanelCFG.padding = ImVec2(0., 0);
	startPanelCFG.border_col = ImVec4(0, 0, 0, 0);

	GUIModule::GUITextConfig startTextCFG;
	startTextCFG.color = ImVec4(0., 0., 0., 1.);
	startTextCFG.text_align = ImVec2(0., .5);

	GUIModule::GUIButtonConfig startButtonCFG;
	startButtonCFG.size = ImVec2(1., 1.);
	startButtonCFG.color = ImVec4(163. / 255., 250. / 255., 231. / 255., 255. / 255.);
	startButtonCFG.color_hover = ImVec4(91 / 255., 250. / 255., 231. / 255., 255. / 255.);
	startButtonCFG.color_active = ImVec4(74 / 255., 179 / 255., 255 / 255., 255. / 255.);

	tempLayout.addWidget("TestPanel1", GUILayout::EWidgetType::WT_Panel, startPanelCFG);
	tempLayout.addWidget("TestButton", GUILayout::EWidgetType::WT_Button, startPanelCFG, startTextCFG, startButtonCFG, "Toggle Panel 2 Visibility");
	tempLayout.addWidget("/TestPanel1", GUILayout::EWidgetType::WT_PanelEnd);

	startPanelCFG.bg_col = ImVec4(1., 0., 1., 1.);
	startPanelCFG.position = ImVec2(.45, .5);
	startPanelCFG.size = ImVec2(.1, .1);
	tempLayout.addWidget("TestPanel2", GUILayout::EWidgetType::WT_Panel, startPanelCFG);
	tempLayout.addWidget("TestText", GUILayout::EWidgetType::WT_Text, startPanelCFG, startTextCFG, startButtonCFG, "Test Text");
	tempLayout.addWidget("/TestPanel2", GUILayout::EWidgetType::WT_PanelEnd);

	// Layout Output Code
	// Will most likely be used in layout design tool but not in the game code itself

	std::stringstream storage;
	std::string path("OUTPUT_FILE_PATH");

	{
		cereal::JSONOutputArchive output{ storage };
		output(tempLayout);
	}

	std::ofstream file(path, std::ios::binary);

	file << storage.str();

	file.flush();
*/