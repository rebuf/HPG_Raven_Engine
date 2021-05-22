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
#include <cstdlib>
#include <algorithm>

#include "Utilities/Core.h"
#include "StringUtils.h"

#ifdef _WIN32
#include <direct.h>
#define GetCwd _getcwd
#else
#include <unistd.h>
#define GetCwd getcwd
#endif


namespace Raven 
{
	namespace StringUtils
	{
		std::string GetExtension(const std::string& fileName)
		{
			auto pos = fileName.find_last_of('.');
			if (pos != std::string::npos)
				return fileName.substr(pos + 1);
			return "";
		}

		std::string RemoveExtension(const std::string& fileName)
		{
			auto pos = fileName.find_last_of('.');
			if (pos != std::string::npos)
				return fileName.substr(0, pos);
			return fileName;
		}

		std::string GetFileName(const std::string& filePath)
		{
			auto pos1 = filePath.find_last_of('/');
			auto pos2 = filePath.find_last_of('\\');

			if (pos1 != std::string::npos && pos2 != std::string::npos)
			{
				return filePath.substr(std::max(pos1, pos2) + 1);
			}
			else if (pos1 != std::string::npos) 
			{
				return filePath.substr(pos1 + 1);
			}
			else if (pos2 != std::string::npos)
			{
				return filePath.substr(pos2 + 1);
			}

			return filePath;
		}

		std::string GetFileNameWithoutExtension(const std::string& filePath)
		{
			return RemoveExtension(GetFileName(filePath));
		}

		void Replace(std::string& src, const std::string& origin, const std::string& des)
		{
			std::string::size_type pos = 0;
			std::string::size_type srcLen = origin.size();
			std::string::size_type desLen = des.size();
			pos = src.find(origin, pos);
			while ((pos != std::string::npos))
			{
				src.replace(pos, srcLen, des);
				pos = src.find(origin, (pos + desLen));
			}
		}


		std::string GetCurrentWorkingDirectory()
		{
			char currentPath[FILENAME_MAX];
			if (!GetCwd(currentPath, sizeof(currentPath)))
			{
				return std::string(); // empty string
			}
			currentPath[sizeof(currentPath) - 1] = '\0'; // terminate the string
			return std::string(currentPath);
		}

		std::vector<std::string> Split(std::string input, const std::string& delimiter)
		{
			std::vector<std::string> ret;
			size_t pos = 0;
			std::string token;
			while ((pos = input.find(delimiter)) != std::string::npos)
			{
				token = input.substr(0, pos);
				ret.emplace_back(token);
				input.erase(0, pos + delimiter.length());
			}
			ret.emplace_back(input);
			return ret;
		}

		bool StartWith(const std::string& str, const std::string& with)
		{
			return str.find(with);
		}

		bool IsHiddenFile(const std::string& path)
		{
			if (path != ".." &&
				path != "." &&
				path[0] == '.')
			{
				return true;
			}
			return false;
		}


		bool IsTextFile(const std::string& filePath)
		{
			std::string extension = GetExtension(filePath);
			Trim(extension);
			if (extension == "txt" || extension == "glsl" || extension == "shader" || extension == "vert"
				|| extension == "frag" || extension == "lua" || extension == "Lua")
				return true;

			return false;
		}

		bool IsLuaFile(const std::string& filePath)
		{
			std::string extension = GetExtension(filePath);
			Trim(extension);
			return extension == "lua" || extension == "Lua";
		}

		bool IsAudioFile(const std::string& filePath)
		{
			std::string extension = GetExtension(filePath);
			Trim(extension);
			return extension == "ogg";
		}

		bool IsSceneFile(const std::string& filePath)
		{
			std::string extension = GetExtension(filePath);
			Trim(extension);
			return extension == "raven";
			
		}

		bool IsControllerFile(const std::string& filePath)
		{
			std::string extension = GetExtension(filePath);
			Trim(extension);
			return extension == "controller";
		}

		bool IsModelFile(const std::string& filePath)
		{
			std::string extension = GetExtension(filePath);
			Trim(extension);
			return extension == "obj" || extension == "gltf" || extension == "glb" || extension == "fbx" || extension == "FBX";
		}

		bool IsTextureFile(const std::string& filePath)
		{
			std::string extension = GetExtension(filePath);
			Trim(extension);
			return extension == "png" || extension == "tga" || extension == "jpg";
		}

		void Trim(std::string& str)
		{
			if (!str.empty())
			{
				str.erase(0, str.find_first_not_of(" "));
				str.erase(str.find_last_not_of(" ") + 1);
			}
		}

		char* IntToString(int num, char* buffer, EBase base)
		{
			int i = 0;
			// check for 0 case
			if (num == 0)
			{
				buffer[i++] = '0';
				buffer[i]   = '\0';
				return buffer;
			}
			// detect negative numbers for base 10
			bool negative = false;
			if (num < 0 && base == 10)
			{
				num = -num;
				negative = true;
			}
			// loop till all digits have been processed
			while (num != 0)
			{
				int remainder = num % base; // determine digit
				buffer[i++] = remainder > 9 ? remainder - 10 + 'a' : remainder + '0';
				num /= base; // integer division by base changes to next digit
			}
			if (negative)
			{
				buffer[i++] = '-';
			}
			buffer[i] = '\0';
			std::reverse(buffer, buffer + strlen(buffer));
			return buffer;
		}
	};
};
