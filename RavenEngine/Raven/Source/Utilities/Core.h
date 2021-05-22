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



#include "Logger/Console.h"



#include <memory>



// Pointer Types
template<class T>
using Ptr = std::shared_ptr<T>;

template<class T>
using WeakPtr = std::weak_ptr<T>;



#if RAVEN_DEBUG

#define RAVEN_ASSERT(condition, ...)   \
	{                                    \
		if(!(condition))                   \
		{                                  \
			LOGE("Assertion Failed : {0}", __VA_ARGS__);   \
			__debugbreak();       \
			exit(-2234);          \
		}                       \
	}


#define RAVEN_ENSURE(condition, ...) RAVEN_ASSERT(condition, __VA_ARGS__)


#else

#define RAVEN_ASSERT(condition, ...) 
#define RAVEN_ENSURE(condition, ...) (condition)

#endif




#define NOCOPYABLE(Type)								\
    Type(const Type&) = delete;							\
    Type& operator=(const Type&) = delete;




// Values Defs...
#define SMALL_NUM 1.e-6






// Use to archive enum using cearal archive functions.
template<class TEnum, std::enable_if_t<std::is_enum<TEnum>::value, bool> = true >
struct EnumAsInt
{
	// Construct.
	EnumAsInt(TEnum& en)
	{
		value = &en;
	}

	// Serialization Save.
	template<typename Archive>
	void save(Archive& archive) const
	{
		int32_t tmp = static_cast<int32_t>(*value);
		archive(tmp);
	}

	// Serialization Load.
	template<typename Archive>
	void load(Archive& archive)
	{
		int32_t tmp;
		archive(tmp);

		*value = static_cast<TEnum>(tmp);
	}
	
	TEnum* value;
};



// Common Include...
#include "Serialization.h"
