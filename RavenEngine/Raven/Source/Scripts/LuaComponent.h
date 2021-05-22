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
extern "C" {
# include "lua.h"
# include "lauxlib.h"
# include "lualib.h"
}
#include <LuaBridge/LuaBridge.h>
#include <entt/entt.hpp>
#include "Engine.h"
#include "Scene/SceneManager.h"
#include "MetaFile.h"
#include "Scene/Component/Component.h"

namespace Raven 
{
	class Scene;
	class LuaComponent : public Component
	{
	public:
		friend class MetaFile;
		LuaComponent(const std::string& file, Scene* scene);
		LuaComponent();
		~LuaComponent();
		void OnInit();
		void OnUpdate(float dt);
		void Reload();
		void LoadScript();

		template<typename Archive>
		void save(Archive& archive) const
		{
			archive(cereal::base_class<Component>(this));
			archive(
				cereal::make_nvp("FilePath", file)
			);

			metaFile.Save(this,file + ".meta");
		}

		template<typename Archive>
		void load(Archive& archive)
		{
			scene = Engine::Get().GetModule<SceneManager>()->GetCurrentScene();
			archive(cereal::base_class<Component>(this));
			archive(
				cereal::make_nvp("FilePath", file)
			);
			Init();
		}

		void OnImGui();
		bool IsLoaded();
		void SetFilePath(const std::string& fileName);

		inline auto& GetFileName() const { return file; }

		void SetScene(Scene* scene) { this->scene = scene; }

	private:

		void SaveNewFile(const std::string & fileName);
		void Init();
		std::string file;
		std::string className;

		std::shared_ptr<luabridge::LuaRef> table;
		std::shared_ptr<luabridge::LuaRef> onInitFunc;
		std::shared_ptr<luabridge::LuaRef> onUpdateFunc;
		Scene* scene = nullptr;

		MetaFile metaFile;
	};

};