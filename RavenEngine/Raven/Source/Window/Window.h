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

#pragma  once
#include <string>
#include <memory>
#include "IModule.h"

#include "glm/vec2.hpp"



struct GLFWwindow;

namespace Raven
{
	class Window : public IModule
	{
	public:
		Window(const std::string& title);
		static EModuleType GetModuleType() { return MT_Window; }
		virtual void Initialize() override;
		virtual void Destroy() override;

		void InitEventsCallback();

		inline auto GetNativeWindow() { return glfwWindow; }

		glm::vec2  GetWindowScale();
		glm::vec2  GetWindowSize();
		glm::ivec2 GetFramebufferSize();

		void PollEvent();
		bool ShouldClose();
		void SwapBuffers();
		void Close();

	private:
		GLFWwindow* glfwWindow = nullptr;
		std::string title;
	};
};