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
#include "RenderSkinnedMesh.h"
#include "Render/RenderResource/Primitives/RenderRscMesh.h"


#include "Render/OpenGL/GLVertexArray.h"
#include "GL/glew.h"



namespace Raven {



RenderSkinnedMesh::RenderSkinnedMesh()
	: mesh(nullptr)
{
	isSkinned = true;
}


RenderSkinnedMesh::~RenderSkinnedMesh()
{

}


void RenderSkinnedMesh::SetMesh(RenderRscSkinnedMesh* inMesh)
{
	mesh = inMesh;
}


RenderRscPrimitive* RenderSkinnedMesh::GetRsc()
{
	return mesh;
}


void RenderSkinnedMesh::Draw(GLShader* shader, bool isShadow) const
{
	mesh->GetArray()->Bind();
	glDrawElements(GL_TRIANGLES, mesh->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
}


} // End of namespace Raven.
