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
#include "RenderRscShader.h"
#include "Utilities/Core.h"

#include "Render/OpenGL/GLShader.h"




#define IMPORT_START_SORT_TAG 1000 
#define IMPORT_COMMON_TAG 0
#define IMPORT_TRANSFORM_VERTEX_TAG 1 
#define IMPORT_MATERIAL_FUNCTION_BASE_TAG 2 
#define IMPORT_COMMON_LIGHTING_TAG 4 
#define IMPORT_COMMON_SHADOW_TAG 5
#define IMPORT_LIGHTING_TAG 6 
#define IMPORT_SHADOW_TAG 7










namespace Raven {




RenderRscShader::RenderRscShader()
	: domain(ERenderShaderDomain::Custom)
	, type(ERenderShaderType::Opaque)
	, renderBatchIndex((uint32_t)-1)
	, isShadow(false)
	, isTwoSidedShader(false)
{

}


RenderRscShader::~RenderRscShader()
{

}


RenderRscShader* RenderRscShader::Create(ERenderShaderDomain domain, const RenderRscShaderCreateData& data)
{
	RenderRscShader* rsc = new RenderRscShader();
	rsc->domain = domain;
	rsc->type = data.type;
	rsc->shader = std::shared_ptr<GLShader>( GLShader::Create(data.name) );
	rsc->isShadow = data.isShadow;
	rsc->isTwoSidedShader = data.isTwoSided;

	// Default Imports...
	rsc->shader->AddExSourceFile(IMPORT_COMMON_TAG, EGLShaderStageBit::All, "shaders/Common.glsl");

	// Setup the shaders for their domain and type
	rsc->SetupShaderForDomain();
	rsc->SetupShaderForType();

	// Materail Function Override.
	EGLShaderStageBit funcStages = data.materialFunction.first;

	if (funcStages != EGLShaderStageBit::None)
	{
		// Override Vertex Function?
		if ((funcStages & EGLShaderStageBit::VertexBit) == EGLShaderStageBit::VertexBit)
			rsc->shader->AddPreprocessor("#define MATERIAL_VERTEX_OVERRIDE 1");

		// Override Fragment Function?
		if ((funcStages & EGLShaderStageBit::FragmentBit) == EGLShaderStageBit::FragmentBit)
			rsc->shader->AddPreprocessor("#define MATERIAL_FUNCTION_OVERRIDE 1");

		// Add The Function
		rsc->shader->AddExSourceFile(IMPORT_START_SORT_TAG + 1,
			data.materialFunction.first, data.materialFunction.second);
	}

	// Build OpenGL Shader/Program.
	rsc->shader->Build();

	// Success?
	if (rsc->shader->IsValid())
	{
		rsc->shader->Use();
		return rsc;
	}

	// Failed...
	delete rsc;
	return nullptr;
}


RenderRscShader* RenderRscShader::CreateCustom(const RenderRscShaderDomainCreateData& domain, const RenderRscShaderCreateData& data)
{
	RenderRscShader* rsc = new RenderRscShader();
	rsc->domain = ERenderShaderDomain::Custom;
	rsc->type = data.type;
	rsc->shader = std::shared_ptr<GLShader>(GLShader::Create(data.name));
	rsc->isShadow = data.isShadow;
	rsc->isTwoSidedShader = data.isTwoSided;

	// Default Imports...
	rsc->shader->AddExSourceFile(0, EGLShaderStageBit::All, "shaders/Common.glsl");

	// Add all improts as external source...
	for (uint32_t i = 0; i < domain.imports.size(); ++i)
	{
		rsc->shader->AddExSourceFile(IMPORT_START_SORT_TAG + i,
			domain.imports[i].first, domain.imports[i].second);
	}

	// Add all Preprocessors
	for (const auto& pre : domain.preprocessors)
	{
		rsc->shader->AddPreprocessor(pre);
	}

	// Custom Domain Main Source
	for (const auto& src : domain.sources)
	{
		rsc->shader->SetSourceFile(src.first, src.second);
	}


	// ...
	rsc->shader->AddPreprocessor("#define RENDER_SHADER_CUSTOM 1");


	// Build OpenGL Shader/Program.
	rsc->shader->Build();

	// Success?
	if (rsc->shader->IsValid())
	{
		rsc->shader->Use();
		return rsc;
	}

	// Failed...
	delete rsc;
	return nullptr;
}


void RenderRscShader::SetupShaderForDomain()
{
	switch (domain)
	{
	case ERenderShaderDomain::Mesh:
	case ERenderShaderDomain::MeshInstance:
	{
		shader->AddExSourceFile(IMPORT_TRANSFORM_VERTEX_TAG,
			EGLShaderStageBit::VertexBit,
			"shaders/VertexTransform.glsl");

		shader->AddExSourceFile(IMPORT_MATERIAL_FUNCTION_BASE_TAG,
			EGLShaderStageBit::VertexBit | EGLShaderStageBit::FragmentBit,
			"shaders/Materials/MaterialFunctions.glsl");


		if (domain == ERenderShaderDomain::MeshInstance)
		{
			shader->AddPreprocessor("#define RENDER_SHADER_MESH_INSTANCE 1");
		}
		
		// Main Source...
		shader->SetSourceFile(EGLShaderStage::Vertex, "shaders/MeshVert.glsl");
		shader->SetSourceFile(EGLShaderStage::Fragment, "shaders/MeshFrag.glsl");

		// Add Input...
		input.AddBlockInput(RenderShaderInput::CommonBlock);
		input.AddBlockInput(RenderShaderInput::TransformBlock);
	}
		break;


	case ERenderShaderDomain::Terrain:
	{
		RAVEN_ASSERT(type == ERenderShaderType::Opaque, "Terrain can only be Opaque.");

		shader->AddExSourceFile(IMPORT_MATERIAL_FUNCTION_BASE_TAG,
			EGLShaderStageBit::VertexBit | EGLShaderStageBit::FragmentBit,
			"shaders/Materials/MaterialFunctions.glsl");

		shader->SetSourceFile(EGLShaderStage::Vertex, "shaders/TerrainVert.glsl");
		shader->SetSourceFile(EGLShaderStage::Fragment, "shaders/TerrainFrag.glsl");
		shader->SetSourceFile(EGLShaderStage::TessControl, "shaders/TerrainTessellation.glsl");
		shader->SetSourceFile(EGLShaderStage::TessEvaluation, "shaders/TerrainTessellation.glsl");

		// Add Input...
		input.AddBlockInput(RenderShaderInput::CommonBlock);
		input.AddBlockInput(RenderShaderInput::TerrainBinBlock);
		input.AddSamplerInput("inHeightMap");
	}
		break;


	case ERenderShaderDomain::Skinned:
	{
		shader->AddExSourceFile(IMPORT_TRANSFORM_VERTEX_TAG,
			EGLShaderStageBit::VertexBit,
			"shaders/VertexTransform.glsl");

		shader->AddExSourceFile(IMPORT_MATERIAL_FUNCTION_BASE_TAG,
			EGLShaderStageBit::VertexBit | EGLShaderStageBit::FragmentBit,
			"shaders/Materials/MaterialFunctions.glsl");

		// Max Bones..
		shader->AddPreprocessor("#define RENDER_BONE_TRANSFORM ");
		shader->AddPreprocessor("#define RENDER_SKINNED_MAX_BONES " + std::to_string(RENDER_SKINNED_MAX_BONES));

		// Main Source...
		shader->SetSourceFile(EGLShaderStage::Vertex, "shaders/SkeletonVert.glsl");
		shader->SetSourceFile(EGLShaderStage::Fragment, "shaders/MeshFrag.glsl");

		// Add Input...
		input.AddBlockInput(RenderShaderInput::CommonBlock);
		input.AddBlockInput(RenderShaderInput::TransformBoneBlock);
	}
		break;
	}


	if (isShadow)
	{
		shader->AddExSourceFile(IMPORT_SHADOW_TAG, EGLShaderStageBit::All,
			"shaders/CommonShadow.glsl");

		shader->AddPreprocessor("#define RENDER_SHADER_TYPE_SHADOW 1");
		shader->SetSourceFile(EGLShaderStage::Fragment, "shaders/ShadowFrag.glsl");
		input.AddBlockInput(RenderShaderInput::ShadowBlock);
	}

}


void RenderRscShader::SetupShaderForType()
{
	switch (type)
	{
	case ERenderShaderType::Opaque:
	{
		shader->AddPreprocessor("#define RENDER_PASS_DEFERRED 1");
		shader->AddPreprocessor("#define RENDER_SHADER_TYPE_OPAQUE 1");
	}
		break;

	case ERenderShaderType::Masked:
	case ERenderShaderType::MaskedFoliage:
	{
		shader->AddPreprocessor("#define RENDER_PASS_DEFERRED 1");
		shader->AddPreprocessor("#define RENDER_SHADER_TYPE_MASKED 1");

		if (type == ERenderShaderType::MaskedFoliage)
		{
			shader->AddPreprocessor("#define RENDER_SHADER_TYPE_MASKED_FOLIAGE 1");
		}
	}
		break;

	case ERenderShaderType::Translucent:
	{
		shader->AddPreprocessor("#define RENDER_PASS_FORWARD 1");
		shader->AddPreprocessor("#define RENDER_SHADER_TYPE_TRANSLUCENT 1");

		// Lighting Shader.
		shader->AddExSourceFile(IMPORT_COMMON_LIGHTING_TAG, EGLShaderStageBit::FragmentBit,
			"shaders/CommonLight.glsl");

		shader->AddExSourceFile(IMPORT_COMMON_SHADOW_TAG, EGLShaderStageBit::FragmentBit,
			"shaders/CommonShadow.glsl");

		shader->AddExSourceFile(IMPORT_LIGHTING_TAG, EGLShaderStageBit::FragmentBit,
			"shaders/Lighting.glsl");


		shader->AddPreprocessor("#define MAX_SHADOW_CASCADE " + std::to_string(RENDER_MAX_SHADOW_CASCADE));
		shader->AddPreprocessor("#define MAX_LIGHTS " + std::to_string(RENDER_PASS_FORWARD_MAX_LIGHTS));
		input.AddBlockInput(RenderShaderInput::LightingBlock_FORWARD);
		input.AddBlockInput(RenderShaderInput::LightShadowBlock);
		input.AddSamplerInput("inSkyEnvironment");
		input.AddSamplerInput("inEnvBRDF");

		// Add sun shadow samplers...
		for (int32_t i = 0; i < RENDER_MAX_SHADOW_CASCADE; ++i)
		{
			input.AddSamplerInput("inSunShadow[" + std::to_string(i) + "]");
		}
	}
		break;

	case ERenderShaderType::PostProcessing:
	{
		RAVEN_ASSERT(0, "Shader PostProcessing type not supported yet.");
		shader->AddPreprocessor("#define RENDER_SHADER_TYPE_POSTPROCESSING 1");
	}
		break;
	}

}


void RenderRscShader::BindBlockInputs()
{
	for (uint32_t i = 0; i < input.GetNumBlocks(); ++i)
	{
		const auto& block = input.GetBlockInput(i);
		shader->BindUniformBlock(block.name, block.binding);
	}
}


void RenderRscShader::BindSamplers()
{
	shader->Use();

	for (uint32_t i = 0; i < input.GetNumSamplers(); ++i)
	{
		const auto& samplers = input.GetSamplerInput(i);
		shader->SetUniform(samplers.name, static_cast<int32_t>(i));
	}
}


bool RenderRscShader::IsTwoSided()
{
	return type == ERenderShaderType::MaskedFoliage || isTwoSidedShader;
}



} // End of namespace Raven.
