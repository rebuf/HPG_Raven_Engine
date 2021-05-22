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
#include <memory>
#include "Component.h"

namespace Raven
{
	class CameraController;

	class CameraControllerComponent : public Component
	{
	public:
		enum class ControllerType : int32_t
		{
			FPS = 0,
			EditorCamera,
			Custom
		};

		static std::string TypeToString(ControllerType type);
		static ControllerType StringToType(const std::string& type);

		CameraControllerComponent()
			: type(ControllerType::FPS)
		{
		}

		CameraControllerComponent(ControllerType type);
	
		void SetControllerType(CameraControllerComponent::ControllerType type);

		inline auto & GetController() {return cameraController;}

		template<typename Archive>
		void save(Archive& archive) const
		{
			archive(cereal::base_class<Component>(this));
			archive(cereal::make_nvp("ControllerType", type));
		}

		template<typename Archive>
		void load(Archive& archive)
		{
			archive(cereal::base_class<Component>(this));
			archive(cereal::make_nvp("ControllerType",type));
			SetControllerType(type);
		}

		inline auto GetType() const 
		{
			return type;
		}

	private:
		ControllerType type = ControllerType::FPS;
		std::shared_ptr<CameraController> cameraController;
	};
};