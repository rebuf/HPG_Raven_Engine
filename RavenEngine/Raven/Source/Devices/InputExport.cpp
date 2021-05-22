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
#include "InputExport.h"
#include "Scripts/LuaVirtualMachine.h"
#include <LuaBridge/LuaBridge.h>
#include "Input.h"

namespace Raven
{
	namespace InputExport
	{

#define MAPPING(FUNC_NAME,PARAM_TYPE,REAL_TYPE,RET) #FUNC_NAME , std::function <RET(const PARAM_TYPE &)>([](const PARAM_TYPE  & parm) -> RET{ \
			return Input::GetInput()->FUNC_NAME((REAL_TYPE)parm);								\
		})

#define MAPPING2(FUNC_NAME,RET) #FUNC_NAME , std::function <RET()>([]() -> RET{ \
			return Input::GetInput()->FUNC_NAME();		 \
		})

		void Export(lua_State* L)
		{
			
			static std::vector<std::pair<std::string, KeyCode::Id>> keyItems =
			{
				{"A", KeyCode::Id::A},
				{"B", KeyCode::Id::B},
				{"C", KeyCode::Id::C},
				{"D", KeyCode::Id::D},
				{"E", KeyCode::Id::E},
				{"F", KeyCode::Id::F},
				{"H", KeyCode::Id::G},
				{"G", KeyCode::Id::H},
				{"I", KeyCode::Id::I},
				{"J", KeyCode::Id::J},
				{"K", KeyCode::Id::K},
				{"L", KeyCode::Id::L},
				{"M", KeyCode::Id::M},
				{"N", KeyCode::Id::N},
				{"O", KeyCode::Id::O},
				{"P", KeyCode::Id::P},
				{"Q", KeyCode::Id::Q},
				{"R", KeyCode::Id::R},
				{"S", KeyCode::Id::S},
				{"T", KeyCode::Id::T},
				{"U", KeyCode::Id::U},
				{"V", KeyCode::Id::V},
				{"W", KeyCode::Id::W},
				{"X", KeyCode::Id::X},
				{"Y", KeyCode::Id::Y},
				{"Z", KeyCode::Id::Z},
				{"Space", KeyCode::Id::Space},
				{"Escape", KeyCode::Id::Escape},
				{ "MINUS", KeyCode::Id::Minus },
				{ "PERIOD", KeyCode::Id::Period },
				{ "SLASH", KeyCode::Id::Slash },
				{ "SEMICOLON", KeyCode::Id::Semicolon },
				{ "EQUAL", KeyCode::Id::Equal },
				{ "LEFT_BRACKET", KeyCode::Id::LeftBracket },
				{ "BACKSLASH", KeyCode::Id::Backslash },
				{ "RIGHT_BRACKET", KeyCode::Id::RightBracket },
				{"Enter", KeyCode::Id::Enter},
				{"Tab", KeyCode::Id::Tab},
				{"Backspace", KeyCode::Id::Backspace},
				{"Insert", KeyCode::Id::Insert},
				{"Delete", KeyCode::Id::Delete},
				{"Right", KeyCode::Id::Right},
				{"Left", KeyCode::Id::Left},
				{"Down", KeyCode::Id::Down},
				{"Up", KeyCode::Id::Up},
				{"PageUp", KeyCode::Id::PageUp},
				{"PageDown", KeyCode::Id::PageDown},
				{"Home", KeyCode::Id::Home},
				{"End", KeyCode::Id::End},
				{ "CAPS_LOCK", KeyCode::Id::CapsLock },
				{ "SCROLL_LOCK", KeyCode::Id::ScrollLock },
				{"NumLock", KeyCode::Id::NumLock},
				{"PrintScreen", KeyCode::Id::PrintScreen},
				{"Pause", KeyCode::Id::Pause},
				{"LeftShift", KeyCode::Id::LeftShift},
				{"LeftControl", KeyCode::Id::LeftControl},
				{ "LEFT_ALT", KeyCode::Id::LeftAlt },
				{ "LEFT_SUPER", KeyCode::Id::LeftSuper },
				{"RightShift", KeyCode::Id::RightShift},
				{"RightControl", KeyCode::Id::RightControl},
				{ "RIGHT_ALT", KeyCode::Id::RightAlt },
				{ "RIGHT_SUPER", KeyCode::Id::RightSuper },
				{"Menu", KeyCode::Id::Menu},
				{"F1", KeyCode::Id::F1},
				{"F2", KeyCode::Id::F2},
				{"F3", KeyCode::Id::F3},
				{"F4", KeyCode::Id::F4},
				{"F5", KeyCode::Id::F5},
				{"F6", KeyCode::Id::F6},
				{"F7", KeyCode::Id::F7},
				{"F8", KeyCode::Id::F8},
				{"F9", KeyCode::Id::F9},
				{"F10", KeyCode::Id::F10},
				{"F11", KeyCode::Id::F11},
				{"F12", KeyCode::Id::F12},
				{"Keypad0", KeyCode::Id::D0},
				{"Keypad1", KeyCode::Id::D1},
				{"Keypad2", KeyCode::Id::D2},
				{"Keypad3", KeyCode::Id::D3},
				{"Keypad4", KeyCode::Id::D4},
				{"Keypad5", KeyCode::Id::D5},
				{"Keypad6", KeyCode::Id::D6},
				{"Keypad7", KeyCode::Id::D7},
				{"Keypad8", KeyCode::Id::D8},
				{"Keypad9", KeyCode::Id::D9}
			};

			auto v1 = luabridge::newTable(L);
			for (auto& item : keyItems)
			{
				v1[item.first] = (uint16_t)item.second;
			}
			luabridge::setGlobal(L, v1, "KeyCode");

			static std::vector<std::pair<std::string, KeyCode::MouseKey>> mouseItems =
			{
				{"Left",	 KeyCode::MouseKey::ButtonLeft},
				{"Right",	 KeyCode::MouseKey::ButtonRight},
				{"Middle",	 KeyCode::MouseKey::ButtonMiddle},
			};

			auto v2 = luabridge::newTable(L);
			for (auto& item : keyItems)
			{
				v1[item.first] = (uint16_t)item.second;
			}
			luabridge::setGlobal(L, v2, "MouseKey");

			luabridge::getGlobalNamespace(L)
				.beginNamespace("Input")
				.addFunction(MAPPING(IsKeyHeld, uint16_t, KeyCode::Id, bool))
				.addFunction(MAPPING(IsKeyPressed, uint16_t, KeyCode::Id, bool))
				.addFunction(MAPPING(IsMouseClicked, uint16_t, KeyCode::MouseKey, bool))
				.addFunction(MAPPING(IsMouseHeld, uint16_t, KeyCode::MouseKey, bool))
				.addFunction(MAPPING2(GetMousePosition, glm::vec2))
				.addFunction(MAPPING2(GetScrollOffset, float))
				.endNamespace();
		}
	};
};
