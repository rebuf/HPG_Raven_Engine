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
#include "Animation.h"

#include "Skeleton.h"
#include "Math/MathUtils.h"

#include "Scene/Component/Transform.h"

#include <algorithm>
#include <glm/glm.hpp>


namespace Raven
{
	Animation::Animation()
	{

	}


	void Animation::SetClips(const std::vector<std::shared_ptr<AnimationClip>>& clips)
	{
		this->clips = clips;
		states.clear();
	}

	void Animation::AddClip(const Ptr<AnimationClip>& clip)
	{
		clips.emplace_back(clip);
		states.clear();
	}

	const std::string& Animation::GetClipName(int32_t index) const
    {
        return clips[index]->clipName;
    }

    float Animation::GetClipLength(int32_t index) const
    {
        return clips[index]->length;
    }

	float Animation::GetCurrentClipLength() const
	{
		return clips[GetPlayingClip()]->length;
	}

	int32_t Animation::GetPlayingClip() const
    {
		if (states.empty() || stopped)
		{
			return -1;
		}
        return states.back().clipIndex;
    }

    float Animation::GetPlayingTime() const
    {
		if (states.empty() || stopped)
		{
			return 0;
		}

		return states.back().playingTime;
    }

    void Animation::SetPlayingTime(float t)
    {
		auto playingClip = GetPlayingClip();
		if (playingClip >= 0)
		{
            t = std::clamp(t, 0.0f, GetClipLength(playingClip));
			float offset = t - this->GetPlayingTime();
			seekTo = time + offset;
		}
    }

	void Animation::SetPlayMode(AnimationWrapMode mode)
	{
		for (auto clip : clips)
		{
			clip->wrapMode = mode;
		}
	}

	void Animation::Play(int32_t index, SkeletonInstance* inSkeletonInstance, float fadeLength)
    {
		started = true;
		skeletonInstance = inSkeletonInstance;
		RAVEN_ASSERT(skeletonInstance->GetParent() == clips[index]->skeleton.get(), "Skeleton Mismatch.");

		if (paused)
		{
			paused = false;
			if (index == GetPlayingClip())
			{
				return;
			}
		}

		if (states.empty())
		{
			fadeLength = 0;
		}

		if (fadeLength > 0.0f)
		{
			for (auto& state : states)
			{
				state.fadeState = FadeState::Out;
				state.fadeStartTime = this->GetTime();
				state.fadeLength = fadeLength;
				state.startWeight = state.weight;
			}
		}
		else
		{
			states.clear();
		}

		AnimationState state;
		state.clipIndex = index;
		state.playStartTime =GetTime();
		state.fadeStartTime =GetTime();
		state.fadeLength = fadeLength;
		if (fadeLength > 0.0f)
		{
			state.fadeState = FadeState::In;
			state.startWeight = 0.0f;
			state.weight = 0.0f;
		}
		else
		{
			state.fadeState = FadeState::Normal;
			state.startWeight = 1.0f;
			state.weight = 1.0f;
		}
		state.playingTime = 0.0f;
		states.emplace_back(state);
		paused = false;
		stopped = false;
    }


    void Animation::Stop()
    {
		paused = false;
		stopped = true;
    }


    void Animation::Pause()
    {
		if (!stopped)
		{
			paused = true;
		}
    }


    void Animation::OnUpdate(float dt)
    {
		UpdateTime(dt);

		bool firstState = true;

		for (auto i = states.begin(); i != states.end(); )
		{
			auto& state = *i;
			float time = this->GetTime() - state.playStartTime;
			const auto& clip = *clips[state.clipIndex];
			bool removeLater = false;

			if (time >= clip.length)
			{
				switch (clip.wrapMode)
				{
				case AnimationWrapMode::Once:
					time = clip.length;
					removeLater = true;
					break;
				case AnimationWrapMode::Loop:
					time = std::fmod(time, clip.length);
					break;
				case AnimationWrapMode::PingPong:
					if ((int)(time / clip.length) % 2 == 1)
					{
						// backward
						time = clip.length - fmod(time, clip.length);
					}
					else
					{
						// forward
						time = fmod(time, clip.length);
					}
					break;
				case AnimationWrapMode::Default:
				case AnimationWrapMode::ClampForever:
					time = clip.length;
					break;
				}

			}
			state.playingTime = time;

			if (stopped)
			{
				state.playingTime = 0;
			}

			float fadeTime = this->GetTime() - state.fadeStartTime;
			switch (state.fadeState)
			{
			case FadeState::In:
				if (fadeTime < state.fadeLength)
				{
					state.weight = MathUtils::Lerp(state.startWeight, 1.0f, fadeTime / state.fadeLength);
				}
				else
				{
					state.fadeState = FadeState::Normal;
					state.fadeStartTime = this->GetTime();
					state.startWeight = 1.0f;
					state.weight = 1.0f;
				}

				if (stopped)
				{
					state.fadeState = FadeState::Normal;
					state.fadeStartTime = this->GetTime();
					state.startWeight = 1.0f;
					state.weight = 1.0f;
				}
				break;
			case FadeState::Normal:
				break;
			case FadeState::Out:
				if (fadeTime < state.fadeLength)
				{
					state.weight = MathUtils::Lerp(state.startWeight, 1.0f, fadeTime / state.fadeLength);
				}
				else
				{
					state.weight = 0.0f;
					removeLater = true;
				}

				if (stopped)
				{
					state.weight = 0.0f;
					removeLater = true;
				}
				break;
			}

			bool lastState = false;
			auto j = i;
			if (++j == states.end())
			{
				lastState = true;
			}

			Sample(state, state.playingTime, state.weight, firstState, lastState);
	
			firstState = false;

			if (skeletonInstance)
			{
				skeletonInstance->UpdateBones();
			}

			if (removeLater)
			{
				i = states.erase(i);

				if (states.empty())
				{
					paused = true;
				}
			}
			else
			{
				++i;
			}
		}

		if (stopped)
		{
			states.clear();
		}
    }

    void Animation::UpdateTime(float dt)
    {
		if (seekTo >= 0)
		{
			time = seekTo;
			seekTo = -1;
		}
		else
		{
			if (!paused)
			{
				time += dt;
			}
		}
    }
   
	void Animation::Sample(AnimationState& state, float time, float weight, bool firstState, bool lastState)
	{
		const auto& clip = *clips[state.clipIndex];
		if (state.targets.size() == 0)
		{
			state.targets.resize(clip.curves.size(), -1);
		}

		for (int i = 0; i < clip.curves.size(); ++i)
		{
			const auto& curve = clip.curves[i];

			Bone* target = nullptr;

			// Get Target...
			if (state.targets[i] == -1)
			{
				if (clips[state.clipIndex]->skeleton->IsValidBoneIndex(curve.index))
				{
					state.targets[i] = curve.index;
					target = &clips[state.clipIndex]->skeleton->GetBone(curve.index);
				}
			}
			else
			{
				target = &clips[state.clipIndex]->skeleton->GetBone(state.targets[i]);
			}

			// Invalid Target?
			if (!target)
				continue;


			glm::vec3 localPos(0);
			glm::vec3 localRot(0);
			glm::vec3 localscale(0);
			bool setPos = false;
			bool setRot = false;
			bool setScale = false;

			for (int j = 0; j < curve.properties.size(); ++j)
			{
				auto type = curve.properties[j].type;
				float value = curve.properties[j].curve.Evaluate(time);

				switch (type)
				{
				case AnimationCurvePropertyType::LocalPositionX:
					localPos.x = value;
					setPos = true;
					break;
				case AnimationCurvePropertyType::LocalPositionY:
					localPos.y = value;
					setPos = true;
					break;
				case AnimationCurvePropertyType::LocalPositionZ:
					localPos.z = value;
					setPos = true;
					break;

				case AnimationCurvePropertyType::LocalRotationX:
					localRot.x = value;
					setRot = true;
					break;
				case AnimationCurvePropertyType::LocalRotationY:
					localRot.y = value;
					setRot = true;
					break;
				case AnimationCurvePropertyType::LocalRotationZ:
					localRot.z = value;
					setRot = true;
					break;
				}

				if (setPos)
				{
					glm::vec3 pos;
					if (firstState)
					{
						pos = localPos * weight;
					}
					else
					{
						pos = target->GetPosition() + localPos * weight;
					}
					target->SetPosition(pos);
				}
				if (setRot)
				{
					glm::vec3 rot;
					if (firstState)
					{
						rot = glm::radians(localRot * weight);
					}
					else
					{
						rot = target->GetRotation() + glm::radians(localRot * weight);
					}
					target->SetRotation(rot);
				}
			}
		}
	}
};