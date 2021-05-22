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

	// IRenderResource:
	//      - Every resource that has its own render data, its render data is managed by this resource. 
	//
	//      - Example of a render resource is RenderTextureResource which holds the allocated texture on 
	//        the GPU. The render then can use that render resource to reference it when drawing.
	//
	class IRenderResource
	{
	public:
		// Virtual Destructor.
		virtual ~IRenderResource() { }

	};


}


