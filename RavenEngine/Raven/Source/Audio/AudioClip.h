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

#include "AudioData.h"
#include <string>
#include <LuaBridge/RefCountedPtr.h>

namespace Raven
{

	/**
	 * interface for different Audio Imp
	 * Now only have openal implementation
	 */

	class  AudioClip
	{
		friend class SoundManager;

	public:
		static AudioClip* Create(const std::string& name, const std::string& extension);
		static luabridge::RefCountedPtr<AudioClip> CreateLuaObj(const std::string& name);
		virtual ~AudioClip();

		inline auto GetData() const
		{
			return data.data;
		}
		int GetBitRate() const
		{
			return data.bitRate;
		}
		inline auto GetFrequency() const
		{
			return data.freqRate;
		}
		inline auto GetChannels() const
		{
			return data.channels;
		}
		inline auto GetSize() const
		{
			return data.size;
		}
		inline auto IsStreaming() const
		{
			return streaming;
		}
		double GetLength() const;

		virtual double StreamData(unsigned int buffer, double timeLeft){return 0.0f;}
		inline auto& GetFilePath() const { return filePath; }

	protected:
		AudioClip();
		bool streaming = false;
		std::string filePath;
		AudioData data;
	};
}
