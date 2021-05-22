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

#include "MetaFile.h"
#include "LuaComponent.h"
#include <fstream>
#include "cereal/archives/json.hpp"
#include "Scene/Entity/Entity.h"
#include "Utilities/Serialization.h"
namespace Raven
{
	void MetaFile::Save(const LuaComponent* comp, const std::string & fileName) const
	{
		std::ofstream os(fileName, std::ios::binary);
		cereal::JSONOutputArchive archive(os);

		if (comp->table) 
		{
			for (auto&& pair : luabridge::pairs(*comp->table))
			{
				auto name = pair.first.tostring();
				if (name == "__cname" || name == "__index") {
					continue;
				}

				if (pair.second.isNumber())
				{
					float value = pair.second;
					archive(cereal::make_nvp(name, value));
				}
				else if (pair.second.isString())
				{
					std::string value = pair.second;
					archive(cereal::make_nvp(name, value));
				}
				else if (pair.second.isBool())
				{
					bool value = pair.second;
					archive(cereal::make_nvp(name, value));
				}
				else if (pair.second.isTable())
				{
					//PRINT_FUNC();
					LOGW("Table serialization Not implementation");
				}
				else if (pair.second.isUserdata())
				{
					if (pair.second.isInstance<glm::vec2>())
					{
						glm::vec2* v = pair.second;
						archive(cereal::make_nvp(name, *v));
					}
					else if (pair.second.isInstance<glm::vec3>())
					{
						glm::vec3* v = pair.second;
						archive(cereal::make_nvp(name, *v));
					}
					else if (pair.second.isInstance<glm::vec4>())
					{
						glm::vec4* v = pair.second;
						archive(cereal::make_nvp(name, *v));

					}
					else if ((pair.second.isInstance<Entity>() && name != "entity"))
					{
						Entity * v = pair.second;
						archive(cereal::make_nvp(name, v->GetHandle()));
					}
				}
			}
		}
	}

	void MetaFile::Load(LuaComponent* comp, const std::string& fileName,Scene * scene)
	{
		std::ifstream os(fileName);
		if (os.good()) 
		{
			os.seekg(0, std::ios::end);
			auto len = os.tellg();
			os.seekg(0, std::ios::beg);
			if (len > 0) {
				cereal::JSONInputArchive archive(os);
				auto table = comp->table;

				for (auto&& pair : luabridge::pairs(*comp->table))
				{
					auto name = pair.first.tostring();
					if (pair.second.isNumber())
					{
						float value = pair.second;
						archive(cereal::make_nvp(name, value));
						(*comp->table)[name] = value;
					}
					else if (pair.second.isString())
					{
						std::string value = pair.second;
						archive(cereal::make_nvp(name, value));
						(*comp->table)[name] = value;
					}
					else if (pair.second.isBool())
					{
						bool value = pair.second;
						archive(cereal::make_nvp(name, value));
						pair.second = value;
						(*comp->table)[name] = value;
					}
					else if (pair.second.isTable())
					{
						PRINT_FUNC();
						LOGC("Table serialization Not implementation");
					}
					else if (pair.second.isUserdata())
					{
						if (pair.second.isInstance<glm::vec2>())
						{
							glm::vec2* v = pair.second;
							archive(cereal::make_nvp(name, *v));
						}
						else if (pair.second.isInstance<glm::vec3>())
						{
							glm::vec3* v = pair.second;
							archive(cereal::make_nvp(name, *v));
						}
						else if (pair.second.isInstance<glm::vec4>())
						{
							glm::vec4* v = pair.second;
							archive(cereal::make_nvp(name, *v));

						}
						else if ((pair.second.isInstance<Entity>() && name != "entity"))
						{
							Entity* v = pair.second;
							entt::entity e;
							archive(cereal::make_nvp(name, e));
							v->SetHandle(e);
							v->SetScene(scene);
						}
					}
				}
			}
		}
	}

};