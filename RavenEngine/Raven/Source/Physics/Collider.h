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

#include <vector>
#include <array>
#include <memory>

#include <reactphysics3d/reactphysics3d.h>
#include <reactphysics3d/mathematics/Transform.h>

#include <cereal/cereal.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/xml.hpp>
#include <cereal/archives/json.hpp>

#include "Utilities/Core.h"
#include "Utilities/ToRp3d.h"
#include "Utilities/Serialization.h"

#include "Scene/Component/Transform.h"
#include "Scene/Component/TerrainComponent.h"

//
// Component to attach to an entity that needs to be collided with another, wrapper for the rp3d collider class
//

namespace Raven
{
	class CollisionBody;
	class RigidBody;

	namespace ColliderPrimitive
	{
		enum Type : uint32_t
		{
			Box,
			Sphere,
			Capsule,
			//Height,
			MAX
		};

		const std::array<std::string, Type::MAX> NAMES =
		{
			"Box",
			"Sphere",
			"Capsule"
			//, "Height"
		};

		std::string GetColliderPrimitiveName(ColliderPrimitive::Type type);
		ColliderPrimitive::Type GetColliderPrimitiveType(const std::string& type);
	}

	// base collider class
	class Collider
	{
		friend CollisionBody;
		friend RigidBody;
	public:
		Collider(rp3d::CollisionBody* body, ColliderPrimitive::Type shapeType, const Transform& t = Transform::Identity);
		~Collider();

		// creates the collider and adds it to the body 
		virtual void InitShape(rp3d::PhysicsCommon* physicsCommon) = 0;

		virtual void OnImGui() = 0;

		// set the body the collider belongs to (should only be called by the body's AddCollider prior to calling CreateCollider)
		void SetBody(rp3d::CollisionBody* parentBody);

		// use a transform to place the collider relative to the body it should be attached to
		void SetTransform(const Transform& transform);

		ColliderPrimitive::Type GetColliderType() const;

		rp3d::CollisionShape* Collider::GetShape();

		Transform GetRelativeTransform();

		void SetFriction(float f);
		float GetFriction();

	protected:
		// pointer to the collider, set by the collision body class
		rp3d::Collider* collider;

		// shape type
		ColliderPrimitive::Type type;

		// the transform relative to the origin of the body the collider will be attached to
		Transform relativeTransform;

		// pointer to the body the collider belongs to
		rp3d::CollisionBody* body;

		// The coefficient of friction for the collider
		float frictionCoeff;
	};

	// Box collider class
	class BoxCollider : public Collider
	{
	public:
		BoxCollider();
		BoxCollider(rp3d::CollisionBody* body, const Transform& t = Transform::Identity);
		~BoxCollider() = default;

		void InitShape(rp3d::PhysicsCommon* physicsCommon) override;

		void OnImGui() override;

		void SetExtents(const glm::vec3& vec);
		void SetExtents(const float& x, const float& y, const float& z);
		glm::vec3 GetExtents() const;

		template<typename Archive>
		void save(Archive& archive) const
		{
			// archive the collider
			archive(cereal::make_nvp("Half extents", extents),
					cereal::make_nvp("Relative transform", relativeTransform),
					cereal::make_nvp("Friction Coeff", frictionCoeff));
		}

		template<typename Archive>
		void load(Archive& archive)
		{
			// load the extents
			archive(cereal::make_nvp("Half extents", extents),
					cereal::make_nvp("Relative transform", relativeTransform),
				cereal::make_nvp("Friction Coeff", frictionCoeff));
		}

	private:
		// inherits the same variables as the base class
		glm::vec3 extents; // box extent

		// pointer to the collider shape
		rp3d::BoxShape* shape;
	};

	// Sphere collider class
	class SphereCollider : public Collider
	{
	public:
		SphereCollider();
		SphereCollider(rp3d::CollisionBody* body);
		~SphereCollider() = default;

		void InitShape(rp3d::PhysicsCommon* physicsCommon) override;

		void OnImGui() override;

		void SetRadius(const float& r);
		float GetRadius() const;

		template<typename Archive>
		void save(Archive& archive) const
		{
			// then archive the collider
			archive(cereal::make_nvp("Radius", radius),
					cereal::make_nvp("Relative transform", relativeTransform));
		}

		template<typename Archive>
		void load(Archive& archive)
		{
			archive(cereal::make_nvp("Radius", radius),
					cereal::make_nvp("Relative transform", relativeTransform));
		}

	private:
		float radius; // sphere radius
		
		// pointer to the collider shape
		rp3d::SphereShape* shape;
	};

	// Capsule collider class
	class CapsuleCollider : public Collider
	{
	public:
		CapsuleCollider();
		CapsuleCollider(rp3d::CollisionBody* body);
		~CapsuleCollider() = default;

		void InitShape(rp3d::PhysicsCommon* physicsCommon) override;

		void OnImGui() override;

		void SetRadius(const float& r);
		void SetHeight(const float& h);

		float GetRadius() const;
		float GetHeight() const;

		template<typename Archive>
		void save(Archive& archive) const
		{
			// then archive the collider
			archive(cereal::make_nvp("Radius", radius),
					cereal::make_nvp("Height", height),
					cereal::make_nvp("Relative transform", relativeTransform));
		}

		template<typename Archive>
		void load(Archive& archive)
		{
			archive(cereal::make_nvp("Radius", radius),
					cereal::make_nvp("Height", height),
					cereal::make_nvp("Relative transform", relativeTransform));
		}


	private:
		float radius; // capsule radius
		float height; // capsule height
		
		// pointer to the collider shape
		rp3d::CapsuleShape* shape;
	};

	// collider shape factory, needs access to the physics common object from physics module
	namespace ColliderShapeFactory
	{
		rp3d::CollisionShape* CreateCollisionShape(rp3d::PhysicsCommon* physicsCommon, Collider* collider);
		rp3d::CollisionShape* CreateBoxShape(rp3d::PhysicsCommon* physicsCommon, BoxCollider* collider);
		rp3d::CollisionShape* CreateSphereShape(rp3d::PhysicsCommon* physicsCommon, SphereCollider* collider);
		rp3d::CollisionShape* CreateCapsuleShape(rp3d::PhysicsCommon* physicsCommon, CapsuleCollider* collider);
		rp3d::CollisionShape* CreateHeightShape(rp3d::PhysicsCommon* physicsCommon, Ptr<TerrainComponent> terrainComp);
	};

	namespace ColliderFactory
	{
		Collider* CreateCollider(ColliderPrimitive::Type type);
		BoxCollider* CreateBoxCollider();
		SphereCollider* CreateSphereCollider();
		CapsuleCollider* CreateCapsuleCollider();
	};
}

// declare relationships between classes for cereal
CEREAL_REGISTER_TYPE(Raven::BoxCollider);
CEREAL_REGISTER_TYPE(Raven::SphereCollider);
CEREAL_REGISTER_TYPE(Raven::CapsuleCollider);

CEREAL_REGISTER_POLYMORPHIC_RELATION(Raven::Collider, Raven::BoxCollider)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Raven::Collider, Raven::SphereCollider)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Raven::Collider, Raven::CapsuleCollider)


