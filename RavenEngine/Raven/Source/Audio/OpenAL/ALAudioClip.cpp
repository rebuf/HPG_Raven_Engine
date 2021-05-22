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
#include "ALAudioClip.h"
#include "Audio/OggLoader.h"

namespace Raven
{
	ALAudioClip::ALAudioClip(const std::string& fileName, const std::string& format)
		: format(0)
	{
		filePath = fileName;
		if(format == "ogg")
			data = LoadOgg(fileName);

		alGenBuffers(1, &buffer);
		alBufferData(buffer, GetOALFormat(data.bitRate, data.channels), data.data, data.size, static_cast<ALsizei>(data.freqRate));
	}

	ALAudioClip::~ALAudioClip()
	{
		alDeleteBuffers(1, &buffer);
	}

	ALenum ALAudioClip::GetOALFormat(uint32_t bitRate, uint32_t channels)
	{
		if(bitRate == 16)
		{
			return channels == 2 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;
		}
		else if(bitRate == 8)
		{
			return channels == 2 ? AL_FORMAT_STEREO8 : AL_FORMAT_MONO8;
		}
		return AL_FORMAT_MONO8;
	}
}
