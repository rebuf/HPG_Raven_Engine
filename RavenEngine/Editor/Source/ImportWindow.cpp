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

#include "ImportWindow.h"

#include "Engine.h"

#include "ResourceManager/FileSystem.h"
#include "ResourceManager/ResourceManager.h"
#include "ResourceManager/Importers/FBXImporter.h"

#include "Animation/Skeleton.h"

#include "Logger/Console.h"

#include <imgui.h>
#include <ImGuiFD/ImGuiFileDialog.h>

namespace Raven
{
	ImportWindow::ImportWindow() :
		resourceManager(Engine::Get().GetModule<ResourceManager>()), // pointer to resource manager
		input("\\"),
		onlyAnimation(false),
		selected(false),
		dragInfo("Drag skeleton here")
	{
		title = "Import";
		for (auto& ext : resourceManager->GetSupportedExtensions())
		{
			filter.append('.' + ext + ',');
		}
		LOGC(filter.c_str());
	} 

	void ImportWindow::OnImGui()
	{
		if (ImGui::Begin(title.c_str(), &active))
		{

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
			ImGui::Columns(1);
			ImGui::PushItemWidth(-1.0f); // means fill available space for any non button widget
			float w = ImGui::GetContentRegionAvailWidth(); // for the button to stretch full 

			if (ImGui::InputText("Select file", input, IM_ARRAYSIZE(input)))
			{
				// Open a file browser dialogue once library is included
				filePath = StringUtils::GetCurrentWorkingDirectory().append(input);
			}

			if (ImGui::Button("Open File Browser Dialog", ImVec2(w, 0.0f)))
			{
				// TODO: change the extensions to valid resources
				igfd::ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", filter.c_str(), ".");
			}

			if (igfd::ImGuiFileDialog::Instance()->FileDialog("ChooseFileDlgKey"))
			{
				// action if OK
				if (igfd::ImGuiFileDialog::Instance()->IsOk == true)
				{
					std::string filePathName = igfd::ImGuiFileDialog::Instance()->GetFilePathName();
					std::string filePath = igfd::ImGuiFileDialog::Instance()->GetCurrentPath(); // will return an absolute path
					// action
				}
				// close
				igfd::ImGuiFileDialog::Instance()->CloseDialog("ChooseFileDlgKey");
			}

			if (StringUtils::GetExtension(filePath) == "fbx")
			{
				ImGui::Columns(2);
				ImGui::TextUnformatted("Animation only");
				ImGui::NextColumn();
				// if an fbx, determine if we want just the animation (if so, specify a skeleton)
				ImGui::Checkbox("##Animation only", &onlyAnimation);
				ImGui::Columns(1);

				if (onlyAnimation)
				{
					//ImGui::Button(dragInfo.c_str(), ImVec2(w, 0.0f));
					ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(255, 0, 0, 100));
					ImGui::BeginChild("Drag area", ImVec2(w, 40.0f));
					ImGui::TextUnformatted(dragInfo.c_str());
					ImGui::EndChild();
					if (ImGui::BeginDragDropTarget())
					{
						auto data = ImGui::AcceptDragDropPayload("AssetFile");
						if (data)
						{
							PRINT_FUNC();
							std::string path = (char*)data->Data;
							if (Engine::GetModule<ResourceManager>()->GetResourceType(path) == EResourceType::RT_Skeleton)
							{
								dragInfo = path;
							}
						}
						ImGui::EndDragDropTarget();
					}
					ImGui::PopStyleColor();
				}
			}

			ImGui::Separator();
			if (ImGui::Button("Import", ImVec2(w, 0.0f)))
			{
				LOGW("Path: {0}", filePath.c_str());
				if (onlyAnimation && dragInfo != "Drag here")
				{
					// in case of animation only, change the import settings of the fbx importer
					resourceManager->GetImporter<FBXImporter>()->settings.skeleton = resourceManager->GetResource<Skeleton>(dragInfo);
					resourceManager->GetImporter<FBXImporter>()->settings.importAnimationOnly = true;
				}	
				// Imports the resource
				resourceManager->Import(filePath);
				selected = false;
				dragInfo = "Drag here";
			}
			ImGui::PopStyleVar();
		}
		ImGui::End();
	}
};

