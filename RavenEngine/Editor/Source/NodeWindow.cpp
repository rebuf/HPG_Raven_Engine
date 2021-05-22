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


#include "NodeWindow.h"
#include "Utilities/StringUtils.h"
#include "Logger/Console.h"
#include "Animation/Animation.h"
#include "Animation/AnimationController.h"
#include "PropertiesWindow.h"
#include "Utilities/StringUtils.h"
#include "imgui_internal.h"
#include "ImGui/ImGuiHelpers.h"
#include "Scene/SceneManager.h"
#include "Scene/Entity/Entity.h"

#include "Animation/Animator.h"
#include "Editor.h"
#include "ResourceManager/ResourceManager.h"


namespace Raven 
{

	NodeWindow::NodeWindow()
	{
		title = "Animator";
	}

	NodeWindow::~NodeWindow()
	{
		if(context !=nullptr)
			ed::DestroyEditor(context);
	}

	void NodeWindow::OnImGui()
	{
		auto& editor = static_cast<Editor&>(Engine::Get());
		if (editor.GetEditorState() == EditorState::Play) 
		{
			auto curr = editor.GetModule<SceneManager>()->GetCurrentScene();
			Entity entity{ editor.GetSelected() ,curr };
			if (entity.Valid())
			{
				auto animator = entity.TryGetComponent<Animator>();

				if (animator)
				{
					auto ctrlInstance =  animator->GetController();
					if (ctrlInstance)
					{
						if(ctrlInstance->Get() != controller)
							OpenController(ctrlInstance->Get());
					}
				}
			}
		}


		ed::SetCurrentEditor(context);

		if (ImGui::Begin(title.c_str(), &active))
		{
			ImGui::Columns(2, "A--B", true);
			LeftPanel();

			ImGui::NextColumn();
			RightPanel();

			if (ImGui::BeginDragDropTarget())
			{
				auto data = ImGui::AcceptDragDropPayload("AssetFile");
				if (data)
				{
					std::string file = (char*)data->Data;

					if (Engine::GetModule<ResourceManager>()->GetResourceType(file) == RT_AnimationClip)
					{
						LoadAnimation(file);
					}
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::Columns(1);

		}
		ImGui::End();

		ed::SetCurrentEditor(nullptr);
		firstFrame = false;
	}

	void NodeWindow::OpenFile(const std::string& name)
	{
		controller = Engine::GetModule<ResourceManager>()->GetResource<AnimationController>(name);
		LoadMeta();
	}

	void NodeWindow::LoadMeta()
	{
		nodes.clear();
		nextId = 100;

		auto& node = nodes.emplace_back();
		node.id = nextId++;
		node.inputId = 0;
		node.outputId = nextId++;
		node.nodeName = "Entry";

		ed::Config config;
		settingFile = controller->GetResourcePath() + ".meta";
		config.SettingsFile = settingFile.c_str();
		context = ed::CreateEditor(&config);
		ed::SetCurrentEditor(context);
		auto& editorStyle = ed::GetStyle();
		editorStyle.FlowDuration = 0.2f;


		std::unordered_map<int32_t, Node> nodeMap;
		for (auto& node : controller->GetAnimatorNodes())
		{
			if (node.first == AnimationController::EntryNodeId)
			{
				nodeMap[AnimationController::EntryNodeId] = nodes[0];
				continue;
			}
			auto& n = nodes.emplace_back();
			n.id = node.first;
			n.inputId = node.second.in;
			n.outputId = node.second.out;
			n.nodeName = StringUtils::GetFileNameWithoutExtension(node.second.name);
			n.animClip = node.second.animClip;
			nodeMap[n.id] = n;
			nextId = std::max<int32_t>(nextId, n.outputId.Get());
		}

		for (auto& link : controller->GetLinkInfo())
		{
			auto& fromNode = nodeMap[link.second.from];
			auto& toNode = nodeMap[link.second.to];

			links.push_back({
					ed::LinkId(link.first),
					ed::PinId(fromNode.outputId),
					ed::PinId(toNode.inputId)
				});
			nextId = std::max<int32_t>(nextId, link.first);
		}
		nextId++;
	}

	void NodeWindow::OpenController(const std::shared_ptr<AnimationController> & controller)
	{
		if (context != nullptr)
			ed::DestroyEditor(context);

		this->controller = controller;
		LoadMeta();
	}

	void NodeWindow::SaveWorkspace()
	{
		if (controller) 
		{
			ed::SetCurrentEditor(context);
			ed::SaveSettings();
			Engine::GetModule<ResourceManager>()->SaveResource(controller);
		}
	}

	void NodeWindow::LoadAnimation(const std::string& file)
	{
		auto clip = Engine::GetModule<ResourceManager>()->GetResource<AnimationClip>(file);

		if (clip != nullptr)
		{
			AddNode(clip);
		}
	}

	void NodeWindow::HandleQuery()
	{
		// Handle creation action, returns true if editor want to create new object (node or link)
		auto& editor = static_cast<Editor&>(Engine::Get());
		if (ed::BeginCreate() && editor.GetEditorState() != EditorState::Play)
		{

			ed::PinId inputPinId, outputPinId;
			if (ed::QueryNewLink(&inputPinId, &outputPinId))
			{
				// QueryNewLink returns true if editor want to create new link between pins.
				//
				// Link can be created only for two valid pins, it is up to you to
				// validate if connection make sense. Editor is happy to make any.
				//
				// Link always goes from input to output. User may choose to drag
				// link from output pin or input pin. This determine which pin ids
				// are valid and which are not:
				//   * input valid, output invalid - user started to drag new ling from input pin
				//   * input invalid, output valid - user started to drag new ling from output pin
				//   * input valid, output valid   - user dragged link over other pin, can be validated

				if (inputPinId && outputPinId) // both are valid, let's accept link
				{
					// ed::AcceptNewItem() return true when user release mouse button.
					if (ed::AcceptNewItem())
					{
						LOGV("InputID {0} , OutputID {1} ", inputPinId.Get(), outputPinId.Get());
						if (CreateLink(inputPinId.Get(), outputPinId.Get())) 
						{
							// Since we accepted new link, lets add one to our list of links.
				
							// Draw new link.
							ed::Link(links.back().id, links.back().inputId, links.back().outputId);
						}
					}
					// You may choose to reject connection between these nodes
					// by calling ed::RejectNewItem(). This will allow editor to give
					// visual feedback by changing link thickness and color.
				}
			}
		}
		ed::EndCreate(); // Wraps up object creation action handling.
	}

	bool NodeWindow::CreateLink(int32_t from, int32_t to)
	{
		if(to != 101)
		{
			Node* fromNode = FindNode(from);
			Node* toNode = FindNode(to);
			if (controller && fromNode && toNode)
			{
				controller->Connect(
					fromNode->animClip,
					fromNode->id,
					fromNode->inputId.Get(),
					fromNode->outputId.Get(),
					toNode->animClip,
					toNode->id,
					toNode->inputId.Get(),
					toNode->outputId.Get(),
					nextId
				);
				links.push_back({ ed::LinkId(nextId++), ed::PinId(fromNode->outputId),  ed::PinId(toNode->inputId) });
				return true;
			}
		}
		return false;
	}

	NodeWindow::Node* NodeWindow::FindNode(int32_t id)
	{
		for (auto & node : nodes)
		{
			/*if(node.id == 100){
				continue;
			}*/
			if (node.inputId.Get() == id || node.outputId.Get() == id) {
				return &node;
			}
		}
		return nullptr;
	}

	void NodeWindow::HandleDelete()
	{
		auto& editor = static_cast<Editor&>(Engine::Get());
		// Handle deletion action
		if (ed::BeginDelete() && editor.GetEditorState() != EditorState::Play)
		{
			// There may be many links marked for deletion, let's loop over them.
			ed::LinkId deletedLinkId;
			while (ed::QueryDeletedLink(&deletedLinkId))
			{
				// If you agree that link can be deleted, accept deletion.
				if (ed::AcceptDeletedItem())
				{
					// Then remove link from your data.
					for (auto it = links.begin();it != links.end();it++)
					{
						if (it->id == deletedLinkId)
						{
							if (controller) {
								controller->RemoveLink(it->id.Get());
							}
							links.erase(it);
							break;
						}
					}
				}
				// You may reject link deletion by calling:
				// ed::RejectDeletedItem();
			}
		}
		ed::EndDelete(); // Wrap up deletion action
	}

	void NodeWindow::DrawNode()
	{
		for (auto & node : nodes)
		{

			const float rounding = 5.0f;
			const float padding = 12.0f;

			const auto pinBackground = ed::GetStyle().Colors[ed::StyleColor_NodeBg];

			ed::PushStyleColor(ed::StyleColor_NodeBg, ImColor(128, 128, 128, 200));
			ed::PushStyleColor(ed::StyleColor_NodeBorder, ImColor(32, 32, 32, 200));
			ed::PushStyleColor(ed::StyleColor_PinRect, ImColor(60, 180, 255, 150));
			ed::PushStyleColor(ed::StyleColor_PinRectBorder, ImColor(60, 180, 255, 150));

			ed::PushStyleVar(ed::StyleVar_NodePadding, ImVec4(0, 0, 0, 0));
			ed::PushStyleVar(ed::StyleVar_NodeRounding, rounding);
			ed::PushStyleVar(ed::StyleVar_SourceDirection, ImVec2(0.0f, 1.0f));
			ed::PushStyleVar(ed::StyleVar_TargetDirection, ImVec2(0.0f, -1.0f));
			ed::PushStyleVar(ed::StyleVar_LinkStrength, 0.0f);
			ed::PushStyleVar(ed::StyleVar_PinBorderWidth, 1.0f);
			ed::PushStyleVar(ed::StyleVar_PinRadius, 5.0f);
			ed::BeginNode(node.id);

			ImGui::BeginVertical(&node.id);
			ImGui::BeginHorizontal("inputs");
			ImGui::Spring(0, padding * 2);

			ImRect inputsRect;
			int inputAlpha = 200;
			if (!node.inputId.Invalid)
			{
				ImGui::Dummy(ImVec2(0, padding));
				ImGui::Spring(1, 0);
				inputsRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax() + ImVec2{ 1,1 });

				ed::PushStyleVar(ed::StyleVar_PinArrowSize, 10.0f);
				ed::PushStyleVar(ed::StyleVar_PinArrowWidth, 10.0f);
				ed::PushStyleVar(ed::StyleVar_PinCorners, 12);
				ed::BeginPin(node.inputId, ed::PinKind::Input);
				ed::PinPivotRect(inputsRect.GetTL(), inputsRect.GetBR());
				ed::PinRect(inputsRect.GetTL(), inputsRect.GetBR());
				ed::EndPin();
				ed::PopStyleVar(3);

				/*if (newLinkPin && !CanCreateLink(newLinkPin, &pin) && &pin != newLinkPin)
					inputAlpha = (int)(255 * ImGui::GetStyle().Alpha * (48.0f / 255.0f));*/
			}
			else 
			{
				ImGui::Dummy(ImVec2(0, padding));
			}

			ImGui::Spring(0, padding * 2);
			ImGui::EndHorizontal();

			ImGui::BeginHorizontal("content_frame");
			ImGui::Spring(1, padding);

			ImGui::BeginVertical("content", ImVec2(0.0f, 0.0f));
			ImGui::Dummy(ImVec2(160, 0));
			ImGui::Spring(1);
			ImGui::TextUnformatted(node.nodeName.c_str());
			auto& editor = static_cast<Editor&>(Engine::Get());
			if (editor.GetEditorState() == EditorState::Play && 
				controller != nullptr && 
				controller->GetCurrentAnimationName() == node.nodeName) {

				auto& cur = controller->GetCurrAnimation();
				if (auto clip = cur->GetPlayingClip(); clip >= 0)
				{
					float time = cur->GetPlayingTime();
					float timeLength = cur->GetClipLength(clip);
					ImGui::ProgressBar(time / timeLength, { 100, 10 });
				}
			}

			ImGui::Spring(1);
			ImGui::EndVertical();
			auto contentRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());

			ImGui::Spring(1, padding);
			ImGui::EndHorizontal();

			ImGui::BeginHorizontal("outputs");
			ImGui::Spring(0, padding * 2);

			ImRect outputsRect;
			int outputAlpha = 200;
			if (!node.outputId.Invalid)
			{
				ImGui::Dummy(ImVec2(0, padding));
				ImGui::Spring(1, 0);
				outputsRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax() + ImVec2{ 1,1 });

				ed::PushStyleVar(ed::StyleVar_PinCorners, 3);
				ed::BeginPin(node.outputId, ed::PinKind::Output);
				ed::PinPivotRect(outputsRect.GetTL(), outputsRect.GetBR());
				ed::PinRect(outputsRect.GetTL(), outputsRect.GetBR());

				ed::EndPin();
				ed::PopStyleVar();

				/*if (newLinkPin && !CanCreateLink(newLinkPin, &pin) && &pin != newLinkPin)
					outputAlpha = (int)(255 * ImGui::GetStyle().Alpha * (48.0f / 255.0f));*/
			}
			else 
			{
				ImGui::Dummy(ImVec2(0, padding));
			//	node.outputIds.emplace_back(node.id);
			}

			ImGui::Spring(0, padding * 2);
			ImGui::EndHorizontal();

			ImGui::EndVertical();

			ed::EndNode();
			ed::PopStyleVar(7);
			ed::PopStyleColor(4);

			auto drawList = ed::GetNodeBackgroundDrawList(node.id);

			drawList->AddRectFilled(inputsRect.GetTL() + ImVec2(0, 1), inputsRect.GetBR(),
				IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), inputAlpha), 4.0f, 12);
			//ImGui::PushStyleVar(ImGuiStyleVar_AntiAliasFringeScale, 1.0f);
			drawList->AddRect(inputsRect.GetTL() + ImVec2(0, 1), inputsRect.GetBR(),
				IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), inputAlpha), 4.0f, 12);
			//ImGui::PopStyleVar();
			drawList->AddRectFilled(outputsRect.GetTL(), outputsRect.GetBR() - ImVec2(0, 1),
				IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), outputAlpha), 4.0f, 3);
			//ImGui::PushStyleVar(ImGuiStyleVar_AntiAliasFringeScale, 1.0f);
			drawList->AddRect(outputsRect.GetTL(), outputsRect.GetBR() - ImVec2(0, 1),
				IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), outputAlpha), 4.0f, 3);
			//ImGui::PopStyleVar();
			drawList->AddRectFilled(contentRect.GetTL(), contentRect.GetBR(), IM_COL32(24, 64, 128, 200), 0.0f);
			//ImGui::PushStyleVar(ImGuiStyleVar_AntiAliasFringeScale, 1.0f);
			drawList->AddRect(
				contentRect.GetTL(),
				contentRect.GetBR(),
				IM_COL32(48, 128, 255, 100), 0.0f);
		}
	}

	void NodeWindow::AddNode(Ptr<AnimationClip> animClip)
	{
		auto& node2 = nodes.emplace_back();
		node2.id = nextId++;
		node2.inputId = nextId++;
		node2.outputId = nextId++;
		node2.nodeName = animClip->GetName();
		node2.animClip = ResourceRef(animClip.get());
	}

	void NodeWindow::LeftPanel()
	{
		ImGui::BeginChild("##parameter window");

		auto colums = ImGui::GetColumnsCount();
		{
			ImGui::BeginChild("##directory_navigation_bar", ImVec2(ImGui::GetColumnWidth(), 50));
			
			ImGui::TextUnformatted("Parameters");
			ImGui::Separator();
			ImGui::TextUnformatted(ICON_MDI_MAGNIFY);

			ImGui::SameLine();

			filter.Draw("##Filter1", ImGui::GetContentRegionAvail().x - ImGui::GetStyle().IndentSpacing-50);

			ImGui::SameLine();

			ImGui::PushID("Saveaaaaaaaaaaaaaaaaaa");
			if (ImGui::SmallButton(ICON_MDI_CONTENT_SAVE))
			{
				SaveWorkspace();
			}
			ImGui::PopID();

			ImGui::SameLine();
			if (ImGui::Selectable(ICON_MDI_PLUS_BOX_OUTLINE))
			{
				ImGui::OpenPopup("Add Parameters Window");
			}

			if (ImGui::BeginPopup("Add Parameters Window"))
			{
				if (ImGui::Selectable("Int") && controller)
				{
					controller->AddCondition(Condition::Type::Int);
				}

				if (ImGui::Selectable("Bool") && controller)
				{
					controller->AddCondition(Condition::Type::Bool);
				}

				if (ImGui::Selectable("Float") && controller)
				{
					controller->AddCondition(Condition::Type::Float);
				}
				ImGui::EndPopup();
			}

			ImGui::EndChild();
		}



		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
		ImGui::Columns(2);
		ImGui::Separator();

		if (controller) 
		{
			for (auto& condition : controller->GetConditions())
			{
				if (selectParam != condition.first) 
				{
					ImGui::TextUnformatted(condition.first.c_str());
				}

				if (ImGui::IsMouseDoubleClicked(0) && ImGui::IsMouseHoveringRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax()))
				{
					selectParam = condition.first;
					doubleClick = true;
				}
				if (selectParam == condition.first) 
				{
					static char objName[256];
					strcpy(objName, condition.first.c_str());
					if (ImGui::InputText("###Name", objName, 256))
					{
						controller->ChangeConditionName(condition.first, objName);
						selectParam = objName;
						break;
					}
					if (!ImGui::IsMouseHoveringRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax())
						&& ImGui::IsMouseClicked(0) 
						)
					{
						if (!doubleClick)
						{
							selectParam = "";
						}
						doubleClick = false;
					}
				}

				ImGui::NextColumn();

				switch (condition.second.type)
				{
				case Condition::Type::Bool:
				{
					bool active = condition.second;
					if (ImGui::Checkbox(("##"+ condition.first).c_str(), &active))
					{
						controller->SetValue(condition.first, std::to_string(active));
					}
				}
				
					break;
				case Condition::Type::Float: 
				{
					float value = condition.second;
					if (ImGui::InputFloat(("##" + condition.first).c_str(), &value))
					{
						controller->SetValue(condition.first, std::to_string(value));
					}
				}
				
					break;
				case Condition::Type::Int:
				{
					int32_t value2 = condition.second;
					if (ImGui::InputInt(("##" + condition.first).c_str(), &value2))
					{
						controller->SetValue(condition.first, std::to_string(value2));
					}
				}
					break;
				}

				ImGui::SameLine();

				ImGui::PushID(("---" + condition.first).c_str());
				if (ImGui::Button("-" ICON_MDI_DELETE))
				{
					controller->RemoveCondition(condition.first);
					ImGui::PopID();
					break;
				}
				ImGui::PopID();
				ImGui::Separator();

				ImGui::NextColumn();
			}
		}

		ImGui::Columns(colums);
		ImGui::PopStyleVar();

		ImGui::EndChild();
	}

	void NodeWindow::RightPanel()
	{
		ImGui::BeginChild("##animation editor");

		auto& editor = static_cast<Editor&>(Engine::Get());

		if (controller )
		{
			ed::Begin("Animator Node");
			DrawNode();
			// Submit Links
			for (auto& linkInfo : links)
			{
				ed::Link(linkInfo.id, linkInfo.inputId, linkInfo.outputId);
			}

			HandleQuery();
			HandleDelete();
		
			ed::End();


			if (auto link = ed::GetDoubleClickedLink()) 
			{
				ed::SelectLink(link);
			}



			std::vector<ed::LinkId> selectedLinks;
			selectedLinks.resize(ed::GetSelectedObjectCount());
			int linkCount = ed::GetSelectedLinks(selectedLinks.data(), static_cast<int>(selectedLinks.size()));
			if (linkCount == 1 && controller)
			{
				auto& link = controller->GetTransition(selectedLinks[0].Get());
				controller->FocusTransition(&link);
				editor.GetWindow<PropertiesWindow>()->SetController(controller);
			}
			else if (controller)
			{
				controller->FocusTransition(nullptr);
				editor.GetWindow<PropertiesWindow>()->SetController(nullptr);
			}
			else
			{
				editor.GetWindow<PropertiesWindow>()->SetController(nullptr);
			}
			if (controller && Engine::Get().GetEditorState() == EditorState::Play)
			{
				ed::Flow(controller->GetCurrentLink());
			}
		}

		ImGui::EndChild();
	}

};

