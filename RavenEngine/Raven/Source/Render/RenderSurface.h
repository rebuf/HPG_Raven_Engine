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






namespace Raven
{
	// Render Surface Info.
	struct RenderSurface
	{
		// OpenGL Major Version.
		int majorVersion;

		// OpenGL Minor Version;
		int minorVersion;

		// Requrie ore Profile.
		bool coreProfile;

		// Number of red bits.
		int redBits;

		// Number of green bits.
		int greenBits;

		// Number of blue bits.
		int blueBits;

		// Number of alpha bits.
		int alphaBits;

		// Require Double Buffer.
		bool doubleBuffer;

		// Number of samples for Multisampling.
		int samples;
	};
}


