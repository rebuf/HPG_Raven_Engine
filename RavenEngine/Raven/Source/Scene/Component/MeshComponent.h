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

//////////////////////////////////////////////////////////////////////////////
// This file is part of the Raven Game Engine			                    //
//////////////////////////////////////////////////////////////////////////////

#pragma once


#include "Utilities/Core.h"
#include "PrimitiveComponent.h"
#include "ResourceManager/Resources/Mesh.h"



#include <string>
#include <vector>
#include <memory>





namespace Raven
{



	// MeshComponent:
	//	- 
	//
	class MeshComponent : public PrimitiveComponent
	{
	public:
		// Construct.
		MeshComponent();

		// Destruct.
		virtual ~MeshComponent();

		// Retun the list mesh in the model.
		inline auto& GetMesh() { return mesh; };
		inline const auto& GetMesh() const { return mesh; };

		// Set a new mesh to the list of meshes in the model.
		void SetMesh(Ptr<Mesh> newMesh);

		// Called by the render to collect render primitives for drawing.
		virtual void CollectRenderPrimitives(RenderPrimitiveCollector& rcollector) override;

	public:
		// serialization load and save
		template<typename Archive>
		void save(Archive& archive) const
		{
			archive(cereal::base_class<PrimitiveComponent>(this));

			// Save Resrouce Reference -> Mesh.
			ResourceRef::Save(archive, mesh.get());
		}

		template<typename Archive>
		void load(Archive& archive)
		{
			archive(cereal::base_class<PrimitiveComponent>(this));

			// Load Resrouce Reference -> Mesh.
			Ptr<Mesh> meshRef = ResourceRef::Load(archive).FindOrLoad<Mesh>();
			SetMesh(meshRef);
		}

	private:
		// The model mesh, each mesh section is mapped to a material.
		Ptr<Mesh> mesh;

	};

};