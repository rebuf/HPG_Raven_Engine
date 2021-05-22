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
#include "UniformBuffer.h"
#include "Render/OpenGL/GLBuffer.h"





namespace Raven {


UniformBuffer::UniformBuffer()
	: buffer(nullptr)
	, cpuData(nullptr)
{

}


UniformBuffer::~UniformBuffer()
{
	delete buffer;
	delete cpuData;
}


UniformBuffer* UniformBuffer::Create(const RSInputBlockDescription& block, bool isCreateCPU)
{
	RAVEN_ASSERT(block.size != -1, "UniformBuffer - Invalid Block Description.");

	UniformBuffer* newUniformBuffer = new UniformBuffer();
	newUniformBuffer->blockDescription = block;
	newUniformBuffer->buffer = GLBuffer::Create(EGLBufferType::Uniform, block.size, EGLBufferUsage::DynamicDraw);

	if (isCreateCPU)
	{
		newUniformBuffer->cpuData = new uint8_t[block.size];
	}

	return newUniformBuffer;
}


void UniformBuffer::UpdateData(int32_t size, int32_t offset, const void* data)
{
	buffer->Bind();
	buffer->UpdateSubData(size, offset, data);
}


void UniformBuffer::BindBase()
{
	RAVEN_ASSERT(blockDescription.binding != -1, "UniformBuffer - Invalid binding.");
	buffer->BindBase(blockDescription.binding);
}


int32_t UniformBuffer::GetInputIndex(const std::string name) const
{
	for (int32_t i = 0; i < blockDescription.inputs.size(); ++i)
	{
		if (blockDescription.inputs[i].first.name == name)
			return i;
	}

	return -1;
}


void UniformBuffer::SetData(int32_t offset, int32_t size, const void* data)
{
	RAVEN_ASSERT(HasCPUBuffer(), "This Buffer has no data on cpu.");
	memcpy(cpuData + offset, data, (size_t)size);
}


void UniformBuffer::Update()
{
	RAVEN_ASSERT(HasCPUBuffer(), "This Buffer has no data on cpu.");
	buffer->Bind();
	buffer->UpdateSubData(blockDescription.size, 0, cpuData);
}


void UniformBuffer::SetDataValues(int32_t index, const std::vector<glm::vec4>& values)
{
	const auto& input = GetInput(index);
	int32_t offset = GetInputOffset(index);
	int32_t size = RenderShaderInput::GetSize(input.inputType);
	RAVEN_ASSERT(input.count > 1, "Invalid Operation - not an array.");
	RAVEN_ASSERT(sizeof(glm::vec4) == size, "Invalid DataType - Size does not match.");

	size = size * input.count;
	SetData(offset, size, values.data());
}


} // End of namespace Raven.


