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


#include "Component.h"
#include "Utilities/Serialization.h"


#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>



namespace Raven 
{
	class Mesh;

	constexpr glm::vec3 ZERO(0, 0, 0);
	constexpr glm::vec3 LEFT(-1.0f, 0.0f, 0.0f);
	constexpr glm::vec3 RIGHT(1.0f, 0.0f, 0.0f);
	constexpr glm::vec3 UP(0.0f, 1.0f, 0.0f);
	constexpr glm::vec3 DOWN(0.0f, -1.0f, 0.0f);
	constexpr glm::vec3 FORWARD(0.0f, 0.0f, 1.0f);
	constexpr glm::vec3 BACK(0.0f, 0.0f, -1.0f);
	constexpr glm::vec3 ONE(1.0f, 1.0f, 1.0f);



	// Transform
	//    - 
	class Transform : public Component
	{
	public:
		// Construct Identity.
		Transform();

		// Construct from matrix.
		Transform(const glm::mat4& matrix);

		// Construct from Position.
		Transform(const glm::vec3& inPosition);

		// Destruct.
		~Transform();

		// Identity Transform.
		static const Transform Identity;

		// Return the transform in world coordinate.
		inline auto& GetWorldMatrix() 
		{ 
			if (isWorldMatrixDiry) 
				UpdateWorldMatrix();

			return worldMatrix; 
		}

		// Return the transform in world coordinate const version.
		inline const auto& GetWorldMatrix() const { return const_cast<Transform*>(this)->GetWorldMatrix(); }

		// Return the transform as a matrix..
		inline const auto& GetLocalMatrix() const { if (isLocalMatrixCacheDiry) UpdateLocalMatrix();  return localMatrix_cache; }

		// Return World Position.
		inline auto GetWorldPosition() const 
		{ 
			if (isWorldMatrixDiry) 
				UpdateWorldMatrix();

			return glm::vec3{ worldMatrix[3][0], worldMatrix[3][1], worldMatrix[3][2] }; 
		}

		// Return World Rotation.
		inline auto GetWorldRotation() const 
		{
			if (isWorldMatrixDiry)
				UpdateWorldMatrix();

			return glm::quat_cast(worldMatrix);
		};

		// Get Local...
		inline auto& GetPosition() const { return position; }
		inline auto& GetScale() const { return scale; }
		inline auto& GetRotation() const { return rotation; }
		glm::vec3 GetRotationEuler() const; // Order of rotation v' = Yaw * Pitch * Roll * v;

		// Set Local...
		// @param update: for performance reasons you might want to disable updating and just leave
		//                the transform dirty for later one pass update. 
		void SetPosition(const glm::vec3& v, bool update);
		void SetScale(const glm::vec3& v, bool update);
		void SetRotation(const glm::quat& v, bool update);


		// Set Local...
		inline void SetPosition(const glm::vec3& v) { SetPosition(v, true); }
		inline void SetScale(const glm::vec3& v)    { SetScale(v, true);    }
		inline void SetRotation(const glm::quat& v) { SetRotation(v, true); }



		// Set Euler Angles:
		//     Order of rotation v' = Yaw * Pitch * Roll * v;
		void SetRotationEuler(float pitch, float yaw, float roll);
		inline void SetRotationEuler(const glm::vec3 angles) { SetRotationEuler(angles.x, angles.y, angles.z); }

		// Set World...
		void SetWorldPosition(const glm::vec3& v);
		void SetWorldScale(const glm::vec3& v);
		void SetWorldRotation(const glm::quat& v);
		void SetWorldRotation(float pitch, float yaw, float roll); // Order of rotation v' = Yaw * Pitch * Roll * v;

		// Set Matrix...
		void SetMatrixTransform(const glm::mat4& mtx);
		void SetWorldMatrixTransform(const glm::mat4& mtx);

		// Set Transform
		void SetTransform(Transform& other);

		// Reset Transform to identity.
		void ResetTransform();

		// Get Transformation Up-Vector.
		glm::vec3 GetUpDirection() const;

		// Get Transformation Right-Vector.
		glm::vec3 GetRightDirection() const;
		
		// Get Transformation Forward-Vector.
		glm::vec3 GetForwardDirection() const;
	
		// Serialize Save.
		template<typename Archive>
		void save(Archive& archive) const
		{
			archive(cereal::base_class<Component>(this));

			archive(
				cereal::make_nvp("Position", position),
				cereal::make_nvp("Scale", scale),
				cereal::make_nvp("Rotation", rotation)
			);
		}

		// Serialize Load.
		template<typename Archive>
		void load(Archive& archive)
		{
			archive(cereal::base_class<Component>(this));

			archive(
				cereal::make_nvp("Position", position),
				cereal::make_nvp("Scale", scale),
				cereal::make_nvp("Rotation", rotation)
			);

			Dirty();
		}

		// Update world matrix only if its dirty.
		void UpdateDirty();

		// Mark the cached matrices as dirty.
		void Dirty();

	public:
		// Extract scale from a matrix.
		static glm::vec3 ExtractScale(const glm::mat4& mtx);

	private:
		// Update local matrix cache.
		void UpdateLocalMatrix() const;

		// Update world matrix.
		void UpdateWorldMatrix() const;

		// Update Children Transforms.
		void UpdateChildrenWorld() const;

		// Return parent world matrix.
		bool GetParentWorldMatrix(glm::mat4& outMtx) const;

	private:
		// The Local Translation.
		glm::vec3 position;

		// The Local Scale.
		glm::vec3 scale;

		// The Local Orientation.
		glm::quat rotation;

		// The world matrix of this component computed when transformation changed.
		mutable glm::mat4 worldMatrix;

		// if true the world matrix need to be updated.
		mutable bool isWorldMatrixDiry;

		// Local cache for the local transform to not compute the local matrix every time.
		mutable glm::mat4 localMatrix_cache;

		// True if the local matrix cache doesn't match the
		mutable bool isLocalMatrixCacheDiry;
	};
	
}



