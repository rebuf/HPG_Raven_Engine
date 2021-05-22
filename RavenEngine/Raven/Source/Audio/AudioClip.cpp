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


#include "AudioClip.h"
#include "OpenAL/ALAudioClip.h"
#include "Utilities/StringUtils.h"
#include "Logger/Console.h"
namespace Raven
{
	AudioClip::AudioClip()
	{
	}

	AudioClip::~AudioClip()
	{
		delete[] data.data;
	}

	AudioClip* AudioClip::Create(const std::string& name, const std::string& extension)
	{
		return new ALAudioClip(name, extension);
	}

	luabridge::RefCountedPtr<AudioClip> AudioClip::CreateLuaObj(const std::string& name)
	{
		return new ALAudioClip(name, StringUtils::GetExtension(name));
	}

	double AudioClip::GetLength() const
	{
		return data.length;
	}
}
