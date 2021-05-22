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
#include "MathExport.h"
#include "LuaVirtualMachine.h"
#include <LuaBridge/LuaBridge.h>
#include "Engine.h"
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <string>
#include <functional>

#include "Scene/Component/Transform.h"


namespace Raven
{
	namespace MathExport
	{
		struct VecHelper
		{
			template <class T, unsigned index>
			static float get(T const* vec)
			{
				return (*vec)[index];
			}

			template <class T, unsigned index>
			static void set(T* vec, float value)
			{
				(*vec)[index] = value;
			}

			template <class T>
			static std::string toString(const T* v)
			{
				return glm::to_string(*v);
			}

			template <class T>
			static bool equal(const T* v)
			{
				return glm::to_string(*v);
			}

			template <class T>
			static T add(const T* t, const T* t2) {
				return *t + *t2;
			}

			template <class T>
			static T sub(const T* t, const T* t2) {
				return *t - *t2;
			}
			template <class T>
			static T mul(const T* t, const T* t2) {
				return (*t) * (*t2);
			}

			template <class T>
			static float dot(const T* t, const T* t2) {
				return  glm::dot(*t, *t2);
			}

			template <class T>
			static T cross(const T* t, const T* t2) {
				return T();
			}


			template <class T>
			static T normalize(const T* t) {
				return  glm::normalize(*t);
			}


			template <>
			static glm::vec3 cross(const glm::vec3* t, const glm::vec3* t2) {
				return glm::cross(*t, *t2);
			}

		};


		void Export(lua_State* L)
		{
			luabridge::getGlobalNamespace(L)
				.beginNamespace("glm")

				.beginClass <glm::vec2>("vec2")
				.addConstructor <void (*) (float, float)>()
				.addProperty("x", &VecHelper::get <glm::vec2, 0>, &VecHelper::set <glm::vec2, 0>)
				.addProperty("y", &VecHelper::get <glm::vec2, 1>, &VecHelper::set <glm::vec2, 1>)
				.addFunction("__tostring", &VecHelper::toString<glm::vec2>)
				.addFunction("__add", &VecHelper::add<glm::vec2>)
				.addFunction("__mul", &VecHelper::mul<glm::vec2>)
				.addFunction("__sub", &VecHelper::sub<glm::vec2>)
				.addFunction("dot", &VecHelper::dot<glm::vec2>)
				.endClass()

				.beginClass <glm::vec3>("vec3")
				.addConstructor <void (*) (float, float, float)>()
				.addProperty("x", &VecHelper::get <glm::vec3, 0>, &VecHelper::set <glm::vec3, 0>)
				.addProperty("y", &VecHelper::get <glm::vec3, 1>, &VecHelper::set <glm::vec3, 1>)
				.addProperty("z", &VecHelper::get <glm::vec3, 2>, &VecHelper::set <glm::vec3, 2>)
				.addFunction("__tostring", &VecHelper::toString<glm::vec3>)
				.addFunction("__add", &VecHelper::add<glm::vec3>)
				.addFunction("__mul", &VecHelper::mul<glm::vec3>)
				.addFunction("__sub", &VecHelper::sub<glm::vec3>)
				.addFunction("dot", &VecHelper::dot<glm::vec3>)
				.addFunction("cross", &VecHelper::cross<glm::vec3>)
				.addFunction("normalize", &VecHelper::normalize<glm::vec3>)
				.endClass()


				.beginClass <glm::vec4>("vec4")
				.addConstructor <void (*) (float, float, float, float)>()
				.addProperty("x", &VecHelper::get <glm::vec4, 0>, &VecHelper::set <glm::vec4, 0>)
				.addProperty("y", &VecHelper::get <glm::vec4, 1>, &VecHelper::set <glm::vec4, 1>)
				.addProperty("z", &VecHelper::get <glm::vec4, 2>, &VecHelper::set <glm::vec4, 2>)
				.addProperty("w", &VecHelper::get <glm::vec4, 3>, &VecHelper::set <glm::vec4, 3>)
				.addFunction("__tostring", &VecHelper::toString<glm::vec4>)
				.addFunction("__add", &VecHelper::add<glm::vec4>)
				.addFunction("__mul", &VecHelper::mul<glm::vec4>)
				.addFunction("__sub", &VecHelper::sub<glm::vec4>)
				.addFunction("dot", &VecHelper::dot<glm::vec4>)
				.addFunction("cross", &VecHelper::cross<glm::vec4>)
				.addFunction("normalize", &VecHelper::normalize<glm::vec4>)
				.endClass()


				.beginClass <glm::mat4>("mat4")
				.addConstructor <void (*) ()>()
				.addConstructor <void (*) (float)>()
				.addFunction("__tostring", &VecHelper::toString<glm::mat4>)
				.addFunction("__add", &VecHelper::add<glm::mat4>)
				.addFunction("__mul", &VecHelper::mul<glm::mat4>)
				.addFunction("__sub", &VecHelper::sub<glm::mat4>)
				.endClass()


				.endNamespace();


			luabridge::getGlobalNamespace(L)

				.beginClass <Transform>("Transform")
				.addConstructor <void (*) (glm::vec3)>()
				.addConstructor <void (*) (glm::mat4)>()
				.addConstructor <void (*) ()>()

				.addFunction("GetScale", &Transform::GetScale)
				.addFunction("GetPosition", &Transform::GetPosition)
				.addFunction("GetRotation", &Transform::GetRotation)
				.addFunction("GetWorldPosition", &Transform::GetWorldPosition)

				.addFunction("Dirty", &Transform::Dirty)
				.addFunction("UpdateDirty", &Transform::UpdateDirty)

				.addFunction("SetScale", static_cast<void(Transform::*)(const glm::vec3&)>(&Transform::SetScale))
				.addFunction("SetPosition", static_cast<void(Transform::*)(const glm::vec3&)>(&Transform::SetPosition))
				.addFunction("SetRotation", static_cast<void(Transform::*)(const glm::quat&)>(&Transform::SetRotation))
				.addFunction("SetWorldPosition", &Transform::SetWorldPosition)

				.addProperty("Scale", &Transform::GetScale, &Transform::SetScale)
				.addProperty("Position", &Transform::GetPosition, &Transform::SetPosition)
				.addProperty("Rotation", &Transform::GetRotation, &Transform::SetRotation)

				.endClass();
			
		}
	};
};
