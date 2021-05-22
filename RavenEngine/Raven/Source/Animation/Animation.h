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


#include "Utilities/Core.h"
#include "ResourceManager/Resources/IResource.h"
#include "AnimationCurve.h"
#include "Animation/Skeleton.h"

#include <vector>
#include <memory>
#include <string>


namespace Raven
{
	class Transform;
	class AnimationController;
	class SkeletonInstance;
	class Skeleton;

	enum class AnimationCurvePropertyType
	{
		Unknown = 0,
		LocalPositionX,
		LocalPositionY,
		LocalPositionZ,
		LocalRotationX,
		LocalRotationY,
		LocalRotationZ,
		LocalScaleX,
		LocalScaleY,
		LocalScaleZ,
		BlendShape
	};

	struct AnimationCurveProperty
	{
		AnimationCurvePropertyType type;
		std::string name;
		AnimationCurve curve;

		// Serialization Load.
		template<class Archive>
		void load(Archive& archive)
		{
			archive(
				EnumAsInt<AnimationCurvePropertyType>(type),
				name,
				curve
			);
		}

		// Serialization Save.
		template<class Archive>
		void save(Archive& archive) const
		{
			archive(
				EnumAsInt<const AnimationCurvePropertyType>(type),
				name,
				curve
			);
		}
	};

	struct AnimationCurveWrapper
	{
		int32_t index;
		std::vector<AnimationCurveProperty> properties;

		// Serialization Load.
		template<class Archive>
		void load(Archive& archive)
		{
			archive(index);
			LoadVector(archive, properties);
		}

		// Serialization Save.
		template<class Archive>
		void save(Archive& archive) const
		{
			archive(index);
			SaveVector(archive, properties);
		}
	};

	enum class AnimationWrapMode
	{
		Default = 0,
		Once = 1,
		Loop = 2,
		PingPong = 4,
		ClampForever = 8,
	};

	class AnimationClip : public IResource 
	{
	public:
		AnimationClip() 
			: IResource()
			, length(0)
			, fps(0)
			, wrapMode(AnimationWrapMode::Default)
		{
			type = AnimationClip::StaticGetType();
		}

		virtual ~AnimationClip()
		{

		}

		// return the resource type
		inline static EResourceType StaticGetType() noexcept { return EResourceType::RT_AnimationClip; }

		std::string clipName;
		float length;
		float fps;
		AnimationWrapMode wrapMode;
		std::vector<AnimationCurveWrapper> curves;

		// The skeleton this animation clip reference.
		Ptr<Skeleton> skeleton;

		// Serialization Load.
		template<class Archive>
		void load(Archive& archive)
		{
			archive(cereal::base_class<IResource>(this));

			archive(
				clipName,
				length,
				fps,
				EnumAsInt<AnimationWrapMode>(wrapMode)
			);

			LoadVector(archive, curves);

			// Load Resrouce Reference -> Skeleton.
			skeleton = ResourceRef::Load(archive).FindOrLoad<Skeleton>();
		}

		// Serialization Save.
		template<class Archive>
		void save(Archive& archive) const
		{
			archive(cereal::base_class<IResource>(this));

			archive(
				clipName,
				length,
				fps,
				EnumAsInt<const AnimationWrapMode>(wrapMode)
			);

			SaveVector(archive, curves);

			// Save Resrouce Reference -> Skeleton.
			ResourceRef::Save(archive, skeleton.get());
		}
	};

	enum class FadeState
	{
		In,
		Normal,
		Out,
	};




	struct AnimationState
	{
		int32_t clipIndex;
		float playStartTime;
		std::vector<int32_t> targets;
		FadeState fadeState;
		float fadeStartTime;
		float fadeLength;
		float startWeight;
		float weight;
		float playingTime;
	};

	//
	class Animation
	{
	public:
		Animation();
		virtual ~Animation() { }

		// Set Clips to be played by this animation.
		void SetClips(const std::vector< Ptr<AnimationClip> >& clips);

		// Add a clip to be played by this animation.
		void AddClip(const Ptr<AnimationClip>& clip);

		const std::string& GetClipName(int32_t index) const;
		float GetClipLength(int32_t index) const;
		float GetCurrentClipLength() const;
		int32_t GetPlayingClip() const;
		float GetPlayingTime() const;
		void SetPlayingTime(float time);


		void SetPlayMode(AnimationWrapMode mode);

	
		inline auto IsPaused() const { return paused; }
		inline auto IsStarted() const { return started; }
		inline auto IsPlaying() const { return !paused && !stopped; }
		inline auto GetStates() const { return states.size(); }

		inline auto IsStopped() const { return stopped; }
		inline auto GetClipCount() const { return clips.size(); }
		inline auto GetTime() const { return time; }

		void Play(int32_t index, SkeletonInstance* inSkeletonInstance, float fadeLength = 0.3f);
		void Stop();
		void Pause();

		void OnUpdate(float dt);

		// Serialization Save.
		template<typename Archive>
		void save(Archive& archive) const
		{
			uint32_t numClips = (uint32_t)clips.size();
			archive(numClips);

			// Save Resrouce References -> Animation Clips.
			for (uint32_t i = 0; i < numClips; ++i)
			{
				ResourceRef::Save(archive, clips[i]);
			}
		}

		// Serialization Load.
		template<typename Archive>
		void load(Archive& archive)
		{
			uint32_t numClips = 0;
			archive(numClips);
			clips.resize(numClips);

			// Load Resrouce References -> Animation Clips.
			for (uint32_t i = 0; i < numClips; ++i)
			{
				clips[i] = ResourceRef::Load(archive).FindOrLoad<AnimationClip>();
			}
		}

	private:
		void UpdateTime(float dt);
		void Sample(AnimationState & state, float time, float weight, bool firstState, bool lastState);

	private:
		// Resrouces -> Animation Clips.
		std::vector< Ptr<AnimationClip> > clips;

		std::vector<AnimationState> states;
		float time = 0;
		float seekTo = -1;
		bool paused = false;
		bool stopped = true;	
		bool started = false;

		// The skeleton this animation currently updating.
		SkeletonInstance* skeletonInstance;
	};

}