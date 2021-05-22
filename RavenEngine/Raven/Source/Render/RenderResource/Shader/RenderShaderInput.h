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
#include "Render/OpenGL/GLTypes.h"


#include <vector>




#define RENDER_PASS_DEFERRED_MAX_LIGHTS 32
#define RENDER_PASS_FORWARD_MAX_LIGHTS 4
#define RENDER_SKINNED_MAX_BONES 82
#define RENDER_MAX_SHADOW_CASCADE 4





namespace Raven
{
	// Different unifrom types defined in shader.
	enum class EShaderUniformType
	{
		// Simple uniforms that is set directly using GLShader::SetUnfirom.
		Uniform,

		// Uniform block that is bound to uniform buffer.
		UniformBlock,

		// Texture Samplers.
		Sampler
	};



	// Different data types of shader input
	enum class EShaderInputType
	{
		Unknown,
		Float,
		Int,
		Vec2,
		Vec3,
		Vec4,
		Mat3,
		Mat4
	};


	// Flag used to know what to set values to if they are not provided by the material.
	enum class ESInputDefaultFlag
	{
		// White: Scalar->1.0, Color->WhiteColor, Texture->WhiteTexture.
		White,

		// Black: Scalar->0.0,  Color->BlackColor, Texture->BlackTexture.
		Black,

		// Normal: Texture->NormalTexture).
		Normal
	};


	// Render Shader Input.
	struct RSInputDescription
	{
		// Uniform type.
		EShaderUniformType uniformType;
		
		// Input Type.
		EShaderInputType inputType;

		// The input name.
		std::string name;

		// If greater than one, it means that this input is an array and this is the size.
		int32_t count;

		// Flag used to know what to set values to if they are not provided by the material.
		ESInputDefaultFlag flag;


		// Serialization Save.
		template<typename Archive>
		void save(Archive& archive) const
		{
			archive(
				name,
				count,
				EnumAsInt<const EShaderUniformType>(uniformType),
				EnumAsInt<const EShaderInputType>(inputType),
				EnumAsInt<const ESInputDefaultFlag>(flag)
			);

		}

		// Serialization Load.
		template<typename Archive>
		void load(Archive& archive)
		{
			archive(
				name,
				count,
				EnumAsInt<EShaderUniformType>(uniformType),
				EnumAsInt<EShaderInputType>(inputType),
				EnumAsInt<ESInputDefaultFlag>(flag)
			);

		}
	};


	// Render Shader Unifrom Block Input Description
	class RSInputBlockDescription
	{
	public:
		// The Block Name
		std::string name;

		// The overall size of the block.
		int32_t size;

		// The inputs and their offset inside the block
		std::vector< std::pair<RSInputDescription, int32_t> > inputs;

		// The binding used by this block.
		int32_t binding;

	public:
		// Construct.
		RSInputBlockDescription()
			: size(-1)
			, binding(-1)
		{

		}

		// Begin Defining a new input of a shader.
		void BeginUniformBlock(const std::string& name);

		// End Defining a input of a shader.
		void EndUniformBlock();

		// Specify the input in a unifrom block, if offset is -1 then it will compute the offset based on the previous input.
		// This is called between Begin/End uniform block.
		void AddInput(EShaderInputType inputType, const std::string& inputName, ESInputDefaultFlag flag = ESInputDefaultFlag::White, int32_t offset = -1);
		void AddInputArray(EShaderInputType inputType, const std::string& inputName, int32_t count, ESInputDefaultFlag flag = ESInputDefaultFlag::White, int32_t offset = -1);

		// Return the index of an input with that name.
		int32_t GetInputIndex(const std::string& name);


		// Serialization Save.
		template<typename Archive>
		void save(Archive& archive) const
		{
			archive(
				name,
				size,
				binding
			);


			uint32_t inputsCount = inputs.size();
			archive(inputsCount);
			for (uint32_t i = 0; i < inputsCount; ++i)
			{
				archive(inputs[i].first);
				archive(inputs[i].second);
			}

		}

		// Serialization Load.
		template<typename Archive>
		void load(Archive& archive)
		{
			archive(
				name,
				size,
				binding
			);


			uint32_t inputsCount = 0;
			archive(inputsCount);
			inputs.resize(inputsCount);
			for (uint32_t i = 0; i < inputsCount; ++i)
			{
				archive(inputs[i].first);
				archive(inputs[i].second);
			}
		}

	public:
		// Utility functions for making specific/global uniform blocks to a shader input.
		static RSInputBlockDescription MakeCommonBlock();
		static RSInputBlockDescription MakeTransformBlock();
		static RSInputBlockDescription MakeTransformBoneBlock();
		static RSInputBlockDescription MakeLightingBlock_DEFERRED();
		static RSInputBlockDescription MakeLightingBlock_FORWARD();
		static RSInputBlockDescription MakeTerrainBinBlock();
		static RSInputBlockDescription MakeShadowBlock();
		static RSInputBlockDescription LightShadowBlock();
	}; 



	// RenderShaderInput:
	//		- Provide an input description for a shader, used by systems that need to know
	//			what the shader expect.
	class RenderShaderInput
	{
	public:
		// Construct.
		RenderShaderInput();

		// Destruct.
		~RenderShaderInput();

		// Specify the input of a normal unifrom.
		void AddInput(EShaderInputType inputType, const std::string& name, ESInputDefaultFlag flag = ESInputDefaultFlag::White);

		// Specify the input of a sampler.
		void AddSamplerInput(const std::string& name, ESInputDefaultFlag flag = ESInputDefaultFlag::White);
		void AddSamplerInputs(const std::vector<RSInputDescription>& inSamplers);

		// Add unifrom block as input.
		void AddBlockInput(const RSInputBlockDescription& block);

		// Return the index of an input with that name.
		int32_t GetInputIndex(const std::string& name);

		// Return the index of an input sampler with that name.
		int32_t GetSamplerInputIndex(const std::string& name);

		// Return the index of an input uniform block with that name.
		int32_t GetBlockInputIndex(const std::string& name);

		// Return the input associated of that name.
		inline RSInputDescription& GetInput(int32_t idx) { return inputs[idx]; }
		inline const RSInputDescription& GetInput(int32_t idx) const { return inputs[idx]; }

		// Return the block input associated of that name.
		inline RSInputBlockDescription& GetBlockInput(int32_t idx) { return blockInputs[idx]; }
		inline const RSInputBlockDescription& GetBlockInput(int32_t idx) const { return blockInputs[idx]; }

		// Return the sampler input associated of that name.
		inline RSInputDescription& GetSamplerInput(int32_t idx) { return samplers[idx]; }
		inline const RSInputDescription& GetSamplerInput(int32_t idx) const { return samplers[idx]; }

		// Return the number of block inputs.
		inline uint32_t GetNumBlocks() { return static_cast<uint32_t>(blockInputs.size()); }

		// Return the number of samplers
		inline uint32_t GetNumSamplers() { return static_cast<uint32_t>(samplers.size()); }

	public:
		// Return the size of an input type.
		static int32_t GetSize(EShaderInputType type);

		// Return the memory alignment of an input type.
		static int32_t GetAlignment(EShaderInputType type);

		// Return the offset after alignment.
		static int32_t AlignOffset(int32_t offset, int32_t align);

	public:
		// Global Uniform Buffer Description...
		static RSInputBlockDescription CommonBlock;
		static RSInputBlockDescription TransformBlock;
		static RSInputBlockDescription TransformBoneBlock;
		static RSInputBlockDescription LightingBlock_DEFERRED;
		static RSInputBlockDescription LightingBlock_FORWARD;
		static RSInputBlockDescription TerrainBinBlock;
		static RSInputBlockDescription ShadowBlock;
		static RSInputBlockDescription LightShadowBlock;

		// The binding for the material shader unifrom block input.
		static int32_t MaterialBlockBinding;

	private:
		// Uniform block inputs.
		std::vector<RSInputBlockDescription> blockInputs;

		// Uniform input.
		std::vector<RSInputDescription> inputs;

		// Sampler input.
		std::vector<RSInputDescription> samplers;
	};

}
