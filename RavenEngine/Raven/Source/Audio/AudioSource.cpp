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

#include "AudioSource.h"
#include "OpenAL/ALAudioSource.h"
#include "Logger/Console.h"

namespace Raven
{

	luabridge::RefCountedPtr<AudioSource> AudioSource::CreateLuaObj()
	{
		return new ALAudioSource();
	}

	AudioSource* AudioSource::Create()
	{
		return new ALAudioSource();
	}
	AudioSource::AudioSource()
	{
		Reset();
	}

	void AudioSource::Reset()
	{
		pitch = 1.0f;
		volume = 1.0f;
		isLooping = true;
		clip = nullptr;
		paused = false;
	}

	AudioSource::~AudioSource()
	{
	}

	void AudioSource::SetAudioClip(AudioClip *s)
	{
		clip = s;
	}
}
