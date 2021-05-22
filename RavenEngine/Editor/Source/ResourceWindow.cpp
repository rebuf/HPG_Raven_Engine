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
#include "ResourceWindow.h"
#include "Utilities/StringUtils.h"
#include "Window/Window.h"

#include "Engine.h"
#include "ResourceManager/ResourceManager.h"

// Resources...
#include "ResourceManager/Resources/Texture2D.h"
#include "ResourceManager/Resources/Mesh.h"
#include "ResourceManager/Resources/SkinnedMesh.h"
#include "ResourceManager/Resources/Material.h"
#include "ResourceManager/Resources/MaterialShader.h"


#include "Render/RenderResource/RenderRscTexture.h"
#include "Render/RenderResource/Shader/RenderRscMaterial.h"
#include "Render/RenderResource/Shader/RenderRscShader.h"
#include "Render/OpenGL/GLTexture.h"


#include <imgui_internal.h>
#include <imgui.h>
#include <imgui/ImGuiHelpers.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>






namespace Raven 
{


ResourceWindow::ResourceWindow()
{
	title = "Resource Window";
	type = RT_None;

}


ResourceWindow::~ResourceWindow()
{

}


void ResourceWindow::OnImGui()
{
	bool close = ImGui::Begin(title.c_str(), &active);

	// Display Resource Properties.
	if (resource)
	{
		ImGui::Text("Type: %s", ResourceToString(type).c_str());
		ImGui::Text("Path: %s", resource->GetResourcePath().c_str());
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();

		switch (type)
		{
		case Raven::RT_Texture2D:
			OnImGuiTexture2D();
			break;

		case Raven::RT_MaterialShader:
			OnImGuiShader();
			break;

		case Raven::RT_Material:
			OnImGuiMaterial();
			break;

		case Raven::RT_Mesh:
			OnImGuiMesh();
			break;

		case Raven::RT_SkinnedMesh:
			OnImGuiSkinned();
			break;
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		// Save Button for saving resources.
		if (ImGui::Button("Save Resource"))
		{
			// Remove from pending save.
			if (Engine::GetModule<ResourceManager>()->IsPendingSave(resource))
			{
				Engine::GetModule<ResourceManager>()->RemovePendingSave(resource);
			}

			// Save...
			Engine::GetModule<ResourceManager>()->SaveResource(resource);
		}
	}



	ImGui::End();
}


void ResourceWindow::SetResource(Ptr<IResource> rsc)
{
	type = rsc->GetType();
	resource = rsc;
}


void ResourceWindow::SetResource(const std::string& rscPath)
{
	type = Engine::GetModule<ResourceManager>()->GetResourceType(rscPath);

	// Supported Types...
	//   - Get/Load from resource manager.
	switch (type)
	{
	case Raven::RT_Texture2D:
		resource = Engine::GetModule<ResourceManager>()->GetResource<Texture2D>(rscPath);
		break;

	case Raven::RT_MaterialShader:
		resource = Engine::GetModule<ResourceManager>()->GetResource<MaterialShader>(rscPath);
		break;

	case Raven::RT_Material:
		resource = Engine::GetModule<ResourceManager>()->GetResource<Material>(rscPath);
		break;

	case Raven::RT_Mesh:
		resource = Engine::GetModule<ResourceManager>()->GetResource<Mesh>(rscPath);
		break;

	case Raven::RT_SkinnedMesh:
		resource = Engine::GetModule<ResourceManager>()->GetResource<SkinnedMesh>(rscPath);
		break;
	}


	// Edit Not Supported?
	if (!resource)
	{
		type = RT_None;
		return;
	}

}


void ResourceWindow::OnImGuiMesh()
{
	Mesh* rscMesh = static_cast<Mesh*>(resource.get());
	bool isModified = false;
	//-- --- -- -




	MeshLOD& mesh = rscMesh->GetMeshLOD(0);

	for (size_t i = 0; i < mesh.sections.size(); ++i)
	{

		auto& section  = mesh.sections[i];

		ImGui::PushID(500 + i);

		ImGui::Separator();
		ImGui::Text("Section(%d)", i);

		ImGui::Columns(2);
		std::string path = section->defaultMaterial.GetPath();
		ImGuiHelper::Property("Default Material", path);

		std::string drop = ImGuiDragDropAssetsTarget();
		if (!drop.empty())
		{
			if (Engine::GetModule<ResourceManager>()->GetResourceType(drop) == RT_Material)
			{
				Ptr<Material> mat = Engine::GetModule<ResourceManager>()->GetResource<Material>(drop);
				section->defaultMaterial = ResourceRef(mat.get());
				section->defaultMaterial.FindOrLoad<Material>();
				isModified = true;
			}
		}

		ImGui::Columns(1);
		ImGui::PopID();
	}





	if (isModified)
	{
		Engine::GetModule<ResourceManager>()->AddPendingSave(resource);
	}
}


void ResourceWindow::OnImGuiSkinned()
{
	SkinnedMesh* rscMesh = static_cast<SkinnedMesh*>(resource.get());
	bool isModified = false;
	//-- --- -- -




	auto& sections = rscMesh->GetMeshSections();

	for (size_t i = 0; i < sections.size(); ++i)
	{

		auto& section = sections[i];

		ImGui::PushID(500 + i);

		ImGui::Separator();
		ImGui::Text("Section(%d)", i);

		ImGui::Columns(2);
		std::string path = section->defaultMaterial.GetPath();
		ImGuiHelper::Property("Default Material", path);

		std::string drop = ImGuiDragDropAssetsTarget();
		if (!drop.empty())
		{
			if (Engine::GetModule<ResourceManager>()->GetResourceType(drop) == RT_Material)
			{
				Ptr<Material> mat = Engine::GetModule<ResourceManager>()->GetResource<Material>(drop);
				section->defaultMaterial = ResourceRef(mat.get());
				section->defaultMaterial.FindOrLoad<Material>();
				isModified = true;
			}
		}

		ImGui::Columns(1);
		ImGui::PopID();
	}





	if (isModified)
	{
		Engine::GetModule<ResourceManager>()->AddPendingSave(resource);
	}
}


void ResourceWindow::OnImGuiTexture2D()
{
	Texture2D* rscTex = static_cast<Texture2D*>(resource.get());
	bool isModified = false;
	//-- --- -- -

	ImGui::Text("Size: %d, %d", rscTex->GetSize().x, rscTex->GetSize().y);

	//

	// Filter.
	{
		static const char* types[3] = { "Nearest", "Linear", "TriLinear" };

		ImGui::Text("Filter: ");
		ImGui::SameLine();
		if (ImGui::BeginMenu(types[static_cast<int>(rscTex->GetFilter())]))
		{
			if (ImGui::MenuItem("Nearest")) { rscTex->SetFitler(ETextureFilter::Nearest); isModified = true; }
			if (ImGui::MenuItem("Linear")) { rscTex->SetFitler(ETextureFilter::Linear); isModified = true; }
			if (ImGui::MenuItem("TriLinear")) { rscTex->SetFitler(ETextureFilter::TriLinear); isModified = true; }

			ImGui::EndMenu();
		}
	}
	

	// Wrap.
	{

		static const char* types[3] = { "Repeat", "Mirror", "ClampToEdge" };

		ImGui::Text("Wrap: ");
		ImGui::SameLine();
		if (ImGui::BeginMenu( types[static_cast<int>(rscTex->GetWrap())]) )
		{
			if (ImGui::MenuItem("Repeat")) { rscTex->SetWrap(ETextureWrap::Repeat); isModified = true; }
			if (ImGui::MenuItem("Mirror")) { rscTex->SetWrap(ETextureWrap::Mirror); isModified = true; }
			if (ImGui::MenuItem("ClampToEdge")) { rscTex->SetWrap(ETextureWrap::ClampToEdge); isModified = true; }

			ImGui::EndMenu();
		}
	}


	if (rscTex->GetRenderRsc())
	{
		GLTexture* texGL = rscTex->GetRenderRsc()->GetTexture().get();

		ImVec2 size(256, 256);
		size.x *= (float)rscTex->GetSize().x / (float)rscTex->GetSize().y;
		ImGui::SetCursorPosX((ImGui::GetWindowSize().x - size.x) * 0.5f);
		ImGui::Image((ImTextureID)texGL->GetID(), size);
	}


	if (isModified)
	{
		rscTex->UpdateParamters();
		Engine::GetModule<ResourceManager>()->AddPendingSave(resource);
	}
}


void ResourceWindow::OnImGuiShader()
{
	MaterialShader* rscShader = static_cast<MaterialShader*>(resource.get());
	bool isModified = false;
	//-- --- -- -


	// Not Loaded or Invalid?
	if (!rscShader->IsOnGPU())
	{
		ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "Invalid Shader - Fix Errors & Build Shader.");
	}
	else
	{
		ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.5f, 1.0f), "Shader Built Successfully.");
	}


	// Build/Rebuild.
	{
		if (ImGui::Button("Build Shader"))
		{
			if (!rscShader->IsOnGPU())
				rscShader->LoadRenderResource();
			else
				rscShader->UpdateRenderResource();
		}

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();
	}


	// Material Function.
	{
		std::string matFunction = rscShader->GetMaterialFunction();
		bool computeVertex = rscShader->HasComputeVertex();
		bool newFuncValues = false;

		std::string editTextValue;
		editTextValue.append(matFunction);
		editTextValue.resize(250);

		ImGui::Text("Material Function:");

		if (ImGui::InputText("###materialfunction", editTextValue.data(), editTextValue.size()))
		{
			matFunction = editTextValue.c_str();
			newFuncValues = true;
		}

		// Drag and Drop shader file...
		std::string shaderAsset = ImGuiDragDropAssetsTarget();
		if (StringUtils::IsTextFile(shaderAsset))
		{
			matFunction = shaderAsset;
			newFuncValues = true;
		}

		if (ImGui::Checkbox("Has Compute Vertex Function.", &computeVertex))
		{
			newFuncValues = true;
		}


		if (newFuncValues)
		{
			rscShader->SetMaterialFunction(matFunction, computeVertex);
			isModified = true;
		}

	}

	ImGui::Separator();


	// Domain.
	{
		static const char* types[6] = { "Mesh", "Terrain", "Skinned", nullptr, "Custom", "MeshInstance" };

		ImGui::Text("Domain: ");
		ImGui::SameLine();
		ImGui::PushItemWidth(140);

		auto currentItem = static_cast<int>(rscShader->GetDomain());

		if (ImGui::BeginCombo("###domaintype", types[currentItem]))
		{
			for (int32_t i = 0; i < 6; ++i)
			{
				if (!types[i])
					continue;

				bool isSlected = (i == currentItem);

				if (ImGui::Selectable(types[i], &isSlected))
				{
					rscShader->SetDomain(static_cast<ERenderShaderDomain>(i));
					isModified = true;
				}
				
			}

			ImGui::EndCombo();
		}

		ImGui::PopItemWidth();
	}


	// Type.
	{
		static const char* types[5] = { "Opaque", "Masked", "Translucent", nullptr, "MaskedFoliage" };

		ImGui::Text("Type: ");
		ImGui::SameLine();
		ImGui::PushItemWidth(150);

		auto currentItem = static_cast<int>(rscShader->GetShaderType());

		if (ImGui::BeginCombo("###shadertype", types[currentItem]))
		{
			for (int32_t i = 0; i < 5; ++i)
			{
				if (!types[i])
					continue;

				bool isSlected = (i == currentItem);

				if (ImGui::Selectable(types[i], &isSlected))
				{
					rscShader->SetShaderType(static_cast<ERenderShaderType>(i));
					isModified = true;
				}
			}

			ImGui::EndCombo();
		}

		ImGui::PopItemWidth();
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();


	// Block Input
	{
		ImGui::PushID(224);
		ImGui::Text("Shader Input Block: ");
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Text("Block Name:");
		ImGui::SameLine();

		{
			std::string editTextValue;
			editTextValue.append(rscShader->GetBlockInput().name);
			editTextValue.resize(260);

			if (ImGui::InputText("###blockname", editTextValue.data(), editTextValue.size()))
			{
				rscShader->GetBlockInput().name = editTextValue;
				isModified = true;
			}
		}


		// Block Input Name
		if (!rscShader->GetBlockInput().name.empty())
		{
			RSInputDescription editInput;
			bool rm = false;
			bool editParam = false;

			for (size_t i = 0; i < rscShader->GetBlockInput().inputs.size(); ++i)
			{
				const auto& input = rscShader->GetBlockInput().inputs[i];
				bool editParam = ImGuiShaderInputUI(i, input.first, editInput, rm);

				if (editParam)
				{
					rscShader->GetBlockInput() = ReCreateBlock(rscShader->GetBlockInput(), i, editInput, rm, false);
					break;
				}
			}



			// Add Input.
			if (ImGui::Button(" + "))
			{
				RSInputDescription newInput;
				newInput.name = "DEFAULT_NAME";
				newInput.inputType = EShaderInputType::Float;
				newInput.flag = ESInputDefaultFlag::White;
				rscShader->GetBlockInput() = ReCreateBlock(rscShader->GetBlockInput(), -1, newInput, false, true);
				isModified = true;
			}
		}
		ImGui::PopID();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::PushID(314);
		ImGui::Text("Shader Samplers: ");
		ImGui::Spacing();
		ImGui::Spacing();

		if (!rscShader->GetSamplers().empty())
		{
			RSInputDescription editInput;
			bool rm = false;
			bool editParam = false;

			for (size_t i = 0; i < rscShader->GetSamplers().size(); ++i)
			{
				const auto& input = rscShader->GetSamplers()[i];
				editParam = ImGuiShaderInputUI(i, input, editInput, rm);

				if (editParam)
				{
					if (rm)
					{
						rscShader->RemoveSampler(i);
					}
					else
					{
						rscShader->GetSamplers()[i].name = editInput.name;
						rscShader->GetSamplers()[i].flag = editInput.flag;
					}

					isModified = true;
					break;
				}
			}

		}

		// Add Samplers.
		if (ImGui::Button(" + "))
		{
			rscShader->AddSampler("DEFAULT_NAME", ESInputDefaultFlag::White);
			isModified = true;
		}

		ImGui::Spacing();
	}

	ImGui::PopID();



	if (isModified)
	{
		Engine::GetModule<ResourceManager>()->AddPendingSave(resource);
	}
}


void ResourceWindow::OnImGuiMaterial()
{
	Material* rscMat = static_cast<Material*>(resource.get());
	bool isModified = false;
	MaterialShader* rscShader = rscMat->GetMaterialShader();
	// -- ----- ----- ------ -- - - - - -

	if (!rscShader)
	{
		ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.3f, 1.0f), "Material Shader not found.");
	}
	else
	{
		if (!rscShader->IsOnGPU())
		{
			ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.3f, 1.0f), "Material Shader is not built or has errors.");
		}
		else
		{
			ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.3f, 1.0f), "Material Shader is valid.");
		}
	}


	{
		ImGui::Text("Shader Path:");
		ImGui::SameLine();

		if (!rscShader)
		{
			ImGui::InputText("", "", 0);
		}
		else
		{
			std::string editTextValue;
			editTextValue.append(rscShader->GetResourcePath());
			editTextValue.resize(260);
			ImGui::InputText("", editTextValue.data(), editTextValue.size());
		}

		// Drag and Drop shader file...
		std::string shaderAsset = ImGuiDragDropAssetsTarget();
		if (Engine::GetModule<ResourceManager>()->GetResourceType(shaderAsset) == RT_MaterialShader)
		{
			Ptr<MaterialShader> shader = Engine::GetModule<ResourceManager>()->GetResource<MaterialShader>(shaderAsset);
			rscMat->SetMaterialShader(shader);
			rscMat->ClearAllParamters();
			rscShader = rscMat->GetMaterialShader();
			isModified = true;
		}
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();


	// Invalid Material...
	if (!rscMat->IsOnGPU())
	{
		if (!rscShader || !rscShader->IsOnGPU())
			return;

		rscMat->LoadRenderResource();
	}


	// Block Input Paramters.
	if (rscShader->GetBlockInput().binding != -1)
	{
		for (size_t i = 0; i < rscShader->GetBlockInput().inputs.size(); ++i)
		{
			const auto& input = rscShader->GetBlockInput().inputs[i].first;

			ImGui::PushID(500 + i);
			

			switch (input.inputType)
			{
			case Raven::EShaderInputType::Float:
			{
				ImGui::Text("%s:", input.name.c_str());

				float value = 0.0f;
				if (!rscMat->GetScalar(input.name, value))
				{
					// Make sure the paramter is set...
					rscMat->LoadDefaulFromShader(input.name);
					rscMat->GetScalar(input.name, value);
				}

				if (ImGui::SliderFloat("###scalarinput", &value, 0.0f, 1.0f))
				{
					rscMat->SetScalar(input.name, value);
					isModified = true;
				}
			}
				break;
			case Raven::EShaderInputType::Vec4:
			{
				ImGui::Text("%s:", input.name.c_str());

				glm::vec4 color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
				if (!rscMat->GetColor(input.name, color))
				{
					// Make sure the paramter is set...
					rscMat->LoadDefaulFromShader(input.name);
					rscMat->GetColor(input.name, color);
				}

				if (ImGui::ColorEdit4("###colorinput", glm::value_ptr(color), 0.05f))
				{
					rscMat->SetColor(input.name, color);
					isModified = true;
				}
			}
				break;
			}

			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::PopID();
		}
	}


	// Input Samplers.
	if (rscShader->GetSamplers().size() > 0)
	{
		for (size_t i = 0; i < rscShader->GetSamplers().size(); ++i)
		{
			const auto& sampler = rscShader->GetSamplers()[i];

			Ptr<ITexture> texture;
			rscMat->GetTexture(sampler.name, texture);

			ImGui::PushID(500 + i);
			ImGui::Text("%s:", sampler.name.c_str());
			
			if (!texture)
			{
				ImGui::InputText("", "", 0);
			}
			else
			{
				std::string editTextValue;
				editTextValue.append(texture->GetResourcePath());
				editTextValue.resize(260);
				ImGui::InputText("", editTextValue.data(), editTextValue.size());
			}

			// Drag and Drop texture file...
			std::string textureAsset = ImGuiDragDropAssetsTarget();
			if (Engine::GetModule<ResourceManager>()->GetResourceType(textureAsset) == RT_Texture2D)
			{
				Ptr<Texture2D> texture2D = Engine::GetModule<ResourceManager>()->GetResource<Texture2D>(textureAsset);
				rscMat->SetTexture(sampler.name, texture2D);
				isModified = true;
			}
			else if (Engine::GetModule<ResourceManager>()->GetResourceType(textureAsset) == RT_TextureCube)
			{
				RAVEN_ASSERT(0, "TODO: Support Texture Cube.");
			}

			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::PopID();
		}
	}

	if (isModified)
	{
		Engine::GetModule<ResourceManager>()->AddPendingSave(resource);
	}
}



std::string ResourceWindow::ImGuiDragDropAssetsTarget()
{
	if (ImGui::BeginDragDropTarget())
	{
		auto data = ImGui::AcceptDragDropPayload("AssetFile");
		if (data)
		{
			std::string file = (char*)data->Data;
			return file;
		}

		ImGui::EndDragDropTarget();
	}

	return "";
}


bool ResourceWindow::ImGuiShaderInputUI(int32_t index, const RSInputDescription& input, RSInputDescription& outEdit, bool& outRm)
{
	bool isModified = false;
	outEdit = input;
	outRm = false;
	// -- -- - - -- ---- - -- - 

	std::string label;
	ImGui::PushID(index+1);

	if (input.uniformType == EShaderUniformType::Sampler)
	{
		ImGui::Text("Sampler(%d):", index+1);
		label = "###sampler" + std::to_string(index);
	}
	else
	{
		ImGui::Text("Input(%d):", index+1);
		label = "###inputname" + std::to_string(index);
	}



	if (outEdit.uniformType != EShaderUniformType::Sampler)
	{
		ImGui::PushItemWidth(100);
		ImGui::SameLine();
		static const char* types[8] = { "Unknown", "Scalar", "Int", "Vec2", "Vec3", "Color", "Mat3", "Mat4" };
		int currentItem = static_cast<int>(outEdit.inputType);

		if (ImGui::BeginCombo("###uniformType", types[currentItem]))
		{
			for (int32_t i = 0; i < 8; ++i)
			{
				bool isSlected = (i == currentItem);

				if (ImGui::Selectable(types[i], &isSlected))
				{
					outEdit.inputType = static_cast<EShaderInputType>(i);
					isModified = true;
				}
			}


			ImGui::EndCombo();
		}
		ImGui::PopItemWidth();
	}


	{
		ImGui::PushItemWidth(72);
		ImGui::SameLine();
		static const char* types[3] = { "White", "Black", "Normal"};
		int currentItem = static_cast<int>(outEdit.flag);

		if (ImGui::BeginCombo("###defaultInput", types[currentItem]))
		{
			for (int32_t i = 0; i < 3; ++i)
			{
				bool isSlected = (i == currentItem);

				if (ImGui::Selectable(types[i], &isSlected))
				{
					outEdit.flag = static_cast<ESInputDefaultFlag>(i);
					isModified = true;
				}
			}


			ImGui::EndCombo();
		}

		ImGui::PopItemWidth();
	}


	ImGui::SameLine();
	ImGui::PushItemWidth(270);

	outEdit.name.resize(260);
	if (ImGui::InputText(label.c_str(), outEdit.name.data(), outEdit.name.size()))
	{
		std::string tmp = outEdit.name.data();
		outEdit.name = tmp;
		isModified = true;
	}
	ImGui::PopItemWidth();




	ImGui::SameLine();
	if (ImGui::Button(" - "))
	{
		outRm = true;
		isModified = true;
	}

	ImGui::PopID();

	return isModified;
}


RSInputBlockDescription ResourceWindow::ReCreateBlock(const RSInputBlockDescription& block, int32_t editIndex, const RSInputDescription& edit, bool rm, bool isNew)
{
	RSInputBlockDescription editBlock;

	if (block.inputs.size() == 1 && rm)
	{
		editBlock.name = block.name;
		return editBlock;
	}

	editBlock.BeginUniformBlock(block.name);

	for (size_t i = 0; i < block.inputs.size(); ++i)
	{
		// Edit
		if (i == editIndex && !isNew)
		{
			if (rm)
				continue;

			editBlock.AddInput(edit.inputType, edit.name, edit.flag);
		}
		else
		{
			// Not Changed.
			if (block.inputs[i].first.count > 0)
			{
				editBlock.AddInput(block.inputs[i].first.inputType, block.inputs[i].first.name, block.inputs[i].first.flag);
			}
			else
			{
				editBlock.AddInputArray(block.inputs[i].first.inputType, block.inputs[i].first.name, block.inputs[i].first.count, block.inputs[i].first.flag);
			}
		}
	}

	if (isNew)
	{
		editBlock.AddInput(edit.inputType, edit.name, edit.flag);
	}

	editBlock.EndUniformBlock();
	return editBlock;
}


} // End of namespace Raven 


