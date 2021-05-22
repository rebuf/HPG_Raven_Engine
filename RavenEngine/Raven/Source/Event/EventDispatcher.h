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
#include <future>
#include "Event.h"
#include "EventHandler.h"
#include "Utilities/Core.h"

namespace Raven 
{
	class EventDispatcher final
	{
	public:
		EventDispatcher();
		~EventDispatcher();

		void AddEventHandler(EventHandler* handler);
		void RemoveEventHandler(EventHandler* handler);
		bool DispatchEvent(std::unique_ptr<Event>&& event);
		std::future<bool> PostEvent(std::unique_ptr<Event>&& event);
		void DispatchEvents();

	private:
		std::vector<EventHandler *> eventHandlers;
		std::set<EventHandler *> eventHandlerAddSet;
		std::set<EventHandler *> eventHandlerDeleteSet;

		std::mutex eventQueueMutex;
		std::queue<std::pair<std::promise<bool>, std::unique_ptr<Event>>> eventQueue;
	};

};

