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





#include "Utilities/Core.h"
#include "Render/RenderResource/IRenderResource.h"
#include "RenderShaderInput.h"
#include "Render/OpenGL/GLTypes.h"


#include <memory>
#include <string>
#include <vector>
#include <map>
#include <set>




namespace Raven
{
	class GLShader;
	class GLBuffer;
	class GLTexture;


	// 
	//
	//
	enum class ERenderShaderDomain
	{
		// The Shaders for rendering a mesh.
		Mesh,

		// The Shaders for rendering a terrain.
		Terrain,

		// The Shaders for rendering a skinned mesh.
		Skinned,

		// The Shaders for custom post-processing.
		PostProcessing,

		// Custom shader domain, defined and managed outside of the domain system.
		Custom,

		// The Shaders for rendering a mesh.
		MeshInstance
	};




	// 
	enum class ERenderShaderType
	{
		// The shader render opaque type objects hand has no alpha/transparency.
		Opaque,
		
		// The shader render opaque type objects with some masking operation.
		Masked,

		// The shader render translucent objects with alpha/transparency.
		Translucent,

		// Post-Processing type shader operation.
		PostProcessing,

		// The shader render two-sided masked foliage.
		MaskedFoliage
	};




	// Data for creating a shader render resoruce.
	class RenderRscShaderCreateData
	{
	public:
		// The type of the shader.
		ERenderShaderType type;

		// The name of the shader.
		std::string name;

		// Is this shader used for shadow rendering.
		bool isShadow;

		// Is this shader used for shadow rendering.
		bool isTwoSided;

		// The functions to override for a specific stage, what functions to override depneds on the type and domain.
		std::pair<EGLShaderStageBit, std::string> materialFunction;

		// Add Material Functions, the stage determine what function to override. @see MaterialFunctions.glsl
		inline void AddFunction(EGLShaderStageBit stages, const std::string& src)
		{
			materialFunction.first = stages;
			materialFunction.second = src;
		}

		
	public:
		// Default Construct.
		RenderRscShaderCreateData()
			: type(ERenderShaderType::Opaque)
			, isShadow(false)
			, isTwoSided(false)
		{
			materialFunction.first = EGLShaderStageBit::None;
		}

	};




	// Data for creating a shader render resoruce with custom domain.
	struct RenderRscShaderDomainCreateData
	{
		// Other shaders to import.
		std::vector< std::pair<EGLShaderStageBit, std::string> > imports;

		// preprocessor.
		std::set<std::string> preprocessors;

		// The main shader source of the domain.
		std::map<EGLShaderStage, std::string> sources;

		// Add domain main source code.
		inline void AddSource(EGLShaderStage stage, const std::string& src)
		{
			sources[stage] = src;
		}

		// Add Preprocessors to the shader.
		inline void AddPreprocessor(const std::string& pre)
		{
			preprocessors.insert(pre);
		}

		// Add sources to be imported.
		inline void AddImport(EGLShaderStageBit stages, const std::string& src)
		{
			std::pair<EGLShaderStageBit, std::string> imp;
			imp.first = stages;
			imp.second = src;
			imports.push_back(imp);
		}
	};





	// RenderRscShader:
	//		- Encapsulate a single shader as a render resource.
	//
	class RenderRscShader : public IRenderResource
	{
		// Private Construct, use Create Function.
		RenderRscShader();

	public:
		// Destruct.
		~RenderRscShader();

		// Creat a shader with one of the pre-defined domains.
		static RenderRscShader* Create(ERenderShaderDomain domain, const RenderRscShaderCreateData& data);

		// Creat a shader with a custom domain.
		static RenderRscShader* CreateCustom(const RenderRscShaderDomainCreateData& domain, const RenderRscShaderCreateData& data);

		// Return the OpenGL shader.
		inline GLShader* GetShader() { return shader.get(); }
		
		// Return the shader domain.
		inline ERenderShaderDomain GetDomain() { return domain; }

		// Return the shader type.
		inline ERenderShaderType GetType() { return type; }

		// Return this shader input.
		inline RenderShaderInput& GetInput() { return input; }
		inline const RenderShaderInput& GetInput() const { return input; }

		// Bind Unifrom Inputs to their binding.
		void BindBlockInputs();

		// Bind Samplers Input to their index.
		void BindSamplers();

		// Is this shader render two sided without face culling.
		bool IsTwoSided();

	private:
		// Setup the shader for the current domain.
		void SetupShaderForDomain();

		// Setup the shader for the current type.
		void SetupShaderForType();

	private:
		// The domain this shader is in.
		ERenderShaderDomain domain;

		// The type this shader draw.
		ERenderShaderType type;

		// The Shader.
		std::shared_ptr<GLShader> shader;

		// The input defined by the shader.
		RenderShaderInput input;

		// Is this shader for shadow rendering.
		bool isShadow;

		// Is this shader render twosided fron/back without face culling.
		bool isTwoSidedShader;

	public:
		// Render Batch Use Only, index set by the render batch to performs fast mapping during batching.
		mutable uint32_t renderBatchIndex;
	};

}


