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
#include "GLContext.h"



#include "GL/glew.h"
#include "GLFW/glfw3.h"



namespace Raven {







GLContext::GLContext()
	: glfw_context(nullptr)
{

}


void GLContext::SetGLFWContext(GLFWwindow* context)
{
	glfw_context = context;
}


RenderSurface GLContext::GetSurface()
{
	RenderSurface surface;

	// OpenGL 4.5 core profile.
	surface.majorVersion = 4;
	surface.minorVersion = 5;
	surface.coreProfile = true;

	// RGBA - 32Bit
	surface.redBits = 8;
	surface.greenBits = 8;
	surface.blueBits = 8;
	surface.alphaBits = 8;

	// Double Buffer.
	surface.doubleBuffer = true;

	// No MSAA, we are using framebuffers.
	surface.samples = 0;

	return surface;
}


void GLContext::MakeCurrent()
{
	glfwMakeContextCurrent(glfw_context);
}



} // End of namespace Raven.
