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
#include "GUILayout.h"
#include "Engine.h"
#include <algorithm>
#include "Logger/Console.h"

#include <sstream>
#include <fstream>
#include <cereal/include/cereal/archives/json.hpp>

namespace Raven {

	/** Constructor */
	GUILayout::GUILayout() 
		: IResource()
	{
		type = GUILayout::StaticGetType();
	}

	/** Add a widget to the layout */
	void GUILayout::addWidget(std::string wName,
		GUIModule::GUIWidgetConfig config,
		const char* wText)
	{
		GUIWidget newWidget;
		newWidget.widgetName = wName;
		newWidget.config = config;
		newWidget.text = wText;
		newWidget.callback = NULL; 

		widgets.push_back(newWidget);
	}

	void GUILayout::ParseTextVars(std::string str)
	{
		// For now just find one identifier
		size_t pos = str.find("%");
		// if found
		if (pos != std::string::npos)
		{
			size_t iStart = pos + 3;
			size_t iEnd = str.find(")", iStart);
			std::string varI = str.substr(iStart, iEnd - iStart);
			// If it an f identifier
			if (str[pos + 1] == 'f')
			{
				textVars.push_back(GUITextVar{ varI, 
											   GUITextVar::VT_Float, 
										       NULL });
			}
			else if (str[pos + 1] == 's')
			{
				textVars.push_back(GUITextVar{ varI,
											   GUITextVar::VT_String,
											   NULL });

			}
			else if (str[pos + 1] == 'i')
			{
				textVars.push_back(GUITextVar{ varI,
											   GUITextVar::VT_Int,
											   NULL });

			}
		}
	}

	void GUILayout::drawLayout() 
	{
		bool cVis = true;
		auto regi = Engine::Get().GetModule<GUIModule>();
		for (auto& widget : widgets)
		{
			switch (widget.config.type)
			{
			case GUIModule::EWidgetType::WT_None:
				/* TODO: Add error checking, type should never be WT_None */
				break;
			case GUIModule::EWidgetType::WT_Panel:
				cVis = widget.visible;
				if (cVis) regi->BeginPanel(widget.config);
				break;
			case GUIModule::EWidgetType::WT_PanelEnd:
				if (cVis) regi->EndPanel();
				break;
			case GUIModule::EWidgetType::WT_Text:
				if (cVis)
				{
					std::string str = widget.text;
					for (auto& textV : textVars)
					{
						if (textV.IsValid()) textV.FillString(&str);
					}
					regi->Text(widget.config, widget.fontID, str.c_str(), "Replace failed");
				}
				break;
			case GUIModule::EWidgetType::WT_Button:
				if (cVis)
				{
					if (regi->Button(widget.config, widget.fontID, widget.text.c_str()))
					{
						if (widget.callback != NULL) widget.callback(this);
					}
				}
				break;
			case GUIModule::EWidgetType::WT_Image:
				if (cVis)
				{
					regi->Image(widget.config, widget.img);
				}
				break;
			default:
				break;
			}
		}
	}


	void GUILayout::setButtonCallback(const char* bName, const std::function<void(GUILayout* layout)>& bCallback) 
	{
		auto w = getWidgetByName(bName);
		if (w) w->callback = bCallback;
	}


	void GUILayout::SetTextVarPointer(std::string varID, Ptr<void> ptr)
	{
		for (auto& tv : textVars)
		{
			if (tv.identifier == varID) tv.value = ptr;
		}
	}

	void GUILayout::ToggleWidgetVisible(const char* wName)
	{
		auto w = getWidgetByName(wName);
		if (w) w->ToggleVisible();
	}


	GUIWidget* GUILayout::getWidgetByName(const char* wName) 
	{
		for (auto& w : widgets)
		{
			if (w.widgetName == wName) 
			{
				return &w;
			}
		}

		LOGC("Attempt to get a widget: \"{}\", that doesnt exist", wName);
		return NULL;
	}

	void GUILayout::InitLayout()
	{
		for (auto& widget : widgets)
		{
			widget.LoadResources();

			if (widget.text != "")
			{
				ParseTextVars(widget.text);
			}
		}
	}


	void GUILayout::generateLayout(const char* filePath)
	{
		GUILayout tempLayout;

		GUIModule::GUIWidgetConfig tpConfig(GUIModule::WT_Panel);
		tpConfig.position = ImVec2(.4, .4);
		tpConfig.size = ImVec2(.2, .05);
		tpConfig.bg_col = ImVec4(.3, .3, .3, .3);
		tpConfig.border_col = ImVec4(1,1,1,1);
		tpConfig.rounding = 0.;

		GUIModule::GUIWidgetConfig ttConfig(GUIModule::WT_Text);
		ttConfig.text_align = ImVec2(0.5, 0.5);
		ttConfig.text_col = ImVec4(0, 0, 0, 1);
		ttConfig.font_path = "./layouts/fonts/DroidSans.ttf";
		ttConfig.font_size = 40;

		GUIModule::GUIWidgetConfig spConfig(GUIModule::WT_Panel);
		spConfig.position = ImVec2(.35, .6);
		spConfig.size = ImVec2(.3, .2);
		spConfig.bg_col = ImVec4(0,0,0,0);
		spConfig.border_col = ImVec4(1, 1, 1, 0);
		spConfig.rounding = 0.;

		GUIModule::GUIWidgetConfig sbConfig(GUIModule::WT_Button);
		sbConfig.position = ImVec2(0,0);
		sbConfig.size = ImVec2(1.,1.);
		sbConfig.text_align = ImVec2(.5,.5);
		sbConfig.bg_col = ImVec4(.8,.8,.8,1.);
		sbConfig.text_col = ImVec4(1, 1, 1, 1);
		sbConfig.font_path = "./layouts/fonts/DroidSans.ttf";
		sbConfig.font_size = 25;

		tempLayout.addWidget("TitlePanel", tpConfig);
		tempLayout.addWidget("TitleText", ttConfig, "Raven Game");
		tempLayout.addWidget("/TitlePanel", tpConfig.End());

		tempLayout.addWidget("StartPanel", spConfig);
		tempLayout.addWidget("StartText", sbConfig, "Start Game");
		tempLayout.addWidget("/StartPanel", spConfig.End());

		// Layout Output Code
		// Will most likely be used in layout design tool but not in the game code itself

		std::stringstream storage;

		{
			cereal::JSONOutputArchive output{ storage };
			output(tempLayout);
		}

		std::ofstream file(filePath, std::ios::binary);

		file << storage.str();

		file.flush();
	}
}