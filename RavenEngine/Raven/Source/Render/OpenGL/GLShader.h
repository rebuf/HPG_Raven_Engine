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





#include "GLTypes.h"
#include "GLBuffer.h"


#include "glm/vec4.hpp"
#include "glm/vec3.hpp"
#include "glm/vec2.hpp"
#include "glm/matrix.hpp"


#include <map>
#include <string>
#include <vector>
#include <set>






namespace Raven
{
	// Code for a specific shader stage, could be code or file.
	struct GLShaderCode
	{
		// The shader stages this code belongs to.
		EGLShaderStageBit stages;

		// The source or a path to the shader source file.
		std::string src;

		// True if the src is a path to a srouce file.
		bool isFile;
	};







	// GLShader:
	//
	class GLShader
	{
	private:
		// Construct. 
		GLShader();

	public:
		// Destruct.
		~GLShader();

		// Return true if the shader has a valid program.
		inline bool IsValid() { return id != 0; }

		// Return the program id of this shader.
		inline GLUINT GetID() { return id; }

		// Create a GLShader.
		static GLShader* Create(const std::string& name);

		// Set the main source for a specific shader stage.
		void SetSource(EGLShaderStage stage, const std::string& src);
		void SetSourceFile(EGLShaderStage stage, const std::string& src);

		// Add extra code to the shader. 
		void AddExSource(uint32_t sortTag, EGLShaderStageBit stages, const std::string& src);
		void AddExSourceFile(uint32_t sortTag, EGLShaderStageBit stages, const std::string& src);

		// Remove the main source of a specific shader stage.
		void RemoveSource(EGLShaderStage stage);

		// Remove extra code previously added. 
		void RemoveExSource(uint32_t sortTag);

		// Add preprocessor definition to the shader sources.
		void AddPreprocessor(const std::string& def);

		// Remove previously preprocessor definition.
		void RemovePreprocessor(const std::string& def);

		// Build Shader - OpenGL Program.
		bool Build();

		// Use this shader's program. Make it the current one.
		void Use();

		// Convert a stage type to matching string name.
		static std::string ToString(EGLShaderStage stage);

	private:
		// Convert a stage to its corresponding stage bit.
		static EGLShaderStageBit ToStageBit(EGLShaderStage stage);

		// Return true if the shader bit has a specific stage in it.
		inline static bool HasStage(EGLShaderStageBit stages, EGLShaderStage stage)
		{
			auto stageBit = ToStageBit(stage);
			return (stages & stageBit) == stageBit;
		}

	public:
		// Set a Uniform value.
		// Note: This assume that this program is the current one.
		void SetUniform(const std::string& name, const float& value);
		void SetUniform(const std::string& name, const int& value);
		void SetUniform(const std::string& name, const glm::vec2& value);
		void SetUniform(const std::string& name, const glm::vec3& value);
		void SetUniform(const std::string& name, const glm::vec4& value);
		void SetUniform(const std::string& name, const glm::mat4& value);
		void SetUniform(const std::string& name, const glm::mat3& value);

		// Bind Uniform buffer to this shader uniform block.
		// Note: This assume that this program is the current one.
		void BindUniformBlock(const std::string& blockName, int binding);

	private:
		// Build a shader stage
		GLUINT BuildStage(EGLShaderStage stage);

		// Load all the source of a shader stage.
		bool LoadSource(EGLShaderStage stage, std::string& loadedSrc);

		// Read source form file.
		std::string ReadSrcFile(const std::string& file);

	private:
		// The OpenGL Program ID.
		GLUINT id;

		// shader name, useful for debugging.
		std::string name;

		// the shader source for a specific stage.
		std::map<EGLShaderStage, GLShaderCode> source;

		// Extra code that will be embedded into the shader.
		std::map<uint32_t, GLShaderCode> exSource;

		// Preprocessor that will be added at the beggining of the main source file
		// for example "#define COMPUTE_MATERIAL 0"
		std::set<std::string> preprocessor;

	};

}








