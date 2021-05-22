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

#include <stdlib.h>
#include <algorithm>
#include "EventDispatcher.h"
#include "Engine.h"

namespace Raven
{
	EventDispatcher::EventDispatcher()
	{
	}

	EventDispatcher::~EventDispatcher()
	{
		for (EventHandler *eventHandler : eventHandlerAddSet)
			eventHandler->eventDispatcher = nullptr;

		for (EventHandler *eventHandler : eventHandlers)
		{
			auto i = std::find(eventHandlerDeleteSet.begin(), eventHandlerDeleteSet.end(), eventHandler);
			if (i == eventHandlerDeleteSet.end()) {
				eventHandler->eventDispatcher = nullptr;
			}
			
		}
	}

	void EventDispatcher::AddEventHandler(EventHandler *eventHandler)
	{
		if (eventHandler->eventDispatcher)
			eventHandler->eventDispatcher->RemoveEventHandler(eventHandler);

		eventHandler->eventDispatcher = this;

		eventHandlerAddSet.insert(eventHandler);

		auto setIterator = eventHandlerDeleteSet.find(eventHandler);

		if (setIterator != eventHandlerDeleteSet.end())
			eventHandlerDeleteSet.erase(setIterator);
	}

	void EventDispatcher::RemoveEventHandler(EventHandler *eventHandler) 
	{
		if (eventHandler->eventDispatcher == this)
			eventHandler->eventDispatcher = nullptr;

		eventHandlerDeleteSet.insert(eventHandler);

		auto setIterator = eventHandlerAddSet.find(eventHandler);

		if (setIterator != eventHandlerAddSet.end())
			eventHandlerAddSet.erase(setIterator);
	}

	bool EventDispatcher::DispatchEvent(std::unique_ptr<Event> &&event)
	{
		if (!event)
			return false;

		bool handled = false;

		for (const EventHandler *eventHandler : eventHandlers)
		{
			auto i = std::find(eventHandlerDeleteSet.begin(), eventHandlerDeleteSet.end(), eventHandler);
			//not find in delete set
			if (i == eventHandlerDeleteSet.end())
			{
				switch (event->GetType())
				{
				case	EventType::WindowClose:break;
				case	EventType::WindowResize:break;
				case	EventType::WindowFocus:break;
				case	EventType::WindowLostFocus:break;

				case	EventType::KeyPressed:
					if (eventHandler->keyPressedHandler) {
						handled = eventHandler->keyPressedHandler(static_cast<KeyPressedEvent*>(event.get()));
					}
					break;
				case	EventType::KeyReleased:
					if (eventHandler->keyReleasedHandler) {
						handled = eventHandler->keyReleasedHandler(static_cast<KeyReleasedEvent*>(event.get()));
					}
					break;
				case	EventType::MouseClicked:
					if (eventHandler->mouseClickHandler)
						handled = eventHandler->mouseClickHandler(static_cast<MouseClickEvent*>(event.get()));
					break;
				case	EventType::MouseReleased:
					if (eventHandler->mouseRelaseHandler)
						handled = eventHandler->mouseRelaseHandler(static_cast<MouseReleaseEvent*>(event.get()));
					break;
				case	EventType::MouseMove:
					if (eventHandler->mouseMoveHandler)
						handled = eventHandler->mouseMoveHandler(static_cast<MouseMoveEvent*>(event.get()));
					break;
				case	EventType::MouseScrolled:
					if (eventHandler->mouseScrollHandler)
						handled = eventHandler->mouseScrollHandler(static_cast<MouseScrolledEvent*>(event.get()));
					break;
				case	EventType::CharInput:
					if (eventHandler->charInputHandler)
						handled = eventHandler->charInputHandler(static_cast<CharInputEvent*>(event.get()));
					break;
				}
			}
			if (handled)//if this event handled,this even will not dispatch in the low priority handler.
				break;
		}
		return handled;
	}

	std::future<bool> EventDispatcher::PostEvent(std::unique_ptr<Event> &&event)
	{
		std::promise<bool> promise;
		std::future<bool> future = promise.get_future();
		std::unique_lock<std::mutex> lock(eventQueueMutex);
		eventQueue.push(std::pair<std::promise<bool>, std::unique_ptr<Event>>(std::move(promise), std::move(event)));
		return future;
	}

	void EventDispatcher::DispatchEvents()
	{
		//# clear handlers that are waiting to be deleted
		for (EventHandler *eventHandler : eventHandlerDeleteSet)
		{
			auto i = std::find(eventHandlers.begin(), eventHandlers.end(), eventHandler);

			if (i != eventHandlers.end())
				eventHandlers.erase(i);
		}

		eventHandlerDeleteSet.clear();

		//# sort with priority
		for (EventHandler *eventHandler : eventHandlerAddSet)
		{
			auto i = std::find(eventHandlers.begin(), eventHandlers.end(), eventHandler);

			if (i == eventHandlers.end())
			{
				auto upperBound = std::upper_bound(eventHandlers.begin(), eventHandlers.end(), eventHandler,
					[](const EventHandler *a, const EventHandler *b) {
						return a->priority > b->priority;
					});
				eventHandlers.insert(upperBound, eventHandler);
			}
		}

		eventHandlerAddSet.clear();

		std::pair<std::promise<bool>, std::unique_ptr<Event>> event;

		// dispatch events
		for (;;)
		{
			std::unique_lock<std::mutex> lock(eventQueueMutex);
			if (eventQueue.empty())
				break;

			event = std::move(eventQueue.front());
			eventQueue.pop();
			lock.unlock();

			event.first.set_value(DispatchEvent(std::move(event.second)));
		}
	}
};