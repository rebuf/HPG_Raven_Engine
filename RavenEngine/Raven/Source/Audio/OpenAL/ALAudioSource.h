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

#include "Audio/AudioSource.h"

#include <AL/al.h>

#define NUM_STREAM_BUFFERS 3

namespace Raven
{
	class ALAudioSource : public AudioSource
	{
	public:
		ALAudioSource();
		virtual ~ALAudioSource();

		void OnUpdate(float msec) override;
		void Pause() override;
		void Resume() override;
		void Stop() override;
		void SetAudioClip(AudioClip *s) override;

	private:
		ALuint source;
		ALuint streamBuffers[NUM_STREAM_BUFFERS];
	};
}