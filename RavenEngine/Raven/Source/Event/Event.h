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
#include <string>
#include <glm/glm.hpp>
#include "Devices/KeyCodes.h"

//some events


#define GENERATE_EVENT_CLASS_TYPE(type) static EventType GetEventType() { return EventType::type; }\
								virtual auto GetType() const  -> EventType  override { return GetEventType(); } \
								virtual auto GetName() const -> const char * override { return #type; }

namespace Raven
{
	enum class EventType 
	{
		None,
		WindowClose,
		WindowResize,
		WindowFocus,
		WindowLostFocus,
		Tick,
		Update,
		Render,
		KeyPressed,
		KeyReleased,
		CharInput,
		MouseClicked,
		MouseReleased,
		MouseMove,
		MouseScrolled,
	};

	class Event 
	{
	public:
		virtual auto GetType() const->EventType = 0;
		virtual auto GetName() const -> const char* = 0;
	};

	class MouseMoveEvent : public Event
	{
	public:
		MouseMoveEvent(float x, float y) :
		position(x, y)
		{
		}
		glm::vec2 position;
		GENERATE_EVENT_CLASS_TYPE(MouseMove);
	};


	class MouseClickEvent : public Event
	{
	public:
		MouseClickEvent(int32_t button, float x, float y) : 
			buttonId(button),position(x,y)
		{
		}
		int32_t buttonId = -1;
		glm::vec2 position;
		GENERATE_EVENT_CLASS_TYPE(MouseClicked);
	};

	class MouseReleaseEvent : public Event
	{
	public:
		MouseReleaseEvent(int32_t button, float x, float y) :
			buttonId(button), position(x, y)
		{
		}
		int32_t buttonId = -1;
		glm::vec2 position;
		GENERATE_EVENT_CLASS_TYPE(MouseReleased);
	};

	class MouseScrolledEvent : public Event
	{
	public:
		MouseScrolledEvent(float xOffset, float yOffset,float x,float y)
			: xOffset(xOffset), yOffset(yOffset) ,position(x,y){}

		inline auto GetXOffset() const { return xOffset; }
		inline auto GetYOffset() const { return yOffset; }

		GENERATE_EVENT_CLASS_TYPE(MouseScrolled);
	private:
		float xOffset;
		float yOffset;
		glm::vec2 position;
	};

	class KeyEvent : public Event
	{
	public:
		inline auto GetKeyCode() const { return keyCode; }

	protected:
		KeyEvent(KeyCode::Id keycode)
			: keyCode(keycode) {}

		KeyCode::Id keyCode;
	};

	class KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(KeyCode::Id keycode, int repeatCount)
			: KeyEvent(keycode), repeatCount(repeatCount) {}

		inline auto GetRepeatCount() const { return repeatCount; }


		GENERATE_EVENT_CLASS_TYPE(KeyPressed);
	private:
		int32_t repeatCount;
	};

	class KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(KeyCode::Id keycode)
			: KeyEvent(keycode) {}

		GENERATE_EVENT_CLASS_TYPE(KeyReleased);
	};

	class CharInputEvent : public KeyEvent
	{
	public:
		CharInputEvent(KeyCode::Id keycode,char ch)
			: KeyEvent(keycode),character(ch) {}


		GENERATE_EVENT_CLASS_TYPE(CharInput);
	public:
        inline auto& GetCharacter() const { return character; }
        inline auto SetCharacter(char character) { this->character = character; }
	private:
		char character;
	};

};