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
#include <cstdint>
#include <queue>
#include <functional>
#include <set>

#include "Event.h"


namespace Raven
{
	class EventDispatcher;

	class EventHandler final
	{
	public:
		friend EventDispatcher;
		static constexpr int32_t PRIORITY_MAX = 0x1000;

		explicit EventHandler(int32_t initPriority = 0) : priority(initPriority)
		{
		}

		~EventHandler();

		std::function<bool(MouseMoveEvent*)> mouseMoveHandler;
		std::function<bool(MouseClickEvent*)> mouseClickHandler;
		std::function<bool(MouseReleaseEvent*)> mouseRelaseHandler;
		std::function<bool(MouseScrolledEvent*)> mouseScrollHandler;

		std::function<bool(KeyPressedEvent*)> keyPressedHandler;
		std::function<bool(KeyReleasedEvent*)> keyReleasedHandler;
		std::function<bool(CharInputEvent*)> charInputHandler;


		auto Remove() -> void;

	private:
		int32_t priority;
		EventDispatcher *eventDispatcher = nullptr;
	};
};

