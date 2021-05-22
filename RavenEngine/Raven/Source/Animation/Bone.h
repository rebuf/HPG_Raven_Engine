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
#include "Utilities/Serialization.h"

#include <cereal/cereal.hpp>
#include <string>
#include <vector>
#include <memory>


namespace Raven
{
	class Skeleton;


	// Bone in a Skeleton.
	class Bone
	{
		// Friends...
		friend class Skeleton;
		friend class SkeletonInstance;

	public:
		// Construct.
		Bone();

		// Serialization Load.
		template<class Archive>
		void load(Archive& archive)
		{
			archive(
				name,
				id,
				parentIdx,
				offsetMatrix,
				restPosition,
				restRotation
			);
		}

		// Serialization Save.
		template<class Archive>
		void save(Archive& archive) const 
		{
			archive(
				name,
				id,
				parentIdx,
				offsetMatrix,
				restPosition,
				restRotation
			);
		}

		// Apply reset pose to current bone transform.
		void ResetPose();

		// Set the rest pose transform and save it for later use.
		void SetRestPose(const glm::mat4& mtx);

		// Set bone transform position.
		void SetPosition(const glm::vec3& v);

		// Return bone transform position.
		const glm::vec3& GetPosition() const { return position; }

		// Set bone transform rotation.
		void SetRotation(const glm::vec3& v);

		// Return bone transform rotation.
		const glm::vec3& GetRotation() const { return rotation; }

		// Return bone transformation as matrix.
		glm::mat4 GetLocalTransform() const;

		// Return bone world transform
		const glm::mat4& GetWorldTransform() const;

		// Set bone name.
		inline void SetName(const std::string& inName) { name = inName; }

		// Return bone name.
		inline const std::string& GetName() const { return name; }

		// Set Bone Offset Matrix.
		inline void SetOffsetMatrix(const glm::mat4& matrix) { offsetMatrix = matrix; }

		// Get Bone Offset Matrix.
		inline const glm::mat4& GetOffsetMatrix() const { return offsetMatrix; }

		// Return bone parent.
		inline Bone* GetParent() const { return parent; }

		// Return bone children.
		inline const std::vector<Bone*>& GetChildren() const { return children; }

		// Mark world matrix dirty.
		void DirtyWorldMatrix() const;

	private:
		// Bone Name.
		std::string name;

		// The Bone Offset Matrix.
		glm::mat4 offsetMatrix;

		// Parent Bone.
		Bone* parent;

		// Children Bone.
		std::vector<Bone*> children;

		// Index of the parent bone in the skeleton.
		int32_t parentIdx;

		// Id of the bone in the skeleton.
		int32_t id;

		// The Bone Local Transformation.
		glm::vec3 position;
		glm::vec3 rotation;

		// The Bone Rest Local Transformation.
		glm::vec3 restPosition;
		glm::vec3 restRotation;

		// The Bone World Transformation.
		mutable glm::mat4 worldTransform;

		// if dirty world transform need updating.
		mutable bool isTransformDirty;
	};


}
