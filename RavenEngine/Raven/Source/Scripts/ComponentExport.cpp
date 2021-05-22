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
#include "ComponentExport.h"

#include "LuaVirtualMachine.h"
#include <LuaBridge/LuaBridge.h>
#include <string>
#include <functional>

#include "Scene/Component/Transform.h"
#include "Scene/Component/Component.h"
#include "Scene/Component/Light.h"
#include "Scene/Component/RigidBody.h"
#include "Scene/Component/SoundComponent.h"
#include "Animation/Animator.h"

#include "LuaComponent.h"

#include "Scene/Entity/Entity.h"
#include "Scene/Entity/EntityManager.h"


namespace Raven
{
#define EXPORT_COMPONENTS(Comp) \
		addFunction("Add" #Comp,&Entity::AddComponent<Comp>) \
		.addFunction("Remove" #Comp, &Entity::RemoveComponent<Comp>) \
		.addFunction("Get" #Comp, &Entity::GetComponent<Comp>) \
		.addFunction("GetOrAdd" #Comp, &Entity::GetOrAddComponent<Comp>) \
		.addFunction("TryGet" #Comp, &Entity::TryGetComponent<Comp>) \
        .addFunction("AddOrReplace" #Comp, &Entity::AddOrReplaceComponent<Comp>) \
        .addFunction("Has" #Comp, &Entity::HasComponent<Comp>) \

	namespace ComponentExport
	{
		void Export(lua_State* L)
		{
			luabridge::getGlobalNamespace(L)
				.beginNamespace("entt")
				.beginClass<entt::registry>("registry")
				.endClass()
				.beginClass<entt::entity>("entity")
				.endClass()
				.endNamespace()



				.beginClass <Entity>("Entity")
				.addConstructor <void (*) (entt::entity, Scene*)>()
				.addConstructor <void (*) ()>()
				.addFunction("Valid", &Entity::Valid)
				.addFunction("Destroy", &Entity::Destroy)
				.addFunction("SetParent", &Entity::SetParent)
				.addFunction("GetParent", &Entity::GetParent)
				.addFunction("IsParent", &Entity::IsParent)
				.addFunction("GetChildren", &Entity::GetChildren)
				.addFunction("SetActive", &Entity::SetActive)
				.addFunction("IsActive", &Entity::IsActive)
				.addFunction("GetChildInChildren", &Entity::GetChildInChildren)

				.EXPORT_COMPONENTS(NameComponent)
				.EXPORT_COMPONENTS(ActiveComponent)
				.EXPORT_COMPONENTS(Transform)
				.EXPORT_COMPONENTS(Light)
				.EXPORT_COMPONENTS(Animator)
				.EXPORT_COMPONENTS(LuaComponent)
				.EXPORT_COMPONENTS(RigidBody)
				.EXPORT_COMPONENTS(SoundComponent)

				.endClass()

				.beginClass<EntityManager>("EntityManager")
				.addFunction("Create", static_cast<Entity(EntityManager::*)()> (&EntityManager::Create))
				.addFunction("GetRegistry", &EntityManager::GetRegistry)
				.endClass()

				.beginClass<NameComponent>("NameComponent")
				.addProperty("name", &NameComponent::name)
				.addFunction("GetEntity", &NameComponent::GetEntity)
				.endClass()

				.beginClass<Light>("Light")
				.addProperty("color", &Light::color)
				.addProperty("position", &Light::position)
				.addProperty("direction", &Light::direction)
				.addProperty("intensity", &Light::intensity)
				.addProperty("innerAngle", &Light::innerAngle)
				.addProperty("outerAngle", &Light::outerAngle)
				.addProperty("clipDistance", &Light::clipDistance)
				.addFunction("GetEntity", &Light::GetEntity)
				.endClass()

				.beginClass<ActiveComponent>("ActiveComponent")
				.addProperty("active", &ActiveComponent::active)
				.addFunction("GetEntity", &ActiveComponent::GetEntity)
				.endClass()

				.beginClass<SoundComponent>("SoundComponent")
				.addFunction("GetAudioSource", &SoundComponent::GetAudioSource)
				.endClass()

				.beginClass<LuaComponent>("LuaComponent")
				.endClass()

				.beginClass<Animator>("Animator")
				.addFunction("SetValue", &Animator::SetValue<float>)
				.addFunction("SetValue", &Animator::SetValue<int32_t>)
				.addFunction("SetValue", &Animator::SetValue<std::string>)
				.addFunction("SetValue", &Animator::SetValue<bool>)
				.addFunction("GetBool",  &Animator::GetValue<bool>)
				.addFunction("GetInt", &Animator::GetValue<int32_t>)
				.addFunction("GetFloat", &Animator::GetValue<float>)
				.addFunction("GetEntity", &Animator::GetEntity)
				.addFunction("SetWrapMode", &Animator::SetWrapMode)
				.endClass()
				

				.beginClass<RigidBody>("RigidBody")
				//.addProperty("Mass", &RigidBody::GetMass, &RigidBody::SetMass)
				.addFunction("SetLinearDamping", &RigidBody::SetLinearDamping)
				.addFunction("SetAngularDamping", &RigidBody::SetAngularDamping)
				.addFunction("GetForwardVector", &RigidBody::GetForwardVector)
				.addFunction("GetRightVector", &RigidBody::GetRightVector)
				.addFunction("GetUpVector", &RigidBody::GetUpVector)
				.addFunction("ApplyForce", &RigidBody::ApplyForce)
				.addFunction("ApplyForceAtLocalPos", &RigidBody::ApplyForceAtLocalPos)
				.addFunction("ApplyForceAtWorldPos", &RigidBody::ApplyForceAtWorldPos)
				.addFunction("ApplyTorque", &RigidBody::ApplyTorque)
				.addFunction("RotateBody", &RigidBody::RotateBody)
				.addFunction("GetEntity", &RigidBody::GetEntity)
				.endClass();
				

			auto v1 = luabridge::newTable(L);
			v1["Default"] = 0;
			v1["Once"] = 1;
			v1["Loop"] = 2;
			v1["PingPong"] = 4;
			v1["ClampForever"] = 8;
			luabridge::setGlobal(L, v1, "AnimationWrapMode");

		}
	};
};