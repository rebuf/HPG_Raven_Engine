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
#pragma once




// The Current Raven Files Version.
#define RAVEN_VERSION 10002



// The First version in the engine.
#define RAVEN_FIRST_VERSION 10000


// Global variables are set by archive or loader for saving and load.
struct RavenVersionGlobals
{
	// Version of the scene that is currently being loaded.
	static unsigned int SCENE_ARCHIVE_VERSION;

};




// Versions:
//    - every time you increase the version please add comments this will help us detect the time
//			a file has been corrupted.
//
//    - FORAMT: NEW VERSION - DATE - WHAT THE NEW VERSION ADD.
//



// 10000 - 28/04/2021 - Initial Version.
// 10001 - 06/05/2021 - Start saving referenced material in Primitve Components.
// 10002 - 16/05/2021 - Cast Shadow boolean in in Primitve Components and Scene Global Settings.

