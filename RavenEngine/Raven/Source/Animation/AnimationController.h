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
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <list>
#include <cereal/cereal.hpp>
#include <cereal/types/unordered_map.hpp>
#include <entt/entt.hpp>

#include "Utilities/StringUtils.h"
#include "Animation.h"

namespace Raven
{
	class Animation;
	class Scene;
	class SkinnedMeshComponent;

	struct Condition
	{
		enum class Type
		{
			Float,
			Int,
			Bool,
		};
		std::string value;
		Type type;

		inline operator bool() const {
			return std::stoi(value) == 1; 
		}

		inline bool operator ==(const Condition & con) const {
			return con.type == type && con.value == value;
		}

		inline bool operator !=(const Condition& con) const {
			return con.type != type || con.value != value;
		}

		inline operator int() const
		{
			return std::stoi(value);
		}

		inline operator float() const
		{
			return std::stof(value);
		}

		template <typename Archive>
		void serialize(Archive& archive)
		{
			archive(cereal::make_nvp("value", value));
			archive(cereal::make_nvp("type", type));
		}

	};

	struct Transition
	{
		int32_t id;
		int32_t from;
		int32_t to;
		std::unordered_map<std::string, Condition> conditions;

		template <typename Archive>
		void serialize(Archive& archive)
		{
			archive(cereal::make_nvp("id", id));
			archive(cereal::make_nvp("from", from));
			archive(cereal::make_nvp("to", to));
			archive(cereal::make_nvp("conditions", conditions));
		}

	};

	class AnimationController : public IResource
	{
		AnimationController& operator=(const AnimationController& other) = delete;

	public:
		struct AnimatorNode
		{
			int32_t nodeId;
			int32_t in;//ma
			int32_t out;
			std::string name;
			ResourceRef animClip;

			template <typename Archive>
			void load(Archive& archive)
			{
				archive(cereal::make_nvp("in", in));
				archive(cereal::make_nvp("out", out));
				archive(cereal::make_nvp("nodeId", nodeId));
				archive(cereal::make_nvp("name", name));
				archive(animClip);
			}

			template <typename Archive>
			void save(Archive& archive) const
			{
				archive(cereal::make_nvp("in", in));
				archive(cereal::make_nvp("out", out));
				archive(cereal::make_nvp("nodeId", nodeId));
				archive(cereal::make_nvp("name", name));
				archive(animClip);
			}
		};
		constexpr static int32_t EntryNodeId = 100;

	public:
		AnimationController();
		AnimationController(const AnimationController& other);

		// return the resource type
		inline static EResourceType StaticGetType() noexcept { return EResourceType::RT_AnimationController; }

		inline void FocusTransition(Transition * info) { focusedLink = info; }
		inline auto& GetTransition(int32_t linkId) { return linkInfo[linkId]; }
		inline auto& GetConditions() const { return conditions; }
		inline auto& GetAnimatorNodes() const { return animatorNodes; }
		inline auto& GetLinkInfo() const { return linkInfo; }
		inline auto& GetCurrentLink() const { return currentLink; }
		inline auto& GetCurrAnimation() const { return currentAnimation; }
		
		const std::string GetCurrentAnimationName() const;
		

		void SetWrapMode(const std::string & name, AnimationWrapMode mode);

		void RemoveCondition(const std::string& key);
		void ChangeConditionName(const std::string& old, const std::string& newName);
		void SetValue(const std::string& name, const std::string& value);
		template<typename T>
		T GetValue(const std::string& name);

		void OnImGui();
		void AddCondition(Condition::Type type);

		void Connect(
			const ResourceRef& fromAnimClip, int32_t fromId, int32_t fromIn,int32_t fromOut,
			const ResourceRef& toAnimClip, int32_t toId, int32_t toIn, int32_t toOut, int32_t linkId);
		void RemoveLink(int32_t link);


		template<typename Archive>
		void save(Archive& archive) const
		{
			archive(cereal::make_nvp("animatorNodes", animatorNodes));
			archive(cereal::make_nvp("linkInfo", linkInfo));
			archive(cereal::make_nvp("conditions", conditions));
		}

		template<typename Archive>
		void load(Archive& archive)
		{
			archive(cereal::make_nvp("animatorNodes", animatorNodes));
			archive(cereal::make_nvp("linkInfo", linkInfo));
			archive(cereal::make_nvp("conditions", conditions));

			for (auto & nodes : linkInfo)
			{	
				//first animation node
				//EntryNodeId means the Entry nodeId 
				if (nodes.second.from == EntryNodeId)
				{
					currentNodeId = nodes.second.to;
					currentLink = nodes.first;
				}
			}
			focusedLink = nullptr;
			LoadAnimation();
		}

		void OnUpdate(float dt, SkinnedMeshComponent* skinnedComp);


	private:

		void LoadAnimation();

		Transition* focusedLink = nullptr;

		std::unordered_map<int32_t,AnimatorNode> animatorNodes;
		std::unordered_map<int32_t, Transition> linkInfo;
		std::map<std::string, Condition> conditions;

		//###runtime value
		int32_t currentNodeId = 0;
		int32_t currentLink = 0;
		Ptr<Animation> currentAnimation;
		std::unordered_map<int32_t, Transition>::iterator iterId;
	};

	template<typename T>
	T Raven::AnimationController::GetValue(const std::string& name)
	{
		if (conditions.count(name) > 0)
			return conditions[name];

		return T(0);
	}

	// AnimationControllerInstance:
	//     - instance of the AnimationController resrouce.
	//
	class AnimationControllerInstance
	{
	public:
		// Construct.
		AnimationControllerInstance(Ptr<AnimationController> parentController);

		// Return controller instance.
		inline Ptr<AnimationController> Get() { return instance; }

		// Return the parent controller.
		inline Ptr<AnimationController> GetParentController() { return controller; }

		// Update the controller instance to play the animation.
		void OnUpdate(float dt, SkinnedMeshComponent* skinnedComp);

	private:
		// The original controller.
		Ptr<AnimationController> controller;

		// a copy of the controller resrouce, only used at run-time.
		Ptr<AnimationController> instance;
	};

};