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
#include "RenderGrid.h"
#include "Render/RenderResource/Shader/RenderRscShader.h"

#include "Render/OpenGL/GLVertexArray.h"
#include "Render/OpenGL/GLBuffer.h"
#include "Render/OpenGL/GLShader.h"


#include "GL/glew.h"
#include "glm/common.hpp"
#include "glm/gtc/constants.hpp"



namespace Raven {


#define GRID_BLOCK_SIZE 10.0f
#define GRID_SIZE 1000.0f
#define GRID_STEP 5.0f





RenderGrid::RenderGrid()
	: vxArray(nullptr)
	, vxBuffer(nullptr)
{

}


RenderGrid::~RenderGrid()
{
	delete vxArray;
	delete vxBuffer;
}


void RenderGrid::MakeGrid(std::vector<glm::vec3>& outVerts)
{
	for (float f = -GRID_SIZE; f <= GRID_SIZE; f += GRID_STEP)
	{
		glm::vec3 Lx0 = glm::vec3(-GRID_SIZE, 0.0f, f);
		glm::vec3 Lx1 = glm::vec3( GRID_SIZE, 0.0f, f);

		glm::vec3 Lz0 = glm::vec3(f, 0.0f,-GRID_SIZE );
		glm::vec3 Lz1 = glm::vec3(f, 0.0f, GRID_SIZE );

		//
		outVerts.push_back(Lx0);
		outVerts.push_back(Lx1);

		outVerts.push_back(Lz0);
		outVerts.push_back(Lz1);
	}		

}			

			
RenderGrid* RenderGrid::Create()
{
	RenderGrid* grid = new RenderGrid();

	std::vector<glm::vec3> verts;
	std::vector<uint32_t> indices;
	MakeGrid(verts);

	grid->numVerts = verts.size();
	grid->vxBuffer = GLBuffer::Create(
		EGLBufferType::Array,
		(int)(verts.size() * sizeof(glm::vec3)),
		verts.data(),
		EGLBufferUsage::StaticDraw
	);

	std::vector<GLVABuildAttribData> attributes{
		// Attribute 0 - Position
		GLVABuildAttribData(
			grid->vxBuffer,        // Buffers
			0,                     // Index
			3,                     // Type-Size
			EGLTypes::Float,       // Type
			sizeof(glm::vec3),     // Stride
			0                      // offset
		)
	};


	grid->vxArray = GLVertexArray::Create();
	grid->vxArray->Build(attributes, nullptr);



	// Grid Shader
	{
		RenderRscShaderDomainCreateData shaderDomainData;
		shaderDomainData.AddSource(EGLShaderStage::Vertex, "shaders/GridVert.glsl");
		shaderDomainData.AddSource(EGLShaderStage::Fragment, "shaders/GridFrag.glsl");

		//shaderDomainData.AddSource(EGLShaderStage::Geometry, "shaders/GridGeom.glsl");
		//shaderDomainData.AddPreprocessor("#define GEOEMTRY_SHADER_LINES");


		// Shader Type Data
		RenderRscShaderCreateData shaderData;
		shaderData.type = ERenderShaderType::PostProcessing;
		shaderData.name = "Grid_Shader";

		grid->shader = Ptr<RenderRscShader>(RenderRscShader::CreateCustom(shaderDomainData, shaderData));
		grid->shader->GetInput().AddBlockInput(RenderShaderInput::CommonBlock);
		grid->shader->BindBlockInputs();
		grid->shader->BindSamplers();
		
	}

	return grid;
}


void RenderGrid::Draw()
{
	shader->GetShader()->Use();
	glLineWidth(1.5f);

	vxArray->Bind();
	glDrawArrays(GL_LINES, 0, numVerts);
}




} // End of namespace Raven.
