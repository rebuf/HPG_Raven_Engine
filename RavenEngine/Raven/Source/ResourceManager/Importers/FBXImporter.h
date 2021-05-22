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
#include "ResourceManager/Importers/Importer.h"

#include "glm/vec3.hpp"
#include "glm/vec2.hpp"
#include "glm/gtx/quaternion.hpp"





// Forward Declaration ofbx.
namespace ofbx
{
	class IScene;
	class Object;
	class Mesh;
	class Geometry;
	class Matrix;
}



namespace Raven
{
	class Mesh;
	class Skeleton;
	class SkinnedMesh;
	class AnimationClip;



	// FbxLoader:
	//		- laod an Fbx file.
	//
	class FbxLoader
	{
	private:
		// Orientation used for loading fbx.
		enum class FbxOrientation
		{
			Y_UP,
			Z_UP,
			Z_MINUS_UP,
			X_MINUS_UP,
			X_UP
		};

	public:
		// Construct.
		FbxLoader();

		// Destruct.
		~FbxLoader();

		// Open an fbx file, return false if failed.
		bool Open(const std::string& file);

		// Does the opened fbx file has meshes.
		bool HasMesh();

		// Does the opened fbx file has animation.
		bool HasAnimation();

		// Does the opened fbx file has skin.
		bool HasSkin();

		// Does the opened fbx file has bones.
		bool HasBones();

		// Load all meshes as a single mesh resrouce.
		Mesh* ImportMesh();

		// Load all meshes as a single skinned mesh resrouce.
		SkinnedMesh* ImportSkinnedMesh();

		// Load the skeleton.
		Skeleton* ImportSkeleton();

		// Load all animation clips.
		void ImportAnimation(std::vector< Ptr<AnimationClip> >& clips);

		// Import animation clips.
		Ptr<AnimationClip> ImportAnimationClip(int32_t index, float frameRate);

		// Fix vector orientation.
		glm::vec3 FixOrientation(const glm::vec3& v);

		// Fix quat orientation.
		glm::quat FixOrientation(const glm::quat& v);

		// Convert Fbx matrix to glm matrix.
		static glm::mat4 FbxMatrixToGLM(const ofbx::Matrix& mat);

		// Return the offset matrix for a bone.
		glm::mat4 GetOffsetMatrix(const ofbx::Mesh* mesh, const ofbx::Object* node);

	private:
		// Loaded fbx scene.
		ofbx::IScene* fbx_scene;

		// Orientation to apply on loaded scene.
		FbxOrientation fbx_orientation;

		// Scale to apply on loaded scene.
		float fbx_scale;

		// All the meshes in the scene.
		std::vector<const ofbx::Mesh*> fbx_meshes;

		// All the bones in the scene.
		std::vector<const ofbx::Object*> fbx_bones;

		// are the meshes skinned.
		bool fbx_containSkin;

		// does the fbx file contain animation.
		bool fbx_containAnimations;

	public:
		// Loaded skeleton.
		Skeleton* skeleton;
	};



	struct FBXImporterSettings
	{
		// The skeleton, used by import animation.
		Ptr<Skeleton> skeleton;

		// Only import animation that use the skeleton
		bool importAnimationOnly;

		// Import only the mesh even if there is animation and a skeleton.
		bool importMeshOnly;

		// Construct.
		FBXImporterSettings()
			: skeleton(nullptr)
			, importAnimationOnly(false)
			, importMeshOnly(false)
		{

		}
	};


	// FBXImporter:
	//    - import fbox into a mesh or skinned mesh resource.
	class FBXImporter : public IImporter
	{
	public:
		// Construct.
		FBXImporter();

		// Return the importer type.
		inline static EImporterType StaticGetType() noexcept { return IMP_FBX; }

		// List all supported extensions by this importer.
		virtual void ListExtensions(std::vector<std::string>& outExt) override;

		// Import a new resrouce.
		virtual bool Import(const std::string& path, std::vector< Ptr<IResource> >& resources) override;

	private:
		// Import animation for this skeleton.
		void ImportAnimation(FbxLoader& fbx, Ptr<Skeleton> skeleton, const std::string& name, std::vector< Ptr<IResource> >& resources);

	public:
		// Next import settings will be reset after importing.
		FBXImporterSettings settings;
	};
}