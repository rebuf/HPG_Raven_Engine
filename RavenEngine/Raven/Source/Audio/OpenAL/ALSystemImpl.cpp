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
#include "ALSystemImpl.h"
#include "ALAudioSource.h"
#include "Core/Camera.h"
#include "Logger/Console.h"
#include "Scene/Scene.h"
#include "Scene/Entity/Entity.h"

#include "Scene/Component/Transform.h"
#include "Scene/Component/SoundComponent.h"
#include <glm/gtc/quaternion.hpp>
#include <imgui/imgui.h>

namespace Raven
{
	
		AudioSystemImpl::AudioSystemImpl(int32_t numChannels) 
			: numChannels(numChannels)
		{

		}

		AudioSystemImpl::~AudioSystemImpl()
		{
			alcDestroyContext(context);
			alcCloseDevice(device);
		}

		void AudioSystemImpl::OnInit()
		{
			LOGV("devices: {0}", alcGetString(nullptr, ALC_DEVICE_SPECIFIER));

			device = alcOpenDevice(nullptr);

			if(!device)
				LOGV("init audio fail");

			LOGV("device: {0}", alcGetString(device, ALC_DEVICE_SPECIFIER));

			context = alcCreateContext(device, nullptr);
			alcMakeContextCurrent(context);
			alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);
		}

		void AudioSystemImpl::OnUpdate(float dt,Scene* scene)
		{
			auto& registry = scene->GetRegistry();
			auto cameraView = registry.view<Camera>();
			if(!cameraView.empty())
			{
				listener = &registry.get<Camera>(cameraView.front());
			}
			UpdateListener();

            auto soundsView = registry.view<SoundComponent>();
          
			for(auto entity : soundsView)
                soundsView.get<SoundComponent>(entity).GetSoundNode()->OnUpdate(dt);
            }

		void AudioSystemImpl::UpdateListener()
		{
			if(listener)
			{
				//TODO . pass camera transform. however, current camera did not calculate velocity;
			}
		}

		void AudioSystemImpl::OnImGui()
		{
		
		}
}
