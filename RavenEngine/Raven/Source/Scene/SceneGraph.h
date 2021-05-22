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
#include <string>
#include "Utilities/Core.h"

#include <entt/entity/fwd.hpp>
#include <cereal/cereal.hpp>

namespace Raven
{
	class SceneGraph final
	{
	public:
		SceneGraph() = default;
		~SceneGraph() = default;

		void Init(entt::registry & registry);
		void DisconnectOnConstruct(bool disable, entt::registry & registry);
		void Update(entt::registry & registry);
		void UpdateTransform(entt::entity entity, entt::registry & registry);
	};

};