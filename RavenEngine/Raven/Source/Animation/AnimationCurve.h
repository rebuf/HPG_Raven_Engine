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

#include <vector>

namespace Raven
{
	class AnimationCurve
	{
	private:
		struct Key
		{
			float time;
			float value;
			float inTangent;
			float outTangent;
		};



	public:
		static AnimationCurve Linear(float timeStart, float valueStart, float timeEnd, float valueEnd);
		void AddKey(float time, float value, float inTangent, float outTangent);
		float Evaluate(float time) const;

		template<class Archive>
		void load(Archive& archive)
		{
			LoadVectorBinary(archive, keys);
		}

		// Serialization Save.
		template<class Archive>
		void save(Archive& archive) const
		{
			SaveVectorBinary(archive, keys);
		}


	private:
		static float Evaluate(float time, const Key& k0, const Key& k1);

	private:
		std::vector<Key> keys;

	};
};