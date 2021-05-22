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

#include "FileSystem.h"
#include <sys/stat.h>
#include <fstream>
namespace Raven 
{

	std::unique_ptr<uint8_t[]> FileSystem::ReadFile(const std::string& path, int64_t& size)
	{
		auto filePtr = fopen(path.c_str(), "rb");
		
		if (filePtr != nullptr)
		{
			fseek(filePtr, 0, SEEK_END);
			size = ftell(filePtr);
			fseek(filePtr, 0, SEEK_SET);
			std::unique_ptr<uint8_t[]> array(new uint8_t[size]);
			memset(array.get(), 0, size);
			fread(array.get(), sizeof(int8_t) * size, 1, filePtr);
			fclose(filePtr);
			return array;
		}
		size = -1;
		return nullptr;
	}


};
