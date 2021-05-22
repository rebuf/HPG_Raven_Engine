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

#include <imgui/imgui.h>
#include <string>
#include <glm/glm.hpp>

namespace ImGuiHelper 
{
	enum class PropertyFlag
	{
		None = 0,
		ColorProperty = 1
	};

	void Tooltip(const char * str);

	bool Property(const std::string& name, bool& value);
	bool Property(const std::string& name, std::string& value, bool disable = false);
	bool Property(const std::string& name, float& value, float min = -1.0f, float max = 1.0f, PropertyFlag flags = PropertyFlag::None);
	bool Property(const std::string& name, glm::vec2& value, PropertyFlag flags);
	bool Property(const std::string& name, glm::vec2& value, float min = -1.0f, float max = 1.0f, PropertyFlag flags = PropertyFlag::None);
	bool Property(const std::string& name, glm::vec3& value, PropertyFlag flags);
	bool Property(const std::string& name, glm::vec3& value, float min = -1.0f, float max = 1.0f, PropertyFlag flags = PropertyFlag::None);
	bool Property(const std::string& name, glm::vec4& value, bool exposeW, PropertyFlag flags);
	bool Property(const std::string& name, glm::vec4& value, float min = -1.0f, float max = 1.0f, bool exposeW = false, PropertyFlag flags = PropertyFlag::None);
};


static inline ImVec2 operator*(const ImVec2& lhs, const float rhs)   { return ImVec2(lhs.x*rhs, lhs.y*rhs); }
static inline ImVec2 operator/(const ImVec2& lhs, const float rhs)   { return ImVec2(lhs.x/rhs, lhs.y/rhs); }
static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x+rhs.x, lhs.y+rhs.y); }
static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x-rhs.x, lhs.y-rhs.y); }
static inline ImVec2 operator*(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x*rhs.x, lhs.y*rhs.y); }
static inline ImVec2 operator/(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x/rhs.x, lhs.y/rhs.y); }
static inline ImVec2& operator+=(ImVec2& lhs, const ImVec2& rhs)     { lhs.x += rhs.x; lhs.y += rhs.y; return lhs; }
static inline ImVec2& operator-=(ImVec2& lhs, const ImVec2& rhs)     { lhs.x -= rhs.x; lhs.y -= rhs.y; return lhs; }
static inline ImVec2& operator*=(ImVec2& lhs, const float rhs)       { lhs.x *= rhs; lhs.y *= rhs; return lhs; }
static inline ImVec2& operator/=(ImVec2& lhs, const float rhs)       { lhs.x /= rhs; lhs.y /= rhs; return lhs; }
static inline ImVec4 operator-(const ImVec4& lhs, const ImVec4& rhs) { return ImVec4(lhs.x-rhs.x, lhs.y-rhs.y, lhs.z-rhs.z, lhs.w-rhs.w); }
static inline ImVec4 operator+(const ImVec4& lhs, const ImVec4& rhs) { return ImVec4(lhs.x+rhs.x, lhs.y+rhs.y, lhs.z+rhs.z, lhs.w+rhs.w); }
static inline ImVec4 operator*(const ImVec4& lhs, const float rhs)   { return ImVec4(lhs.x*rhs, lhs.y*rhs,lhs.z*rhs,lhs.w*rhs); }
static inline ImVec4 operator/(const ImVec4& lhs, const float rhs)   { return ImVec4(lhs.x/rhs, lhs.y/rhs,lhs.z/rhs,lhs.w/rhs); }
static inline ImVec4 operator*(const ImVec4& lhs, const ImVec4& rhs) { return ImVec4(lhs.x*rhs.x, lhs.y*rhs.y,lhs.z*rhs.z,lhs.w*rhs.w); }
static inline ImVec4 operator/(const ImVec4& lhs, const ImVec4& rhs) { return ImVec4(lhs.x/rhs.x,lhs.y/rhs.y,lhs.z/rhs.z,lhs.w/rhs.w); }
static inline ImVec4& operator+=(ImVec4& lhs, const ImVec4& rhs)     { lhs.x += rhs.x; lhs.y += rhs.y;lhs.z += rhs.z;lhs.w += rhs.w;return lhs; }
static inline ImVec4& operator-=(ImVec4& lhs, const ImVec4& rhs)     { lhs.x -= rhs.x; lhs.y -= rhs.y;lhs.z -= rhs.z;lhs.w -= rhs.w; return lhs; }
static inline ImVec4& operator*=(ImVec4& lhs, const float rhs)       { lhs.x *= rhs; lhs.y *= rhs; return lhs; }
static inline ImVec4& operator/=(ImVec4& lhs, const float rhs)       { lhs.x /= rhs; lhs.y /= rhs; return lhs; }
