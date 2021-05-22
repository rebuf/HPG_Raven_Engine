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
#include "AudioSystem.h"
#include "Engine.h"
#include "Scene/SceneManager.h"
#include "Scene/Scene.h"
#include "Scene/Entity/Entity.h"

#include "Scene/Component/SoundComponent.h"
#include "AudioSource.h"
#include "OpenAL/ALSystemImpl.h"

namespace Raven
{
    AudioSystem* AudioSystem::Create()
    {
        return new AudioSystemImpl();
    }
	
	void AudioSystem::SetPaused(bool paused)
	{
		this->paused = paused;
		auto scene = Engine::Get().GetModule<SceneManager>()->GetCurrentScene();
        if(!scene)
            return;

		auto soundsView = scene->GetRegistry().view<SoundComponent>();
		
		if(paused)
		{
			for (auto entity : soundsView)
				soundsView.get<SoundComponent>(entity).GetSoundNode()->Stop();
		}
		else
		{
			for(auto entity : soundsView)
			{
				auto soundNode = soundsView.get<SoundComponent>(entity).GetSoundNode();
                if(!soundNode->GetPaused())
					soundNode->Resume();
			}
		}
	}
}
