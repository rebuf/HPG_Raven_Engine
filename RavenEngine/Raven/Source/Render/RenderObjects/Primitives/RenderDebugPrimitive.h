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

#include "glm/vec4.hpp"



namespace Raven
{
	class RenderRscPrimitive;
	class RenderRscMaterial;
	class RenderRscDebugMesh;



	// RenderDebugPrimitive:
	//		- Render primitive for debug derawing.
	//		
	class RenderDebugPrimitive : public RenderPrimitive
	{
	public:
		// Construct. 
		RenderDebugPrimitive();

		// Destruct. 
		virtual ~RenderDebugPrimitive();

		// Return the primitive resource.
		virtual RenderRscPrimitive* GetRsc() override;

		// Set color.
		inline void SetColor(const glm::vec4& value) { color = value; }

		// Return color.
		inline const glm::vec4& GetColor() { return color; }

		//
		void SetDebugMesh(RenderRscDebugMesh* debugMesh);

		//
		virtual void Draw(GLShader* shader, bool isShadow) const override;

		// Return Expected Domain of this primitive.
		inline virtual ERenderShaderDomain GetDomain() { return ERenderShaderDomain::Custom; }

	private:
		// The Color.
		glm::vec4 color;

		//
		RenderRscDebugMesh* mesh;
	};


}


