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

#include "Window/Window.h"
#include "Event/Event.h"
#include "Event/WindowEvent.h"

#include <GL/glew.h>
#include "Render/RenderModule.h"

#include <GLFW/glfw3.h>
#include "Engine.h"



namespace Raven
{

	Window::Window(const std::string& title)
		:title(title)
	{

	}

	void Window::Initialize()
	{
		glfwInit();

		// Render Surface Properties.
		RenderSurface surface = RenderModule::GetRequiredRenderSurface();

		//
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, surface.majorVersion);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, surface.minorVersion);
		glfwWindowHint(GLFW_OPENGL_PROFILE, surface.coreProfile ? GLFW_OPENGL_CORE_PROFILE : GLFW_OPENGL_COMPAT_PROFILE);
		glfwWindowHint(GLFW_RED_BITS, surface.redBits);
		glfwWindowHint(GLFW_GREEN_BITS, surface.greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, surface.blueBits);
		glfwWindowHint(GLFW_ALPHA_BITS, surface.alphaBits);
		glfwWindowHint(GLFW_SAMPLES, surface.samples);
		glfwWindowHint(GLFW_DOUBLEBUFFER, surface.doubleBuffer ? GLFW_TRUE : GLFW_FALSE);

		//	glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

		// Create Window...
		glfwWindow = glfwCreateWindow(1280, 820, title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(glfwWindow);
		InitEventsCallback();
	}


	void Window::Destroy()
	{
		glfwDestroyWindow(glfwWindow);
	}

	void Window::InitEventsCallback()
	{
		glfwSetWindowSizeCallback(glfwWindow, [](GLFWwindow* win, int32_t w, int32_t h) {
			Engine::Get().GetEventDispatcher().PostEvent(std::make_unique<WindowResizeEvent>(w, h));
			});

		glfwSetMouseButtonCallback(glfwWindow, [](GLFWwindow* window, int32_t btnId, int32_t state, int32_t mods) {
			double x;
			double y;
			glfwGetCursorPos(window, &x, &y);

			auto btn = -1;
			switch (btnId)
			{
			case GLFW_MOUSE_BUTTON_LEFT:btn = 0; break;
			case GLFW_MOUSE_BUTTON_MIDDLE:btn = 1; break;
			case GLFW_MOUSE_BUTTON_RIGHT:btn = 2; break;
			}

			if (state == GLFW_PRESS || state == GLFW_REPEAT)
			{
				Engine::Get().GetEventDispatcher().PostEvent(std::make_unique<MouseClickEvent>(btn, x, y));
			}
			if (state == GLFW_RELEASE)
			{
				Engine::Get().GetEventDispatcher().PostEvent(std::make_unique<MouseReleaseEvent>(btn, x, y));
			}
			});


		glfwSetCursorPosCallback(glfwWindow, [](GLFWwindow*, double x, double y) {
			Engine::Get().GetEventDispatcher().PostEvent(std::make_unique<MouseMoveEvent>(x, y));
		});

		glfwSetScrollCallback(glfwWindow, [](GLFWwindow* win, double xOffset, double yOffset) {
			double x;
			double y;
			glfwGetCursorPos(win, &x, &y);
			Engine::Get().GetEventDispatcher().PostEvent(std::make_unique<MouseScrolledEvent>(xOffset, yOffset,
				x, y));
			});

		glfwSetCharCallback(glfwWindow, [](GLFWwindow* window, unsigned int keycode) {
			Engine::Get().GetEventDispatcher().PostEvent(std::make_unique<CharInputEvent>(KeyCode::Id(keycode), (char)keycode));
			});

		glfwSetKeyCallback(glfwWindow, [](GLFWwindow*, int32_t key, int32_t scancode, int32_t action, int32_t mods) {
			switch (action)
			{
				case GLFW_PRESS:
				{
					Engine::Get().GetEventDispatcher().PostEvent(std::make_unique <KeyPressedEvent>(static_cast<KeyCode::Id>(key), 0));
					break;
				}
				case GLFW_RELEASE:
				{
					Engine::Get().GetEventDispatcher().PostEvent(std::make_unique <KeyReleasedEvent>(static_cast<KeyCode::Id>(key)));
					break;
				}
				case GLFW_REPEAT:
				{
					Engine::Get().GetEventDispatcher().PostEvent(std::make_unique <KeyPressedEvent>(static_cast<KeyCode::Id>(key), 1));
					break;
				}
			}
		});

	}

	glm::vec2 Window::GetWindowScale()
	{
		float x, y;
		glfwGetWindowContentScale(glfwWindow, &x, &y);
		return {x,y};
	}

	glm::vec2 Window::GetWindowSize()
	{
		int32_t x, y;
		glfwGetWindowSize(glfwWindow, &x, &y);
		return { x,y };
	}

	glm::ivec2 Window::GetFramebufferSize()
	{
		glm::ivec2 size;
		glfwGetFramebufferSize(glfwWindow, &size.x, &size.y);
		return size;
	}

	void Window::PollEvent()
	{
		glfwPollEvents();
	}

	bool Window::ShouldClose()
	{
		return glfwWindowShouldClose(glfwWindow);
	}

	void Window::SwapBuffers()
	{
		glfwSwapBuffers(glfwWindow);
	}

	void Window::Close()
	{
		glfwSetWindowShouldClose(glfwWindow, GLFW_TRUE);
	}

};