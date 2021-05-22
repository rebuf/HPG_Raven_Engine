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
#include <vector>

#include "EditorWindow.h"

namespace Raven 
{
	struct FileInfo
	{
		std::string fileName;
		std::string fileType;
		std::string absolutePath;
		bool isFile;
		char* icon;
		ImVec4 colour;
		int type;
	};

	class AssetsWindow : public EditorWindow
	{
	public:
		AssetsWindow();
		virtual void OnImGui() override;

		bool DrawFile(int32_t dirIndex, bool folder, int32_t shownIndex, bool gridView);
		void DrawFolder(const FileInfo & dirInfo);
		void RenderNavigationBar();
		void RenderBottom();
		void GetDirectories(const std::string& path);

		void PopupWindow();

		inline auto GetParsedAssetID(const std::string& extension)
		{
			for (int i = 0; i < assetTypes.size(); i++)
			{
				if (extension == assetTypes[i])
				{
					return i;
				}
			}

			return -1;
		}

		static std::string ParseFilename(const std::string& str, const char delim, std::vector<std::string>& out);
		static std::string ParseFiletype(const std::string& filename);

		static std::vector<FileInfo> GetFsContents(const std::string& path);
		static std::vector<FileInfo> ReadDirectory(const std::string& path);

		static std::vector<FileInfo> GetChildDirectories(const std::string& path);
		static std::string GetParentPath(const std::string& path);


		static std::vector<std::string> SearchFiles(const std::string& query);
		static bool Move(const std::string& filePath, const std::string& movePath);

		static std::string StripExtras(const std::string& filename);

	private:
		void SplitPath(std::string relativePath);
		bool IsInSplitPath(const std::string& dir);

		static inline std::vector<std::string> assetTypes = {
			"fbx", "obj", "wav", "mp3","png","jpg","bmp", "raven", "ogg", "lua" };

		std::string currentDirPath;
		std::string baseDirPath;
		std::string previousDirPath;
		std::string movePath;
		std::string lastNavPath;
		static std::string delimiter;

		std::string directories[100];
		int32_t directoryCount;

		size_t basePathLen;
		bool isDragging = false;
		bool isInListView = true;
		bool updateBreadCrumbs = true;
		bool showHiddenFiles = false;
		int32_t gridItemsPerRow;

		ImGuiTextFilter filter;

		char* inputText = nullptr;
		char* inputHint = nullptr;
		char inputBuffer[1024];

		std::vector<FileInfo> currentDir;
		std::vector<FileInfo> baseProjectDir;
		std::vector<std::string> splitPath;

	};
};