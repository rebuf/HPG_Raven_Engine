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


#include "glm/matrix.hpp"
#include <vector>



namespace Raven
{
	class RenderScene;
	class RenderPrimitive;
	class RenderMesh;
	class RenderSkinnedMesh;


	// RenderPrimitiveCollector:
	//    - Used to collect render primitives.
	//
	class RenderPrimitiveCollector
	{
		// Friend...
		friend class RenderScene;

	public:
		// Construct.
		RenderPrimitiveCollector(RenderScene* sceneOwner);

		// Create new render primitive for collection.
		RenderMesh* NewMesh();

		// Create new render primitive for collection.
		RenderSkinnedMesh* NewSkinnedMesh();

		// Return the view distance to the primitive.
		inline float GetViewDistance() { return viewDist; }

	private:
		// Clear all collected primitives form the collector.
		void Reset();

		// Set current collector transform.
		inline void SetTransform(const glm::mat4* world, const glm::mat4* normal)
		{
			worldMatrix = world;
			normalMatrix = normal;
		}

	private:
		// The Render Scene.
		RenderScene* owner;

		// The new primitives collected.
		std::vector<RenderPrimitive*> primitive;

		// Current World Matrix to set the new render primitives 
		const glm::mat4* worldMatrix;

		// Current Normal Matrix to set the new render primitives 
		const glm::mat4* normalMatrix;

		// The distance from the view to the primitive, can be used for LODs.
		float viewDist;
	};

}


