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

#include "Input.h"
#include "Engine.h"

namespace Raven
{
	std::unique_ptr<Raven::Input> Input::input;

	auto Input::Create() -> void
	{
		input = std::make_unique<Input>();
	}

	auto Input::Reset() -> void
	{
		memset(keyHeld, 0, MAX_KEYS);
		memset(keyPressed, 0, MAX_KEYS);
		memset(mouseClicked, 0, MAX_BUTTONS);
		memset(mouseHeld, 0, MAX_BUTTONS);

		mouseOnScreen = true;
		windowFocus = true;
		scrollOffset = 0.0f;
	}

	auto Input::ResetPressed() -> void
	{
		memset(keyPressed, 0, MAX_KEYS);
		memset(mouseClicked, 0, MAX_BUTTONS);
		scrollOffset = 0;
	}


	Input::Input()
	{
		handler.mouseScrollHandler = std::bind(&Input::OnMouseScrolled, this, std::placeholders::_1);
		handler.mouseClickHandler = std::bind(&Input::OnMousePressed, this, std::placeholders::_1);
		handler.mouseRelaseHandler = std::bind(&Input::OnMouseReleased, this, std::placeholders::_1);
		handler.mouseMoveHandler = std::bind(&Input::OnMouseMoved, this, std::placeholders::_1);
		handler.keyPressedHandler = std::bind(&Input::OnKeyPressed, this, std::placeholders::_1);
		handler.keyReleasedHandler = std::bind(&Input::OnKeyReleased, this, std::placeholders::_1);
		Engine::Get().GetEventDispatcher().AddEventHandler(&handler);
		Reset();
	}



	auto Input::OnKeyPressed(KeyPressedEvent* e) -> bool
	{
		SetKeyPressed(KeyCode::Id(e->GetKeyCode()), e->GetRepeatCount() < 1);
		SetKeyHeld(KeyCode::Id(e->GetKeyCode()), true);
		return false;
	}

	auto Input::OnKeyReleased(KeyReleasedEvent* e) -> bool
	{
		SetKeyPressed(KeyCode::Id(e->GetKeyCode()), false);
		SetKeyHeld(KeyCode::Id(e->GetKeyCode()), false);
		return false;
	}

	auto Input::OnMousePressed(MouseClickEvent* e) -> bool
	{
		SetMouseClicked( KeyCode::MouseKey(e->buttonId), true);
		SetMouseHeld(KeyCode::MouseKey(e->buttonId), true);
		return false;
	}

	auto Input::OnMouseReleased(MouseReleaseEvent* e) -> bool
	{
		SetMouseClicked(KeyCode::MouseKey(e->buttonId), false);
		SetMouseHeld(KeyCode::MouseKey(e->buttonId), false);
		return false;
	}

	auto Input::OnMouseScrolled(MouseScrolledEvent* e) -> bool
	{
		SetScrollOffset(e->GetYOffset());
		return false;
	}

	auto Input::OnMouseMoved(MouseMoveEvent* e) -> bool
	{
		SetMousePosition(e->position);
		return false;
	}

};

