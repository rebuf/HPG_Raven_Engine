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
#include "Render/RenderResource/Shader/RenderRscShader.h"

#include "glm/matrix.hpp"

#include <vector>



namespace Raven
{
	class RenderRscPrimitive;
	class RenderRscMaterial;
	class GLShader;



	// RenderPrimitive:
	//		- Primitive dynamic render data used by the render to draw a single primitive.
	//		
	class RenderPrimitive
	{
		// Friend...
		friend class RenderScene;

	public:
		// Construct. 
		RenderPrimitive();

		// Destruct. 
		virtual ~RenderPrimitive();

		// Return the primitive resource.
		virtual RenderRscPrimitive* GetRsc() = 0;

		// Return Expected Domain of this primitive.
		virtual ERenderShaderDomain GetDomain() = 0;

		// Return material.
		inline RenderRscMaterial* GetMaterial() { return material; }

		// Set Render Resrouce Material to be used by for drawing this primitive.
		void SetMaterial(RenderRscMaterial* mat);

		// Set the world transformation.
		void SetWorldMatrix(const glm::mat4& mtx);

		// Set the normals world transformation.
		void SetNormalMatrix(const glm::mat4& mtx);

		// Return world transformation.
		inline const glm::mat4& GetWorldMatrix() const { return worldMatrix; }

		// Draw ths Primitive.
		virtual void Draw(GLShader* shader, bool isShadow) const = 0;

		// Add a light that is going to lit this primitive.
		inline void AddLight(uint32_t lightIndex) { lights.push_back(lightIndex); }

		// Return the lighst that are going to lit this primitive.
		inline const std::vector<uint32_t>& GetLights() const { return lights; }

		// Render Shader Domain from the material set to this primitive.
		ERenderShaderDomain GetMaterialDomain() const;

		// Render Shader Type from the material set to this primitive.
		ERenderShaderType GetShaderType() const;

		// Return true if skinned mesh.
		inline bool IsSkinned() const { return isSkinned; }

		// Return true if this primitive cast shadow.
		bool IsCastShadow();

	private:
		// The material.
		RenderRscMaterial* material;

		// Primitive World Transform.
		glm::mat4 worldMatrix;

		// Primitive Normal Transform.
		glm::mat4 normalMatrix;

		// Lights that lit that is used to lit this primitive.
		std::vector<uint32_t> lights;

		// Used by RenderScene, the index of this primitive in the render scene.
		int32_t indexInScene;

	protected:
		// True if this is a skinned primitive.
		bool isSkinned;

	};


}


