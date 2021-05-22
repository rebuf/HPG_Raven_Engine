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



	// Place holder types to easily change types later to match our Engine Types when they are defined...
	typedef unsigned int GLENUM;
	typedef unsigned int GLUINT;
	typedef unsigned int GLINT;




	// OpenGL Data Types.
	enum class EGLTypes : GLENUM
	{
		Byte  = 0x1400,
		Byte2 = 0x1407,
		Byte3 = 0x1408,
		Byte4 = 0x1409,

		UnsignedByte = 0x1401,
		UnsignedShort = 0x1403,
		UnsignedInt = 0x1405,

		Short = 0x1402,
		Int = 0x1404,

		Float = 0x1406,
		Double = 0x140A
	};




	// Types of OpenGL buffer objects.
	enum class EGLBufferType : GLENUM
	{
		None = 0x0500,
		Array = 0x8892,
		Element = 0x8893,
		Uniform = 0x8A11
	};






	// Usage of OpenGL buffer.
	enum class EGLBufferUsage : GLENUM
	{
		StreamDraw = 0x88E0,
		StreamRead = 0x88E1,
		StreamCopy = 0x88E2,

		StaticDraw = 0x88E4,
		StaticRead = 0x88E5,
		StaticCopy = 0x88E6,

		DynamicDraw = 0x88E8,
		DynamicRead = 0x88E9,
		DynamicCopy = 0x88EA
	};






	// Different Types of OpenGL Shader Stages.
	enum class EGLShaderStage : GLENUM
	{
		Vertex = 0x8B31,
		Geometry = 0x8DD9,
		Fragment = 0x8B30,
		TessEvaluation = 0x8E87,
		TessControl = 0x8E88
	};


	// Shader Types Bit flags.
	enum class EGLShaderStageBit : GLENUM
	{
		None = 0x0,
		VertexBit = 0x00000001,
		FragmentBit = 0x00000002,
		GeometryBit = 0x00000004,
		TessControlBit = 0x00000008,
		TessEvaluationBit = 0x00000010,

		All = VertexBit 
		| FragmentBit | GeometryBit 
		| TessControlBit | TessEvaluationBit
	};

	// Operators for EGLShaderStageBit
	inline EGLShaderStageBit operator&(EGLShaderStageBit a, EGLShaderStageBit b) { return static_cast<EGLShaderStageBit>((GLENUM)a & (GLENUM)b); }
	inline EGLShaderStageBit operator|(EGLShaderStageBit a, EGLShaderStageBit b) { return static_cast<EGLShaderStageBit>((GLENUM)a | (GLENUM)b); }


	// OpenGL Texture Types
	enum class EGLTexture : GLENUM
	{
		None = 0,
		Texture2D = 0x0DE1,
		CubeMap = 0x8513
	};


	// OpenGL Formats.
	enum class EGLFormat : GLENUM
	{
		None = 0x0,

		R = 0x1903,
		RGB = 0x1907,
		RGBA = 0x1908,

		R16F = 0x822D,
		RG16F = 0x822F,
		RGB16F = 0x881B,
		RGBA16F = 0x881A,

		Depth24 = 0x81A6,
		Depth32 = 0x81A7,
		DepthStencil = 0x88F0,

		R32F = 0x822E,
		RG32F = 0x8230,

	};



	// OpenGL Texture Filters.
	enum class EGLFilter : GLENUM
	{
		Nearest = 0x2600,
		Linear = 0x2601,
		TriLinear = 0x2703
	};


	// OpenGL Texture Wrap.
	enum class EGLWrap : GLENUM
	{
		Repeat = 0x2901,
		Mirror = 0x8370,
		ClampToEdge = 0x812F,
		ClampToBorder = 0x812D
	};


	// OpenGL Attachments Types.
	enum class EGLAttachment : GLENUM
	{
		None = 0,

		Color0 = 0x8CE0,
		Color1 = 0x8CE1,
		Color2 = 0x8CE2,
		Color3 = 0x8CE3,
		Color4 = 0x8CE4,
		Color5 = 0x8CE5,
		Color6 = 0x8CE6,
		Color7 = 0x8CE7,
		Color8 = 0x8CE8,
		Color9 = 0x8CE9,
		Depth  = 0x8D00,
		DepthStencil = 0x821A
	};


	// FrameBuffer targets.
	enum class EGLFrameBuffer : GLENUM
	{
		// Both Read and Draw.
		Framebuffer = 0x8D40,

		Read = 0x8CA8,
		Draw = 0x8CA9
	};


	// FrameBuffer targets.
	enum class EGLBufferMask : GLENUM
	{
		Color   = 0x00004000,
		Depth   = 0x00000100,
		Stencil = 0x00000400
	};

}
