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
#include <memory>
#include <unordered_map>
#include <typeinfo>
#include "Utilities/Core.h"
#include "ISystem.h"

namespace Raven
{
	class Scene;
	class SystemManager final
	{
	public:
		template<typename T, typename... Args>
		std::shared_ptr<ISystem> AddSystem(Args&&... args)
		{
			static_assert(std::is_base_of<ISystem, T>::value, "class T should extend from ISystem");
			auto typeName = typeid(T).hash_code();
			RAVEN_ASSERT(systems.find(typeName) == systems.end(), "Add system more than once.");
			return systems.emplace(typeName, std::make_shared<T>(std::forward<Args>(args)...)).first->second;
		}

		template<typename T>
		std::shared_ptr<ISystem> AddSystem(T* t)
		{
			static_assert(std::is_base_of<ISystem, T>::value, "class T should extend from ISystem");
			auto typeName = typeid(T).hash_code();
			RAVEN_ASSERT(systems.find(typeName) == systems.end(), "Add system more than once.");
			return systems.emplace(typeName, std::shared_ptr<T>(t)).first->second;
		}

		template<typename T>
		void RemoveSystem()
		{
			auto typeName = typeid(T).hash_code();

			if (systems.find(typeName) != systems.end())
			{
				systems.erase(typeName);
			}
		}

		template<typename T>
		T* GetSystem()
		{
			auto typeName = typeid(T).hash_code();
			if (systems.find(typeName) != systems.end())
			{
				return dynamic_cast<T*>(systems[typeName].get());
			}
			return nullptr;
		}

		template<typename T>
		T* HasSystem()
		{
			auto typeName = typeid(T).hash_code();
			return systems.find(typeName) != systems.end();
		}

		void OnUpdate(float dt, Scene* scene);
		void OnImGui();

	private:
		std::unordered_map<size_t, std::shared_ptr<ISystem>> systems;
	};
}
