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
//////////////////////////////////////////////////////////////////////////////
// This file is part of the Raven Game Engine			                    //
//////////////////////////////////////////////////////////////////////////////

#pragma once
#include "IModule.h"
extern "C" {
# include "lua.h"
# include "lauxlib.h"
# include "lualib.h"
}


#include <string>

namespace Raven 
{
	class LuaVirtualMachine final : public IModule
	{
	public:
		LuaVirtualMachine();
		void Initialize() override;
		void Destroy() override;
		inline auto GetState() { return L; }

		static auto GetModuleType() { return MT_LuaVirtualMachine; }

	private:
		void AddSystemPath(const std::string& path);
		void AddPath(const std::string& path);

		lua_State * L = nullptr;
	};
};