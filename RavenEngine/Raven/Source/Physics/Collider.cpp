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

#include <iostream>

#include <imgui/imgui.h>

#include <glm/gtc/type_ptr.hpp>

#include <reactphysics3d/mathematics/Vector3.h> 

#include "Engine.h"

#include "Physics/PhysicsModule.h"
#include "Physics/Collider.h"

namespace Raven
{
	//
	// Collider base class
	//

	Collider::Collider(rp3d::CollisionBody* initBody, ColliderPrimitive::Type shapeType, const Transform& t) :
		type(shapeType),							   // When creating a new collider, specify the type
		body(initBody),
		collider(nullptr),
		frictionCoeff(0.3),
		relativeTransform(t) // initialise the relative transform to the identity matrix
	{}

	// when deleting the collider, remove it from the body it's attached to
	Collider::~Collider()
	{}

	// call this when creating a collider 
	void Collider::SetBody(rp3d::CollisionBody* parentBody)
	{
		body = parentBody;
	}

	void Collider::SetTransform(const Transform& transform)
	{
		relativeTransform = transform;
		collider->setLocalToBodyTransform(Rp3dConvert::ToRp3dTransform(relativeTransform));
	}

	ColliderPrimitive::Type Collider::GetColliderType() const
	{
		return type;
	}

	rp3d::CollisionShape* Collider::GetShape()
	{
		if (collider)
			return collider->getCollisionShape();
	}

	Transform Collider::GetRelativeTransform()
	{
		return relativeTransform;
	}

	// BEN
	void Collider::SetFriction(float f)
	{
		frictionCoeff = f;
		if (collider) collider->getMaterial().setFrictionCoefficient(frictionCoeff);
	}

	// BEN
	float Collider::GetFriction()
	{
		return frictionCoeff;
	}

	//
	// Box collider class
	//

	BoxCollider::BoxCollider() :
		Collider(nullptr, ColliderPrimitive::Box),
		extents(1.0f, 1.0f, 1.0f)
	{
		// don't init shape here since no body provided
	}

	BoxCollider::BoxCollider(rp3d::CollisionBody* body, const Transform& t) :
		Collider(body, ColliderPrimitive::Box, t),
		extents(1.0f, 1.0f, 1.0f)
	{
		//InitShape(Engine::Get().GetModule<PhysicsModule>()->GetPhysicsCommon());
	}

	// creates the collider by adding it to the body 
	void BoxCollider::InitShape(rp3d::PhysicsCommon* physicsCommon)
	{
		// only init shape when body is present and collider is nullptr
		if (body && collider == nullptr)
		{
			rp3d::CollisionShape* s = ColliderShapeFactory::CreateBoxShape(physicsCommon, this);
			collider = body->addCollider(s, Rp3dConvert::ToRp3dTransform(relativeTransform));
			collider->getMaterial().setFrictionCoefficient(frictionCoeff); // BEN
		}
	}

	void BoxCollider::OnImGui()
	{
		// the UI for modifying a box collider
		ImGui::TextUnformatted("Half extents");
		ImGui::NextColumn();

		ImGui::PushItemWidth(-1);
		if (ImGui::DragFloat3("##Half extents", glm::value_ptr(extents), 0.05))
		{
			// need to update collider's extents in collider and in engine
			SetExtents(extents);
		}

		ImGui::PopItemWidth();
		ImGui::NextColumn();

		float fric = GetFriction();

		ImGui::TextUnformatted("Friction");
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);
		if (ImGui::DragFloat("##Friction", &fric, 0.05f, 0.0f, 1.0f))
		{
			SetFriction(fric);
		}

		ImGui::PopItemWidth();
		ImGui::NextColumn();
	}

	void BoxCollider::SetExtents(const glm::vec3& vec)
	{
		// update the extents
		extents = vec;
		// if collider was assigned a body, it exists in physics engine so also update extents there
		if (body)
			((rp3d::BoxShape*)collider->getCollisionShape())->setHalfExtents(Rp3dConvert::ToRp3dVector3(vec));
	}

	void BoxCollider::SetExtents(const float& x, const float& y, const float& z)
	{
		extents = glm::vec3(x,y,z);
		if (body)
			((rp3d::BoxShape*)collider->getCollisionShape())->setHalfExtents(Rp3dConvert::ToRp3dVector3(extents));
	}
	glm::vec3 BoxCollider::GetExtents() const
	{
		return extents;
	}

	//
	// Sphere collider class
	//
	
	SphereCollider::SphereCollider() :
		Collider(nullptr, ColliderPrimitive::Sphere),
		radius(1.0f)
	{}

	SphereCollider::SphereCollider(rp3d::CollisionBody* body) :
		Collider(body, ColliderPrimitive::Sphere),
		radius(1.0f)
	{
		//InitShape(Engine::Get().GetModule<PhysicsModule>()->GetPhysicsCommon());
	}

	// creates the collider by adding it to the body 
	void SphereCollider::InitShape(rp3d::PhysicsCommon* physicsCommon)
	{
		if (body)
			collider = body->addCollider(ColliderShapeFactory::CreateSphereShape(physicsCommon, this), Rp3dConvert::ToRp3dTransform(relativeTransform));
	}

	void SphereCollider::OnImGui()
	{
		// the UI for modifying a sphere collider
		ImGui::TextUnformatted("Radius");
		ImGui::NextColumn();

		ImGui::PushItemWidth(-1);
		if (ImGui::DragFloat("##Radius", &radius, 0.05))
		{
			// need to update collider's extents in collider and in engine
			SetRadius(radius);
		}

		ImGui::PopItemWidth();
		ImGui::NextColumn();
	}

	void SphereCollider::SetRadius(const float& r)
	{
		radius = r;
		// if collider was assigned a body, it exits in physics engine so also update extents there
		if (body)
			((rp3d::SphereShape*)collider->getCollisionShape())->setRadius(r);
	}

	float SphereCollider::GetRadius() const
	{
		return radius;
	}

	//
	// Capsule collider class
	//

	CapsuleCollider::CapsuleCollider() :
		Collider(nullptr, ColliderPrimitive::Capsule),
		radius(1.0f),
		height(1.0f)
	{}

	CapsuleCollider::CapsuleCollider(rp3d::CollisionBody* body) :
		Collider(body, ColliderPrimitive::Capsule),
		radius(1.0f),
		height(1.0f)
	{
		//InitShape(Engine::Get().GetModule<PhysicsModule>()->GetPhysicsCommon());
	}

	void CapsuleCollider::InitShape(rp3d::PhysicsCommon* physicsCommon)
	{
		if (body)
			collider = body->addCollider(ColliderShapeFactory::CreateCapsuleShape(physicsCommon, this), Rp3dConvert::ToRp3dTransform(relativeTransform));
	}

	void CapsuleCollider::OnImGui()
	{
		// the UI for modifying a capsule collider
		ImGui::TextUnformatted("Radius");
		ImGui::NextColumn();

		ImGui::PushItemWidth(-1);
		if (ImGui::DragFloat("##Radius", &radius, 0.05))
		{
			// need to update collider's extents in collider and in engine
			SetRadius(radius);
		}

		ImGui::PopItemWidth();
		ImGui::NextColumn();

		ImGui::TextUnformatted("Height");
		ImGui::NextColumn();

		ImGui::PushItemWidth(-1);
		if (ImGui::DragFloat("##Height", &height, 0.05))
		{
			// need to update collider's extents in collider and in engine
			SetHeight(height);
		}

		ImGui::PopItemWidth();
		ImGui::NextColumn();
	}

	void CapsuleCollider::SetRadius(const float& r)
	{
		radius = r;
		if (body)
			((rp3d::SphereShape*)collider->getCollisionShape())->setRadius(r);
	}

	void CapsuleCollider::SetHeight(const float& h)
	{
		height = h;
		if (body)
			((rp3d::CapsuleShape*)collider->getCollisionShape())->setHeight(h);
	}

	float CapsuleCollider::GetRadius() const
	{
		return radius;
	}

	float CapsuleCollider::GetHeight() const
	{
		return height;
	}

	//
	// Collider shape factory
	//

	namespace ColliderShapeFactory
	{
		rp3d::CollisionShape* CreateCollisionShape(rp3d::PhysicsCommon* physicsCommon, Collider* collider)
		{
			// Use a C style cast to avoid overhead of dynamic cast, and avoid compile time error for static cast
			// Naturally, only pointers to derived colliders should be passed
			switch (collider->GetColliderType())
			{
			case Raven::ColliderPrimitive::Box:
				return CreateBoxShape(physicsCommon, (BoxCollider*) collider);
			case Raven::ColliderPrimitive::Sphere:
				return CreateSphereShape(physicsCommon, (SphereCollider*) collider);
			case Raven::ColliderPrimitive::Capsule:
				return CreateCapsuleShape(physicsCommon, (CapsuleCollider*) collider);
			default:
				return nullptr;
				// height must be handled differently so not in this method
			}
		}

		rp3d::CollisionShape* CreateBoxShape(rp3d::PhysicsCommon* physicsCommon, BoxCollider* collider)
		{
			return physicsCommon->createBoxShape(Rp3dConvert::ToRp3dVector3(collider->GetExtents()));
		}

		rp3d::CollisionShape* CreateSphereShape(rp3d::PhysicsCommon* physicsCommon, SphereCollider* collider)
		{
			return physicsCommon->createSphereShape(collider->GetRadius());
		}

		rp3d::CollisionShape* CreateCapsuleShape(rp3d::PhysicsCommon* physicsCommon, CapsuleCollider* collider)
		{
			return physicsCommon->createCapsuleShape(collider->GetRadius(), collider->GetHeight());
		}

		rp3d::CollisionShape* CreateHeightShape(rp3d::PhysicsCommon* physicsCommon, Ptr<TerrainComponent> terrainComp)
		{
			Ptr<Terrain> terrain = terrainComp->GetTerrain();
			const HeightMap* heightMap = terrain->GetHeightMap();

			return physicsCommon->createHeightFieldShape(
				heightMap->GetSize().x,           // columns
				heightMap->GetSize().y,           // rows
				terrain->GetHeight().x,           // min height
				terrain->GetHeight().y,           // max height
				heightMap->GetHeightMapData(),    // ptr to height data
				rp3d::HeightFieldShape::HeightDataType::HEIGHT_FLOAT_TYPE  // Float Data Type.
			);

			RAVEN_ASSERT(0, "Not Implemented.");

			return nullptr;
		}
	}

	namespace ColliderFactory
	{
		Collider* CreateCollider(ColliderPrimitive::Type type)
		{
			switch (type)
			{
			case Raven::ColliderPrimitive::Box:
				return new BoxCollider();
			case Raven::ColliderPrimitive::Sphere:
				return new SphereCollider();
			case Raven::ColliderPrimitive::Capsule:
				return new CapsuleCollider();
			default:
				return new BoxCollider();
			}
		}
	};

	//
	// Collider shape primitives
	//

	namespace ColliderPrimitive
	{
		std::string GetColliderPrimitiveName(Type type)
		{
			return NAMES[(uint32_t)type];
		};

		Type GetColliderPrimitiveType(const std::string& type)
		{
			if (type == "Box")
			{
				return Type::Box;
			}
			else if (type == "Sphere")
			{
				return Type::Sphere;
			}
			else if (type == "Capsule")
			{
				return Type::Capsule;
			}
			/*
			else if (type == "Height")
			{
				return Type::Height;
			}
			*/
			return Type::Box;
		}
	}
}

