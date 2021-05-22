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
#include "LogExport.h"
#include "Scripts/LuaVirtualMachine.h"
#include <LuaBridge/LuaBridge.h>
#include "Console.h"


namespace Raven
{
	namespace LogExport
	{
	
		void Export(lua_State* L)
		{
			luabridge::getGlobalNamespace(L)
				.addFunction("LOGE", std::function <void(const std::string&)>([](const std::string& str) {
				LOGE(str);
					}))
				.addFunction("LOGV", std::function <void(const std::string&)>([](const std::string& str) {
						LOGV(str);
					}))
						.addFunction("LOGI", std::function <void(const std::string&)>([](const std::string& str) {
						LOGI(str);
							}))
						.addFunction("LOGC", std::function <void(const std::string&)>([](const std::string& str) {
								LOGC(str);
							}));

		}
	};
};
