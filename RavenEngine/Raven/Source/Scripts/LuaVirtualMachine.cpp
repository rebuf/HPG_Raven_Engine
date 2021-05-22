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

#include "LuaVirtualMachine.h"
#include "Logger/Console.h"
#include <LuaBridge/LuaBridge.h>
#include <functional>
#include "LuaComponent.h"
#include "Devices/InputExport.h"
#include "MathExport.h"
#include "Devices/Input.h"
#include "Logger/LogExport.h"
#include "Audio/AudioExport.h"
#include "ComponentExport.h"
#include <filesystem>

namespace Raven 
{

	LuaVirtualMachine::LuaVirtualMachine()
	{

	}

	void LuaVirtualMachine::Initialize()
	{
		PRINT_FUNC();
		L = luaL_newstate();
		luaL_openlibs(L);//load all default lua functions
		AddPath(".");
		InputExport::Export(L);
		LogExport::Export(L);
		MathExport::Export(L);
		ComponentExport::Export(L);
		AudioExport::Export(L);
	}

	void LuaVirtualMachine::Destroy()
	{
		lua_close(L);
	}

	void LuaVirtualMachine::AddSystemPath(const std::string& path)
	{
		std::string v;
		lua_getglobal(L, "package");
		lua_getfield(L, -1, "path");
		v.append(lua_tostring(L, -1));
		v.append(";");
		v.append(path.c_str());
		lua_pushstring(L, v.c_str());
		lua_setfield(L, -3, "path");
		lua_pop(L, 2);
	}

	void LuaVirtualMachine::AddPath(const std::string & path)
	{
		AddSystemPath(path + "/?.lua");

		for (const auto& entry : std::filesystem::directory_iterator(path))
		{
			bool isDir = std::filesystem::is_directory(entry);
			if (isDir) {
				AddPath(entry.path().string());
			}
		}
	}

};
