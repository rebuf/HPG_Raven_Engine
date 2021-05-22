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
#include <string>
#include <vector>

namespace Raven
{
	namespace StringUtils 
	{

		constexpr uint32_t MAX_PATH_LENGTH = 2048;

		// later we should use enums to identify formats for faster comparison
		enum EFileExtensions : uint32_t
		{
			Txt,
			Obj,
			Fbx
		};

		enum EBase : uint32_t
		{
			Decimal		= 10,
			Hexadecimal = 16,
			Binary		= 2
		};

		std::string GetExtension(const std::string& fileName);
		std::string RemoveExtension(const std::string& fileName);
		std::string GetFileName(const std::string& filePath);
		std::string GetFileNameWithoutExtension(const std::string& filePath);
		std::string GetCurrentWorkingDirectory();
		std::vector<std::string> Split(std::string input, const std::string& delimiter);
		bool StartWith(const std::string& str, const std::string& with);
		void Replace(std::string& src, const std::string& origin, const std::string& des);
		bool IsHiddenFile(const std::string& path);
		bool IsTextFile(const std::string& filePath);
		bool IsLuaFile(const std::string& filePath);
		bool IsAudioFile(const std::string& filePath);
		bool IsSceneFile(const std::string& filePath);
		bool IsControllerFile(const std::string& filePath);
		bool IsModelFile(const std::string& filePath);
		bool IsTextureFile(const std::string& filePath);
		void Trim(std::string& str);

		char* IntToString(int num, char* buffer, EBase base); // itoa
	};

};
