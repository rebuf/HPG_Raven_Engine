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
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"


#include <list>
#include <array>




namespace Raven
{
	class IRenderResource;
	class RenderPrimitive;
	class RenderRscDebugMesh;
	class RenderRscShader;
	class RenderRscMaterial;



	// Debug Draw Data.
	struct RenderDebugDraw
	{
		//
		float time;

		// Has this debug draw been drawn before.
		bool isDrawn;

		//
		RenderPrimitive* primitive;

		//
		RenderDebugDraw()
			: time(0.0f)
			, primitive(nullptr)
			, isDrawn(false)
		{

		}

		//
		~RenderDebugDraw();
	};



	// RenderDebug 
	//		- provide debugging utilities for the render.
	//		- you can get instance to it from the render module.
	class RenderDebug
	{
		// Friend Render Module...
		friend class RenderModule;

	private:
		// Private Construct, managed by render module.
		RenderDebug();

		// Setup Debug Render.
		void Setup();

		// Deatroy/Cleanup Debug Render.
		void Destroy();

		// Called by the render module to update each frame. 
		void Update(float dt);

		// Render the render debug primitives that need to be draw this frame. 
		const std::vector<RenderPrimitive*>& GetRenderPrimitive() const;

	public:
		// Destruct.
		~RenderDebug();

		// Draw a box at position with a scale. If time is 0.0 then draw for a single frame only.
		void DrawBox(const glm::vec3& position, const glm::vec3& scale, 
			glm::vec4 color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), float time = 0.0f);

	private:
		// Hold debug render resources.
		std::array<RenderRscDebugMesh*, 3> resources;

		//
		std::list<RenderDebugDraw> debugDrawList;

		//
		std::vector<RenderPrimitive*> debugPrimitives;

		// Shader for debug drawing.
		RenderRscShader* debugShader;

		//
		RenderRscMaterial* debugMaterial;
	};

}




