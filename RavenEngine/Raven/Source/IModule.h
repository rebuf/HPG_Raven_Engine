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

#include <vector>

/** Modules Types. */
enum EModuleType
{
	MT_Render,
	MT_ImGui,
	MT_Window,
	MT_GUI,
	MT_SceneManager,
	MT_ResourceManager,
	MT_ProceduralGenerator,
	MT_LuaVirtualMachine,
	MT_Physics,
	MT_MAX
};

/**
 * Interface for modules in the engine.
 */
class IModule
{
public:
	/** Virtual Destruct. */
	virtual ~IModule() { }

	/** Called by the engine to initialize the module. */
	virtual void Initialize() = 0;

	/** Called by the engine to destory the module. */
	virtual void Destroy() = 0;
};

