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


#include "Render/RenderResource/Primitives/RenderRscPrimitive.h"
#include "Render/RenderResource/Shader/RenderRscShader.h"
#include "Render/RenderResource/Shader/RenderRscMaterial.h"
#include "Render/RenderObjects/Primitives/RenderPrimitive.h"




#include <cstdlib>
#include <vector>
#include <array>
#include <unordered_map>





namespace Raven
{


	// Types of Render Batches used for rendering different passes.
	enum class ERenderBatchType
	{
		// Batch that store primitives based on shaders & materials.
		Deferred,

		// Batch that store primitives based on distance to the view.
		Translucent,

		// Batch that store primitives based on their type.
		Shadow
	};



	// RenderBatch.
	template<ERenderBatchType type>
	class RenderBatch;






	// --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- 







	// RenderBatch<Deferred>
	//		- Store primitive for deferred rendering pass.
	//		- sort objects based material & shader.
	//
	template<>
	class RenderBatch<ERenderBatchType::Deferred>
	{
		// Invlaid Index for shader & materail batches.
		const uint32_t INVALID_BATCH_INDEX = (uint32_t)-1;

		// Represent a shader in this batch.
		struct ShaderBatch
		{
			// The index in the batch list.
			uint32_t index;

			// The shader.
			RenderRscShader* shader;

			// List of materials that is part of this shader.
			std::vector<uint32_t> materials;
		};


		// Represent a material in this batch.
		struct MaterialBatch
		{
			// The index in the batch list.
			uint32_t index;

			// The material.
			RenderRscMaterial* material;

			// List of primitives that use this material.
			std::vector<uint32_t> primitives;
		};


	public:
		// Construct.
		inline RenderBatch()
		{

		}

		// Rest/Clear the batch.
		inline void Reset()
		{
			// Clear Index.
			for (auto& elem : shaders)
				elem.shader->renderBatchIndex = INVALID_BATCH_INDEX;

			// Clear Index.
			for (auto& elem : materials)
				elem.material->renderBatchIndex = INVALID_BATCH_INDEX;

			shaders.clear();
			materials.clear();
			primitives.clear();
		}

		// Add new primitive to the batch to be drawn.
		inline void Add(RenderPrimitive* prim)
		{
			// Add to the list.
			uint32_t primIndex = primitives.size();
			primitives.push_back(prim);

			// Add to the material batch.
			MaterialBatch* matBatch = FindOrCreateMaterail(prim->GetMaterial());
			matBatch->primitives.push_back(primIndex);
		}

		// Return the number of shaders batches to render.
		inline uint32_t GetNumShaders() const { return shaders.size(); }

		// Return a shader batch to render.
		inline const ShaderBatch& GetShaderBatch(uint32_t index) const { return shaders[index]; }

		// Return a material batch to render.
		inline const MaterialBatch& GetMaterialBatch(uint32_t index) const { return materials[index]; }

		// Return all primitives in this entire batch.
		inline const std::vector<RenderPrimitive*>& GetPrimitives() const { return primitives; }

		// Return true if the batch has no primitives to draw.
		bool IsEmpty() { return primitives.empty(); }

	private:
		// Find if there is a batch created for the shader, if not create a new one.
		inline ShaderBatch* FindOrCreateShader(RenderRscShader* shader)
		{
			// Exist?
			if (shader->renderBatchIndex != INVALID_BATCH_INDEX)
			{
				return &shaders[shader->renderBatchIndex];
			}

			// New...
			ShaderBatch newShaderBatch;
			newShaderBatch.index = shaders.size();
			newShaderBatch.shader = shader;
			shaders.push_back(newShaderBatch);

			// Map...
			shader->renderBatchIndex = newShaderBatch.index;

			return &shaders[newShaderBatch.index];
		}

		// Find if there is a batch created for the material, if not create a new one.
		inline MaterialBatch* FindOrCreateMaterail(RenderRscMaterial* mat)
		{
			// Exist?
			if (mat->renderBatchIndex != INVALID_BATCH_INDEX)
			{
				return &materials[mat->renderBatchIndex];
			}

			// New...
			MaterialBatch newMatBatch;
			newMatBatch.index = materials.size();
			newMatBatch.material = mat;
			materials.push_back(newMatBatch);
	
			// Add to shader batch.
			ShaderBatch* shaderBatch = FindOrCreateShader(mat->GetShaderRsc());
			shaderBatch->materials.push_back(newMatBatch.index);

			// Map...
			mat->renderBatchIndex = newMatBatch.index;

			return &materials[newMatBatch.index];
		}

	private:
		// The shaders the primitives materials are part of.
		std::vector<ShaderBatch> shaders;

		// The materials the primitives are using.
		std::vector<MaterialBatch> materials;

		// The primitives this batch is drawing.
		std::vector<RenderPrimitive*> primitives;
	};




	// --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- 




	// RenderBatch<Deferred>
	//		- Store primitive for translucent rendering pass.
	//		- sort objects based distance from view.
	//
	template<>
	class RenderBatch<ERenderBatchType::Translucent>
	{
		// Primtive in this batch.
		struct PrimitiveElem
		{
			// The primitive to render.
			RenderPrimitive* primitive;

			// Distance from the view.
			float distance;
		};

	public:
		// Construct.
		inline RenderBatch()
		{

		}

		// Rest/Clear the batch.
		inline void Reset()
		{
			primitives.clear();
		}

		// Add new primitive to the batch to be drawn.
		inline void Add(RenderPrimitive* prim, float distance)
		{
			PrimitiveElem elem;
			elem.primitive = prim;
			elem.distance = distance;
			primitives.push_back(elem);
		}

		// Sort render primitives based on their distance from the view.
		inline void Sort()
		{
			// Quicksort...
			std::qsort(
				primitives.data(), 
				primitives.size(),
				sizeof(PrimitiveElem),
				&RenderBatch<ERenderBatchType::Translucent>::ComparePrimitiveElem
			);
		}

		// Return true if the batch has no primitives to draw.
		bool IsEmpty() { return primitives.empty(); }

		// Return all primitives in this batch.
		inline const std::vector<PrimitiveElem>& GetPrimitives() const { return primitives; }

	private:
		// Compare two primitive element based on distance.
		inline static int ComparePrimitiveElem(const void* p1, const void* p2)
		{
			const auto* e1 = static_cast<const PrimitiveElem*>(p1);
			const auto* e2 = static_cast<const PrimitiveElem*>(p2);

			return e1->distance > e2->distance ? -1 : 1;
		}


	private:
		// The primitives this batch is drawing.
		std::vector<PrimitiveElem> primitives;

	};


	// --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- 




	// RenderBatch<Default>
	//		- Store primitive for shadow map rendering, if the primitive have custom shadow shader
	//      store in a shader batch.
	// 
	template<>
	class RenderBatch<ERenderBatchType::Shadow>
	{
	public:
		// Represent a shader in this batch.
		struct ShaderBatch
		{
			// The index in the batch list.
			uint32_t index;

			// The shader.
			RenderRscShader* shader;

			// List of materials that is part of this shader.
			std::vector<uint32_t> materials;

			// The domain of the shadow shader, used to identify default shader types.
			ERenderShaderDomain domain;
		};


		// Represent a material in this batch.
		struct MaterialBatch
		{
			// The index in the batch list.
			uint32_t index;

			// The material.
			RenderRscMaterial* material;

			// List of primitives that use this material.
			std::vector<uint32_t> primitives;
		};


	public:
		// Construct.
		inline RenderBatch()
		{
			// Default shader from primitives that has no custom shader for shadows.
			SetupDefaultBatches();

		}

		// Rest/Clear the batch.
		inline void Reset()
		{
			matMap.clear();
			shaderMap.clear();
			primitives.clear();
			shaders.resize(3);
			materials.resize(3);

			for (size_t i = 0; i < 3; ++i)
			{
				materials[i].primitives.clear();
			}
		}

		// Add new primitive to the batch to be drawn.
		inline void Add(RenderPrimitive* prim, bool isDefault)
		{
			// Add to the list.
			uint32_t primIndex = primitives.size();
			primitives.push_back(prim);

			// Use default shader shaders?
			if (!prim->GetMaterial()->GetShadowShaderRsc() || isDefault)
			{
				switch (prim->GetDomain())
				{
				case Raven::ERenderShaderDomain::Mesh:
					materials[0].primitives.push_back(primIndex);
					break;
				case Raven::ERenderShaderDomain::Skinned:
					materials[1].primitives.push_back(primIndex);
					break;
				case Raven::ERenderShaderDomain::Terrain:
					materials[2].primitives.push_back(primIndex);
					break;
				}
			}
			else
			{
				// Add to the material batch.
				MaterialBatch* matBatch = FindOrCreateMaterail(prim->GetMaterial());
				matBatch->primitives.push_back(primIndex);
			}
		}

		// Return the number of shaders batches to render.
		inline uint32_t GetNumShaders() const { return shaders.size(); }

		// Return a shader batch to render.
		inline const ShaderBatch& GetShaderBatch(uint32_t index) const { return shaders[index]; }

		// Return a material batch to render.
		inline const MaterialBatch& GetMaterialBatch(uint32_t index) const { return materials[index]; }

		// Return all primitives in this entire batch.
		inline const std::vector<RenderPrimitive*>& GetPrimitives() const { return primitives; }

		// Return true if the batch has no primitives to draw.
		inline bool IsEmpty() { return primitives.empty(); }

		// Set deafult materail and its shader.
		inline void SetDefaultShader(uint32_t index, RenderRscShader* defaultShader, RenderRscMaterial* defaultMaterial)
		{
			shaders[index].shader = defaultShader;
			materials[index].material = defaultMaterial;
		}

	private:
		// Default shader from primitives that has no custom shader for shadows.
		inline void SetupDefaultBatches()
		{
			materials.resize(3);
			shaders.resize(3);

			ERenderShaderDomain domains[3] = {
				ERenderShaderDomain::Mesh,
				ERenderShaderDomain::Skinned,
				ERenderShaderDomain::Terrain
			};


			for (size_t i = 0; i < 3; ++i)
			{
				shaders[i].index = i;
				shaders[i].materials.push_back(i);
				shaders[i].shader = nullptr;
				shaders[i].domain = domains[i];

				materials[i].index = i;
				materials[i].material = nullptr;
			}
		}


		// Find if there is a batch created for the shader, if not create a new one.
		inline ShaderBatch* FindOrCreateShader(RenderRscShader* shader)
		{
			// Exist?
			if (shaderMap.count(shader))
			{
				return &shaders[ shaderMap[shader] ];
			}

			// New...
			ShaderBatch newShaderBatch;
			newShaderBatch.index = shaders.size();
			newShaderBatch.shader = shader;
			shaders.push_back(newShaderBatch);

			// Map...
			shaderMap[shader] = newShaderBatch.index;

			return &shaders[newShaderBatch.index];
		}

		// Find if there is a batch created for the material, if not create a new one.
		inline MaterialBatch* FindOrCreateMaterail(RenderRscMaterial* mat)
		{
			// Exist?
			if (matMap.count(mat))
			{
				return &materials[ matMap[mat] ];
			}

			// New...
			MaterialBatch newMatBatch;
			newMatBatch.index = materials.size();
			newMatBatch.material = mat;
			materials.push_back(newMatBatch);

			// Add to custom shader batch.
			ShaderBatch* shaderBatch = FindOrCreateShader(mat->GetShadowShaderRsc());
			shaderBatch->materials.push_back(newMatBatch.index);

			// Map...
			matMap[mat] = newMatBatch.index;

			return &materials[newMatBatch.index];
		}


	private:
		// Map materials to their index in the materials batches list.
		std::unordered_map<RenderRscMaterial*, uint32_t> matMap;

		// Map shader to their index in the shaders batches list.
		std::unordered_map<RenderRscShader*, uint32_t> shaderMap;

		// The shaders the primitives materials are part of.
		std::vector<ShaderBatch> shaders;

		// The materials the primitives are using.
		std::vector<MaterialBatch> materials;

		// The primitives this batch is drawing.
		std::vector<RenderPrimitive*> primitives;
	};


}


