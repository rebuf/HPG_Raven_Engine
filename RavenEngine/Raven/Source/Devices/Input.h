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
#include <glm/glm.hpp>
#include "Event/Event.h"
#include "KeyCodes.h"
#include "Event/EventHandler.h"

#define MAX_KEYS	1024
#define MAX_BUTTONS	32

namespace Raven
{
	class Event;

	class Input
	{
	public:
		virtual ~Input() = default;
		Input();
		static auto& GetInput() { return input; }
		static auto Create() -> void;

		inline auto IsKeyPressed(KeyCode::Id key)   const { return keyPressed[int32_t(key)]; }
		inline auto IsKeyHeld(KeyCode::Id key)      const { return keyHeld[int32_t(key)]; }
		inline auto IsMouseClicked(KeyCode::MouseKey key) const { return mouseClicked[int32_t(key)]; }
		inline auto IsMouseHeld(KeyCode::MouseKey key)    const { return mouseHeld[int32_t(key)]; }

		inline auto SetKeyPressed(KeyCode::Id key, bool a) { keyPressed[int32_t(key)] = a; }
		inline auto SetKeyHeld(KeyCode::Id key, bool a) { keyHeld[int32_t(key)] = a; }
		inline auto SetMouseClicked(KeyCode::MouseKey key, bool a) { mouseClicked[int32_t(key)] = a; }
		inline auto SetMouseHeld(KeyCode::MouseKey key, bool a) { mouseHeld[int32_t(key)] = a; }


		auto Reset() -> void;
		auto ResetPressed() -> void;

	protected:

		static std::unique_ptr<Input> input;

		auto OnKeyPressed(KeyPressedEvent* e) -> bool;
		auto OnKeyReleased(KeyReleasedEvent* e)-> bool;
		auto OnMousePressed(MouseClickEvent* e)-> bool;
		auto OnMouseReleased(MouseReleaseEvent* e)-> bool;
		auto OnMouseScrolled(MouseScrolledEvent* e)-> bool;
		auto OnMouseMoved(MouseMoveEvent* e)-> bool;

		bool keyPressed[MAX_KEYS];
		bool keyHeld[MAX_KEYS];

		bool mouseHeld[MAX_BUTTONS];
		bool mouseClicked[MAX_BUTTONS];

		float scrollOffset = 0.0f;

		bool mouseOnScreen;
		bool windowFocus;
		EventHandler handler;
		glm::vec2 mousePosition;
	public:
		inline auto& GetScrollOffset() const { return scrollOffset; }
		inline auto SetScrollOffset(float scrollOffset) { this->scrollOffset = scrollOffset; }

		inline auto& IsMouseOnScreen() const { return mouseOnScreen; }
		inline auto SetMouseOnScreen(bool mouseOnScreen) { this->mouseOnScreen = mouseOnScreen; }

		inline auto& IsWindowFocus() const { return windowFocus; }
		inline auto SetWindowFocus(bool windowFocus) { this->windowFocus = windowFocus; }

		inline auto& GetMousePosition() const { return mousePosition; }
		inline auto SetMousePosition(const glm::vec2& mousePosition) { this->mousePosition = mousePosition; }

	};
};
