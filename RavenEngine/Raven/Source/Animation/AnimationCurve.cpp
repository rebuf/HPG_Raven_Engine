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
#include "AnimationCurve.h"
#include "Math/MathUtils.h"

namespace Raven
{

    AnimationCurve AnimationCurve::Linear(float timeStart, float valueStart, float timeEnd, float valueEnd)
    {
		AnimationCurve curve;
		float tangent = (valueEnd - valueStart) / (timeEnd - timeStart);
		curve.AddKey(timeStart, valueEnd, tangent, tangent);
		curve.AddKey(timeEnd, valueEnd, tangent, tangent);
		return curve;
    }

    void AnimationCurve::AddKey(float time, float value, float inTangent, float outTangent)
    {
		keys.push_back({ time, value, inTangent, outTangent });
    }

    float AnimationCurve::Evaluate(float time) const
    {
		if (keys.empty())
		{
			return 0;
		}

		const auto& back = keys.back();
		if (time >= back.time)
		{
			return back.value;
		}

		for (int i = 0; i < keys.size(); ++i)
		{
			const auto& key = keys[i];

			if (time < key.time)
			{
				if (i == 0)
				{
					return key.value;
				}
				else
				{
					return Evaluate(time, keys[i - 1], key);
				}
			}
		}

		return 0;
    }

    float AnimationCurve::Evaluate(float time, const Key& k0, const Key& k1)
    {

		//result.Time = k0.time + (k1.time - k0.time) * alpha;
		//AnimationUtils::Interpolate(a.Value, b.Value, alpha, result.Value);
		float dt = std::abs((time - k0.time) / (k1.time - k0.time));
		return MathUtils::Lerp(k0.value, k1.value, dt, false);
    }
};