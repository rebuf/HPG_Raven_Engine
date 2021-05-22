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
#include "Scene/System/ISystem.h"
#include <vector>
#include <memory>

namespace Raven
{
	class Camera;
	class AudioSource;

	class AudioSystem : public ISystem
	{
	public:
		static AudioSystem* Create();

		virtual ~AudioSystem() = default;
		virtual void OnInit() override = 0;
		virtual void OnUpdate(float dt, Scene* scene) override = 0;

		inline auto GetListener() const	{	return listener;	}

		inline auto AddSoundNode(const std::shared_ptr<AudioSource> & node)
		{
			soundNodes.emplace_back(node);
		}

		inline auto Clear()
		{
			soundNodes.clear();
		}
        
        bool isPaused() const { return paused; }
        void SetPaused(bool paused);

	protected:
		Camera* listener;
		std::vector<std::shared_ptr<AudioSource>> soundNodes;
        bool paused;
	};
}
