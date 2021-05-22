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
#include "Skeleton.h"


#include "Scene/Entity/Entity.h"
#include "Scene/Scene.h"
#include "Scene/Component/SkinnedMeshComponent.h"
#include "Scene/Component/Transform.h"


#include <vector>
#include <iostream>




namespace Raven
{


Skeleton::Skeleton()
	: IResource()
	, isBuilt(false)
	, root(nullptr)
{
	type = Skeleton::StaticGetType();
}


Skeleton::~Skeleton()
{

}


Bone& Skeleton::CreateBone(int32_t parentId)
{
	RAVEN_ASSERT(!isBuilt, "Skeleton is built and finished, you can't create new bones.");

	// New Bone.
	auto& bone = bones.emplace_back();

	// Index mapping.
	bone.id = bones.size() - 1;
	bone.parentIdx = parentId;

	return bone;
}


int32_t Skeleton::GetBoneIndex(const std::string& name)
{
	for (int32_t i = 0; i < bones.size(); i++)
	{
		if (bones[i].name == name)
			return i;
	}
	
	return -1;
}


void Skeleton::ResetTransforms()
{
	RAVEN_ASSERT(isBuilt, "Skeleton is not built.");

	for (auto & bone : bones)
	{
		bone.SetPosition(glm::vec3(0.0f));
		bone.SetRotation(glm::vec3(0.0f));
	}
}


void Skeleton::Build()
{
	RAVEN_ASSERT(!isBuilt, "Skeleton rebuild not allowed.");

	// Build skeleton bones.
	for (Bone& bone : bones)
	{
		// Has Parent?
		if (bone.parentIdx != -1)
		{
			bone.parent = &bones[bone.parentIdx];
			bone.parent->children.push_back(&bone);
		}
		else
		{
			root = &bone;
			root->parent = nullptr;
		}

		// Rest to rest-pose.
		bone.ResetPose();
	}

	// No Root Found?
	RAVEN_ASSERT(root != nullptr, "Root Not Found.");

	// Done.
	isBuilt = true;
}




// -- - --- - -- - --- - -- - --- - -- - --- - -- - --- - -- - --- - -- - --- - -- - --- - -- - --- - 



SkeletonInstance::SkeletonInstance(SkinnedMeshComponent* inOwner, Ptr<Skeleton> inParent)
	: parent(inParent)
{
	RAVEN_ASSERT(parent != nullptr, "Invalid Skeleton.");

	bonesTransformation.resize(parent->GetBones().size(), glm::mat4(1.0f));
	owner = inOwner->GetEntity();
}


SkeletonInstance::~SkeletonInstance()
{

}


void SkeletonInstance::UpdateBones()
{
	auto& registry = owner.GetScene()->GetRegistry();

	// First mark all cached transform dirty.
	DirtyTransforms();

	// Update...
	const auto& bones = parent->GetBones();
	int32_t boneCount = static_cast<int32_t>(bones.size());

	for (int32_t i = 0; i < boneCount; ++i)
	{
		bonesTransformation[i] = bones[i].GetWorldTransform() * bones[i].GetOffsetMatrix();

		// Update Transform Componenets...
		if ( registry.valid(skeletonTransforms[i]) )
		{
			Transform& trComp = registry.get<Transform>(skeletonTransforms[i]);
			trComp.SetPosition( bones[i].GetPosition(), false );
			trComp.SetRotation( bones[i].GetRotation(), false );
		}
	}

	// Update Transforms -> World/Children...
	const auto rootBone = parent->GetRoot();
	if (rootBone && rootBone->id != -1)
	{
		if ( registry.valid(skeletonTransforms[rootBone->id]) )
		{
			Transform& trComp = registry.get<Transform>(skeletonTransforms[rootBone->id]);
			trComp.UpdateDirty();
		}
	}

}


void SkeletonInstance::DirtyTransforms()
{
	auto& bones = parent->GetBones();
	int32_t boneCount = static_cast<int32_t>(bones.size());

	for (int32_t i = 0; i < boneCount; ++i)
	{
		bones[i].DirtyWorldMatrix();
	}
}


void SkeletonInstance::BuildTransformHierarchy()
{
	RAVEN_ASSERT(skeletonTransforms.empty(), "Can't rebuild hierarchy.");
	Scene* scene = owner.GetScene();
	const auto& bones = parent->GetBones();

	skeletonTransforms.resize(bones.size());

	// Create Transforms...
	for (const auto& bone : bones)
	{
		Entity newEntity = scene->CreateEntity();
		newEntity.GetOrAddComponent<Transform>();
		newEntity.GetOrAddComponent<Hierarchy>();
		newEntity.GetOrAddComponent<NameComponent>().name = bone.name;
		skeletonTransforms[bone.id] = newEntity.GetHandle();
	}

	// Reparent Transforms...
	const auto& rootBone = *parent->GetRoot();
	Entity(skeletonTransforms[rootBone.id], scene).SetParent(owner);

	for (const auto& bone : bones)
	{
		if (bone.parentIdx == -1)
			continue;

		// Set Parent
		Entity(skeletonTransforms[bone.id], scene).SetParent(
			Entity(skeletonTransforms[bone.parentIdx], scene)
		);
	}

}


void SkeletonInstance::DestroyTransformHierarchy()
{
	Scene* scene = owner.GetScene();

	if (!scene)
		return;

	// Destroy Transforms...
	for (const auto& trComp : skeletonTransforms)
	{
		Entity(trComp, scene).Destroy();
	}

	skeletonTransforms.clear();
}


void SkeletonInstance::SetScene(Scene* scene)
{
	owner.SetScene(scene);
}



} // End of namespace Raven.
