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
#include <memory>


namespace Raven
{
	class AudioSource;
	class AudioClip;
	class SoundComponent : public Component
	{
	public:
		SoundComponent();
		~SoundComponent();
		void OnImGui();

		inline auto GetSoundNode() const { return soundNode; }

		template<typename Archive>
		void save(Archive& archive) const
		{
			archive(*soundNode.get());
		}

		template<typename Archive>
		void load(Archive& archive)
		{
			auto node = AudioSource::Create();
			archive(*node);
			soundNode.reset(node);
		}

		inline auto & GetAudioSource() { return *soundNode; }

	private:
		//simple to do. 
		std::shared_ptr <AudioSource> soundNode;
		AudioClip * clip = nullptr;
	};


};