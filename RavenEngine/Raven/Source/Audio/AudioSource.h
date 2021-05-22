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
#pragma once

#include "AudioClip.h"
#include "Utilities/StringUtils.h"
#include <glm/glm.hpp>
#include <LuaBridge/RefCountedPtr.h>

namespace Raven
{
	class AudioSource
	{
	public:

		static luabridge::RefCountedPtr<AudioSource> CreateLuaObj();
		static AudioSource* Create();
		AudioSource();
		virtual ~AudioSource();
		void Reset();

		AudioClip* GetClip() const { return clip; }

		inline auto SetVelocity(const glm::vec3& vel) { velocity = vel; }
		inline auto GetVelocity() const { return velocity; }
		inline auto SetVolume(float volume) { this->volume = std::min(1.0f, std::max(0.0f, volume)); }
		inline auto GetVolume() const { return volume; }
		inline auto SetLooping(bool state) { isLooping = state; }
		inline auto GetLooping() const { return isLooping; }
		inline auto SetPaused(bool state) { paused = state; }
		inline auto GetPaused() const { return paused; }
		inline auto GetPitch() const { return pitch; }
		inline auto SetPitch(float value) { pitch = value; }

		virtual void OnUpdate(float msec) = 0;
		virtual void Pause() = 0;
		virtual void Resume() = 0;
		virtual void Stop() = 0;
		virtual void SetAudioClip(AudioClip* s);
		
		
		template<typename Archive>
			void save(Archive& archive) const
		{
			archive(
				cereal::make_nvp("Pitch", pitch), 
				cereal::make_nvp("Volume",    volume),
				cereal::make_nvp("Looping",   isLooping), 
				cereal::make_nvp("Paused", paused)
				
			);
		}									 
		
		template<typename Archive>
		void load(Archive& archive)
		{
			archive(
				cereal::make_nvp("Pitch", pitch), 
				cereal::make_nvp("Volume",volume), 
				cereal::make_nvp("Looping",isLooping), 
				cereal::make_nvp("Paused",paused)
				);
			
	
		}

	protected:

		AudioClip * clip = nullptr;
		glm::vec3 velocity = { 0.f,0,0 };
		float volume = 1.f;
		float pitch = 1.;
		bool isLooping = true;
		bool paused = false;
		bool managedByComponent = false;

	};

}
