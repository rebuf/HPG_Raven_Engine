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



#include "RenderPrimitive.h"




namespace Raven
{
	class RenderRscMesh;
	

	//	RenderMesh:
	//			- Represent a Mesh while rendering.
	class RenderMesh : public RenderPrimitive
	{	
	public:
		// Construct.
		RenderMesh();

		// Destruct.
		~RenderMesh();

		// Return the primitive resource.
		virtual RenderRscPrimitive* GetRsc() override;

		// Set the mesh to be drawn.
		void SetMesh(RenderRscMesh* inMesh);

		// Draw Mesh.
		void Draw(GLShader* shader, bool isShadow) const override;

		// Return Expected Domain of this primitive.
		inline virtual ERenderShaderDomain GetDomain() { return ERenderShaderDomain::Mesh; }

	private:
		// The Mesh Render Resource.
		RenderRscMesh* mesh;
	};

}
