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
#include <imgui_node_editor.h>
#include "EditorWindow.h"
#include "Animation/AnimationController.h"



namespace Raven
{
	namespace ed = ax::NodeEditor;




	class NodeWindow : public EditorWindow
	{
	public:
		NodeWindow();
		~NodeWindow();
		virtual void OnImGui()  override;
		void OpenFile(const std::string& name);
		void OpenController(const std::shared_ptr<AnimationController>& controller);
		virtual void SaveWorkspace() override;
	private:

		void LoadMeta();

		void LoadAnimation(const std::string& file);
		void HandleQuery();
		void HandleDelete();
		void DrawNode();
		void AddNode(Ptr<AnimationClip> animClip);

		void LeftPanel();
		void RightPanel();

		bool CreateLink(int32_t from, int32_t to);

	
		struct LinkInfo
		{
			ed::LinkId id;
			ed::PinId  inputId;
			ed::PinId  outputId;
		};
		
		struct Node 
		{
			int32_t id;
			ed::PinId inputId;
			ed::PinId outputId;
			std::string nodeName;
			ResourceRef animClip;
		};

		Node* FindNode(int32_t id);

		ed::EditorContext* context = nullptr;
		bool firstFrame = true;
		std::vector<LinkInfo> links;
		std::vector<Node> nodes;
		int32_t nextId = 100;

		std::shared_ptr<AnimationController> controller;
		ImGuiTextFilter filter;


		std::string selectParam;
		bool doubleClick = true;
		std::string settingFile;
	};
};