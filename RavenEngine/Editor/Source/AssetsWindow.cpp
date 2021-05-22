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
#ifdef _WIN32
#include "windows.h"
#endif
#include "AssetsWindow.h"
#include "ResourceWindow.h"
#include <IconsMaterialDesignIcons.h>
#include <imgui.h>
#include <imgui_internal.h>
#include "Logger/Console.h"

#include <algorithm>
#include "Animation/AnimationController.h"
#include "Utilities/StringUtils.h"
#include "Editor.h"
#include "ResourceManager/ResourceManager.h"
#include "ResourceManager/Resources/Material.h"
#include "ResourceManager/Resources/MaterialShader.h"

#include <filesystem>




namespace Raven 
{
#ifdef _WIN32
	std::string AssetsWindow::delimiter = "\\";
#else
	std::string AssetsWindow::delimiter = "/";
#endif



	AssetsWindow::AssetsWindow()
	{
		title = "Assets";

		baseDirPath = ".";
		currentDirPath = baseDirPath;
		previousDirPath = currentDirPath;
		lastNavPath = baseDirPath;
		baseProjectDir = ReadDirectory(baseDirPath);
		currentDir = ReadDirectory(baseDirPath);
		basePathLen = baseDirPath.length();

	}

	void AssetsWindow::OnImGui()
	{
		ImGui::Begin(title.c_str());
		{
			ImGui::Columns(2, "AB", true);

			ImGui::BeginChild("##folders_common");
			{
				RenderNavigationBar();

				{
					ImGui::BeginChild("##folders");
					{
						for (auto i = 0; i < baseProjectDir.size(); i++)
							DrawFolder(baseProjectDir[i]);
					}
					ImGui::EndChild();
				}

				if (ImGui::IsMouseDown(1))
				{
					ImGui::OpenPopup("window");
				}
			}

			ImGui::EndChild();


			if (ImGui::BeginDragDropTarget())
			{
				auto data = ImGui::AcceptDragDropPayload("selectable", ImGuiDragDropFlags_AcceptNoDrawDefaultRect);
				if (data)
				{
					std::string file = (char*)data->Data;
					if (this->Move(file, movePath))
					{
						LOGV("Moved File: " + file + " to " + movePath);
						currentDir = ReadDirectory(currentDirPath);
					}
					isDragging = false;
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::NextColumn();

			ImGui::BeginChild("##directory_structure");
			{
				{
					ImGui::BeginChild("##directory_navigation_bar", ImVec2(ImGui::GetColumnWidth(), 30));
					if (isInListView)
					{
						if (ImGui::Button(ICON_MDI_VIEW_GRID))
						{
							isInListView = !isInListView;
						}
						ImGui::SameLine();
					}
					else
					{
						if (ImGui::Button(ICON_MDI_VIEW_LIST))
						{
							isInListView = !isInListView;
						}
						ImGui::SameLine();
					}

					ImGui::TextUnformatted(ICON_MDI_MAGNIFY);

					ImGui::SameLine();

					filter.Draw("##Filter", ImGui::GetContentRegionAvail().x - ImGui::GetStyle().IndentSpacing);

					ImGui::EndChild();
				}

				{
					ImGui::BeginChild("##Scrolling");

					if (ImGui::IsMouseClicked(1)) {
						ImGui::OpenPopup("AssetsWindow::PopupWindow");
					}
					PopupWindow();

					int shownIndex = 0;

					float xAvail = ImGui::GetContentRegionAvail().x;
					gridItemsPerRow = (int32_t)std::floor(xAvail / 70.0f);
					gridItemsPerRow = std::max(1, gridItemsPerRow);

					for (int i = 0; i < currentDir.size(); i++)
					{
						if (currentDir.size() > 0)
						{
							if (!showHiddenFiles && StringUtils::IsHiddenFile(currentDir[i].fileName))
							{
								continue;
							}

							if (filter.IsActive())
							{
								if (!filter.PassFilter(currentDir[i].fileName.c_str()))
								{
									continue;
								}
							}

							bool doubleClicked = DrawFile(i, !currentDir[i].isFile, shownIndex, !isInListView);

							if (doubleClicked)
								break;
							shownIndex++;
						}
					}

					ImGui::EndChild();
				}
				ImGui::EndChild();

			

			}

			if (ImGui::BeginDragDropTarget())
			{
				auto data = ImGui::AcceptDragDropPayload("selectable", ImGuiDragDropFlags_AcceptNoDrawDefaultRect);
				if (data)
				{
					std::string a = (char*)data->Data;
					if (this->Move(a, movePath))
					{
						LOGI("Moved File: " + a + " to " + movePath);
					}
					isDragging = false;
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::End();
		}
	}

	bool AssetsWindow::DrawFile(int32_t dirIndex, bool folder, int32_t shownIndex, bool gridView)
	{
		//auto fileID = GetParsedAssetID(currentDir[dirIndex].fileType);
		bool doubleClicked = false;

		auto & editor = static_cast<Editor&>(Editor::Get());

		if (gridView)
		{
			ImGui::BeginGroup();

			//auto fileID = GetParsedAssetID(currentDir[dirIndex].fileType);

			if (ImGui::Button(currentDir[dirIndex].icon, ImVec2(70.0f, 70.0f)))
			{

			}

			if (ImGui::IsMouseDoubleClicked(0))
			{
				doubleClicked = true;
			}

			auto& fname = currentDir[dirIndex].fileName;
			auto newFname = StripExtras(fname);

			ImGui::TextWrapped("%s", newFname.c_str());
			ImGui::EndGroup();

			if ((shownIndex + 1) % gridItemsPerRow != 0)
				ImGui::SameLine();

		}
		else
		{
			ImGui::TextUnformatted(currentDir[dirIndex].icon);
			if (ImGui::IsItemHovered() && currentDir[dirIndex].isFile)
				ImGui::SetTooltip(ResourceToString( static_cast<EResourceType>(currentDir[dirIndex].type )).c_str());
			ImGui::SameLine();
			if (ImGui::Selectable(currentDir[dirIndex].fileName.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick))
			{
				if (ImGui::IsMouseDoubleClicked(0))
				{
					doubleClicked = true;
				}
			}
		}

		if (doubleClicked)
		{
			if (folder)
			{
				previousDirPath =currentDir[dirIndex].absolutePath;
				currentDirPath = currentDir[dirIndex].absolutePath;
				currentDir = ReadDirectory(currentDir[dirIndex].absolutePath);
			}

			else
			{
				//LOGW("Open File {0} did not implementation {1}", currentDir[dirIndex].absolutePath,__FUNCTION__);
				//openFile
				editor.OpenFile(currentDir[dirIndex].absolutePath);

			}
		}

		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
		{
			ImGui::TextUnformatted(editor.GetIconFontIcon(currentDir[dirIndex].absolutePath));

			ImGui::SameLine();
			ImGui::TextUnformatted(currentDir[dirIndex].fileName.c_str());
			size_t size = currentDir[dirIndex].absolutePath.length();
			ImGui::SetDragDropPayload("AssetFile", currentDir[dirIndex].absolutePath.c_str(), size + 1);
			isDragging = true;
			ImGui::EndDragDropSource();
		}

		return doubleClicked;
	}

	void AssetsWindow::DrawFolder(const FileInfo& dirInfo)
	{
		ImGuiTreeNodeFlags nodeFlags = ((dirInfo.absolutePath == currentDirPath) ? ImGuiTreeNodeFlags_Selected : 0);
		nodeFlags |= ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

		const ImColor TreeLineColor = ImColor(128, 128, 128, 128);
		const float SmallOffsetX = 6.0f;
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		/*
		if (!dirInfo.isFile)
		{
			bool containsFolder = false;
			for (auto& file : children)
			{
				if (!file.isFile)
				{
					containsFolder = true;
					break;
				}
			}
			if (!containsFolder)
		*/
			auto children = GetChildDirectories(dirInfo.absolutePath);
			if (children.empty())
				nodeFlags |= ImGuiTreeNodeFlags_Leaf;

			static std::string folderIcon = ICON_MDI_FOLDER " ";

			bool isOpen = ImGui::TreeNodeEx((folderIcon + dirInfo.fileName).c_str(), nodeFlags);

			ImVec2 verticalLineStart = ImGui::GetCursorScreenPos();

			if (ImGui::IsItemClicked())
			{
				previousDirPath = GetParentPath(currentDirPath);
				currentDirPath = dirInfo.absolutePath;
				currentDir = ReadDirectory(dirInfo.absolutePath.c_str());
			}

			if (isOpen)
			{
				verticalLineStart.x += SmallOffsetX; //to nicely line up with the arrow symbol
				ImVec2 verticalLineEnd = verticalLineStart;

				auto children = GetChildDirectories(dirInfo.absolutePath);

				for (int i = 0; i < children.size(); i++)
				{
					/*
					if (!currentDir[i].isFile)
					{
						auto dirDataTemp = GetChildDirectories(children[i].absolutePath);

						if (!dirDataTemp.empty())
							HorizontalTreeLineSize *= 0.5f;
					}
					*/
						float HorizontalTreeLineSize = 16.0f; //chosen arbitrarily
						auto currentPos = ImGui::GetCursorScreenPos();

						ImGui::Indent(10.0f);

						DrawFolder(children[i]); // draw a folder for each child directory

						const ImRect childRect(currentPos, ImVec2{ currentPos.x,  currentPos.y+ImGui::GetFontSize() });

						const float midpoint = (childRect.Min.y + childRect.Max.y) / 2.0f;
						drawList->AddLine(ImVec2(verticalLineStart.x, midpoint), ImVec2(verticalLineStart.x + HorizontalTreeLineSize, midpoint), TreeLineColor);
						verticalLineEnd.y = midpoint;

						ImGui::Unindent(10.0f);
				}

				drawList->AddLine(verticalLineStart, verticalLineEnd, TreeLineColor);

				ImGui::TreePop();
			}
			/*
			if (isOpen && !containsFolder)
				ImGui::TreePop();
			*/
		//}

		if (isDragging && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
		{
			movePath = dirInfo.absolutePath.c_str();
		}
	}

	void AssetsWindow::RenderNavigationBar()
	{
		ImGui::BeginChild("##directory_navigation_bar", ImVec2(ImGui::GetColumnWidth(), 30));
		{
			if (ImGui::Button(ICON_MDI_ARROW_LEFT))
			{
				if (currentDirPath.length() != basePathLen)
				{
					previousDirPath = GetParentPath(currentDirPath);
					currentDirPath = previousDirPath;
					currentDir = ReadDirectory(currentDirPath);
				}
			}
			ImGui::SameLine();
			if (ImGui::Button(ICON_MDI_ARROW_RIGHT))
			{
				previousDirPath = GetParentPath(currentDirPath);
				currentDirPath = lastNavPath;
				currentDir = ReadDirectory(lastNavPath);
			}
			ImGui::SameLine();

			GetDirectories(currentDirPath);

			int32_t secIdx = 0, newPwdLastSecIdx = -1;
			auto dir = std::filesystem::path(currentDirPath);

			std::string filePath(dir.string());
			
			ImGui::TextUnformatted(filePath.c_str());

			ImGui::SameLine();
		}

		ImGui::EndChild();
	}

	void AssetsWindow::RenderBottom()
	{
		ImGui::BeginChild("##nav", ImVec2(ImGui::GetColumnWidth() - 12, 23));
		{
			ImGui::EndChild();
		}
	}

	void AssetsWindow::GetDirectories(const std::string& path)
	{
		directoryCount = 0;
		size_t start;
		size_t end = 0;

		while ((start = path.find_first_not_of(delimiter.c_str(), end)) != std::string::npos)
		{
			end = path.find(delimiter.c_str(), start);
			directories[directoryCount] = path.substr(start, end - start);
			directoryCount++;
		}
	}

	void AssetsWindow::PopupWindow()
	{
		if (ImGui::BeginPopupContextWindow("AssetsWindow::PopupWindow"))
		{
			bool isAdd = false;

			if (ImGui::Selectable("Add Animation Controller"))
			{
				Ptr<AnimationController> controller(new AnimationController);
				Engine::GetModule<ResourceManager>()->SaveNewResource(controller,
					currentDirPath + "/New_Controller.raven");

				isAdd = true;
			}

			if (ImGui::Selectable("Add Material"))
			{
				Ptr<Material> material(new Material());
				Engine::GetModule<ResourceManager>()->SaveNewResource(material,
					currentDirPath + "/New_Material.raven");

				isAdd = true;
			}

			if (ImGui::Selectable("Add Shader"))
			{
				Ptr<MaterialShader> shader(new MaterialShader());
				Engine::GetModule<ResourceManager>()->SaveNewResource(shader,
					currentDirPath + "/New_Shader.raven");

				static_cast<Editor&>(Editor::Get()).GetWindow<ResourceWindow>()->SetResource(shader);

				isAdd = true;
			}

			if (isAdd)
			{
				currentDir = ReadDirectory(currentDirPath);
				//baseProjectDir = ReadDirectory(baseDirPath);
				//currentDir = baseProjectDir;
			}

			ImGui::EndPopup();
		}
	}

	std::string AssetsWindow::ParseFilename(const std::string& str, const char delim, std::vector<std::string>& out)
	{
		size_t start;
		size_t end = 0;

		while ((start = str.find_first_not_of(delim, end)) != std::string::npos)
		{
			end = str.find(delim, start);
			out.push_back(str.substr(start, end - start));
		}

		return out[out.size() - 1];
	}

	std::string AssetsWindow::ParseFiletype(const std::string& filename)
	{
		size_t start;
		size_t end = 0;
		std::vector<std::string> out;

		while ((start = filename.find_first_not_of(".", end)) != std::string::npos)
		{
			end = filename.find(".", start);
			out.push_back(filename.substr(start, end - start));
		}

		return out[out.size() - 1];
	}

	std::vector<FileInfo> AssetsWindow::GetFsContents(const std::string& path)
	{
		std::vector<FileInfo> dInfo;

		for (const auto& entry : std::filesystem::directory_iterator(path))
		{

			bool isDir = std::filesystem::is_directory(entry);
			auto test = std::vector<std::string>();
			const char del = *delimiter.c_str();

			auto dir_data = ParseFilename(entry.path().string(), del, test);
			auto fileExt = ParseFiletype(dir_data);

			if (isDir)
			{
				dInfo.push_back({ dir_data, fileExt, entry.path().string(), false });
			}
			else
			{
				dInfo.push_back({ dir_data, fileExt, entry.path().string(), true });
			}
		}

		return dInfo;
	}

	std::vector<FileInfo> AssetsWindow::ReadDirectory(const std::string& path)
	{
		std::vector<FileInfo> dInfo;
		auto rm = Engine::Get().GetModule<ResourceManager>();

		for (const auto& entry : std::filesystem::directory_iterator(path))
		{
			bool isDir = std::filesystem::is_directory(entry);

			auto test = std::vector<std::string>();
			const char del = *delimiter.c_str();

			auto dir_data = ParseFilename(entry.path().string(), del, test);
			auto fileExt = ParseFiletype(dir_data);

			if (isDir)
			{
				dInfo.push_back({ StringUtils::RemoveExtension(dir_data), fileExt, entry.path().string(), false, ICON_MDI_FOLDER, ImVec4(1.0f, 1.0f, 1.0f, 1.0f), EResourceType::RT_None });
				continue; // add continue here so a folder call "raven" isnt considered a resource
			}
			// use the path to determine if a resource or not (see the type)
			auto type = rm->GetResourceType(entry.path().string());
			if (type != EResourceType::RT_None)
			{
				// raven resources are , so try to get the resource 
				dInfo.push_back({ StringUtils::RemoveExtension(dir_data), fileExt, entry.path().string(), true, ResourceToIcon(type), ImVec4(1.0f, 1.0f, 1.0f, 1.0f), type });
			}
		}

		return dInfo;
	}

	std::vector<FileInfo> AssetsWindow::GetChildDirectories(const std::string& path)
	{
		std::vector<FileInfo> dInfo;

		for (const auto& entry : std::filesystem::directory_iterator(path))
		{
			if (std::filesystem::is_directory(entry))
			{
				auto test = std::vector<std::string>();
				const char del = *delimiter.c_str();

				auto dir_data = ParseFilename(entry.path().string(), del, test);
				dInfo.push_back({ dir_data, ".raven", entry.path().string(), false });
			}
		}

		return dInfo;
	}

	std::string AssetsWindow::GetParentPath(const std::string& path)
	{
		auto p = std::filesystem::path(path);
		return p.parent_path().string();
	}

	std::vector<std::string> AssetsWindow::SearchFiles(const std::string& query)
	{
		return std::vector<std::string>();
	}

	bool AssetsWindow::Move(const std::string& filePath, const std::string& movePath)
	{
		return false;
	}

	std::string AssetsWindow::StripExtras(const std::string& filename)
	{
		std::vector<std::string> out;
		size_t start;
		size_t end = 0;

		while ((start = filename.find_first_not_of(".", end)) != std::string::npos)
		{
			end = filename.find(".", start);
			out.push_back(filename.substr(start, end - start));
		}

		if (out[0].length() >= 8)
		{
			auto cutFilename = "     " + out[0].substr(0, 5) + "...";
			return cutFilename;
		}

		auto filenameLength = out[0].length();
		auto paddingToAdd = 9 - filenameLength;

		std::string newFileName;

		for (int i = 0; i <= paddingToAdd; i++)
		{
			newFileName += " ";
		}

		newFileName += out[0];

		return newFileName;
	}

	void AssetsWindow::SplitPath(std::string relativePath)
	{
		splitPath.clear();
		// starting from the end of the string, remove elements till there are no separators left (or the relative path is just the base directory)
		while (relativePath.compare(baseDirPath) != 0)
		{
			// find the last occurence of the delimiter in the string
			auto lastDel = relativePath.find_last_of(delimiter);
			// extract any character from the string after the delimiter and append it to the split path
			splitPath.push_back(relativePath.substr(lastDel + 1));
			// update the relative path by removing the 
			relativePath.erase(lastDel, lastDel + splitPath.back().size());
		}
	}

	bool AssetsWindow::IsInSplitPath(const std::string& dir)
	{
		// returns true if the directory name is in the current directory's split relative path
		for (auto& s : splitPath)
			if (s.compare(dir) == 0)
				return true;
		return false;
	}
};

