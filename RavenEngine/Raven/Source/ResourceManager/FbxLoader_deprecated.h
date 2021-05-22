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


#include <vector>
#include <unordered_map>
#include "ofbx.h"
#include "Scene/Entity/Entity.h"

namespace Raven
{
	class Model_deprecated;
	class SkinnedMesh;
	class SkinnedMeshSection;
	class Skeleton;
	class Animation;
	class AnimationClip;

	// deprecated Moved to Fbx Importer.
	class FbxLoader_deprecated
	{
	public:
		~FbxLoader_deprecated();
		void Load(const std::string& file, Model_deprecated* model);
		void LoadHierarchy(const std::string& file, Model_deprecated* model);
		void LoadAnimation(const std::string& file, Model_deprecated* model);
		void LoadBones(const std::string& file);

		inline auto& GetAnimation() { return animation; }
		inline auto& GetSkeleton() { return skeleton; }


	private:
		std::shared_ptr<AnimationClip> ImportAnimation(Model_deprecated* model, int32_t index, float frameRate);
		void LoadBones(const ofbx::Mesh* fbxMesh, Model_deprecated* model);
		void LoadMeshes(Model_deprecated* model);
		void LoadWeight(const ofbx::Skin* skin, int32_t firstVertexOffset, SkinnedMeshSection* mesh);
		void ImportMesh(Model_deprecated* model, const ofbx::Mesh* aMesh, int32_t triangleStart, int32_t triangleEnd);
		void LoadAnimation(Model_deprecated* model);
		void LoadBones();

		ofbx::IScene* scene = nullptr;
		Skeleton * skeleton = nullptr;
		std::shared_ptr<Animation> animation;
		std::vector<const ofbx::Object*> bones;
		std::string file;
	};


}; 