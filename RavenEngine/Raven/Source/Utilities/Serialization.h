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


#include "Core.h"


#include <glm/glm.hpp>
#include "imgui/imgui.h"


namespace glm
{
	template<class Archive> void serialize(Archive& archive, glm::vec2& v) { archive(v.x, v.y); }
	template<class Archive> void serialize(Archive& archive, glm::vec3& v) { archive(v.x, v.y, v.z); }
	template<class Archive> void serialize(Archive& archive, glm::vec4& v) { archive(v.x, v.y, v.z, v.w); }
	template<class Archive> void serialize(Archive& archive, glm::ivec2& v) { archive(v.x, v.y); }
	template<class Archive> void serialize(Archive& archive, glm::ivec3& v) { archive(v.x, v.y, v.z); }
	template<class Archive> void serialize(Archive& archive, glm::ivec4& v) { archive(v.x, v.y, v.z, v.w); }
	template<class Archive> void serialize(Archive& archive, glm::uvec2& v) { archive(v.x, v.y); }
	template<class Archive> void serialize(Archive& archive, glm::uvec3& v) { archive(v.x, v.y, v.z); }
	template<class Archive> void serialize(Archive& archive, glm::uvec4& v) { archive(v.x, v.y, v.z, v.w); }
	template<class Archive> void serialize(Archive& archive, glm::dvec2& v) { archive(v.x, v.y); }
	template<class Archive> void serialize(Archive& archive, glm::dvec3& v) { archive(v.x, v.y, v.z); }
	template<class Archive> void serialize(Archive& archive, glm::dvec4& v) { archive(v.x, v.y, v.z, v.w); }

	// glm matrices serialization
	template<class Archive> void serialize(Archive& archive, glm::mat2& m) { archive(m[0], m[1]); }
	template<class Archive> void serialize(Archive& archive, glm::dmat2& m) { archive(m[0], m[1]); }
	template<class Archive> void serialize(Archive& archive, glm::mat3& m) { archive(m[0], m[1], m[2]); }
	template<class Archive> void serialize(Archive& archive, glm::mat4& m) { archive(m[0], m[1], m[2], m[3]); }
	template<class Archive> void serialize(Archive& archive, glm::dmat4& m) { archive(m[0], m[1], m[2], m[3]); }

	template<class Archive> void serialize(Archive& archive, glm::quat& q) { archive(q.x, q.y, q.z, q.w); }
	template<class Archive> void serialize(Archive& archive, glm::dquat& q) { archive(q.x, q.y, q.z, q.w); }

}

// ImGui vector serialization
template<class Archive> void serialize(Archive& archive, ImVec2& v) { archive(v.x, v.y); }
template<class Archive> void serialize(Archive& archive, ImVec4& v) { archive(v.x, v.y, v.z, v.w); }

#include <rp3d/include/reactphysics3d/reactphysics3d.h>
#include <rp3d/include/reactphysics3d/decimal.h>
#include <rp3d/include/reactphysics3d/mathematics/Vector3.h>
#include <rp3d/include/reactphysics3d/mathematics/Quaternion.h>
#include <rp3d/include/reactphysics3d/mathematics/Transform.h>

namespace reactphysics3d
{
	// rp3d vector3, quaternion and transform serialization
	template<class Archive> void serialize(Archive& archive, Vector3& vector3) { archive(vector3.x, vector3.y, vector3.z); }
	template<class Archive> void serialize(Archive& archive, Quaternion& quaternion) { archive(quaternion.x, quaternion.y, quaternion.z, quaternion.w); }
	template<class Archive> void serialize(Archive& archive, Transform& transform) { archive(transform.mPosition, transform.mOrientation); }
}




namespace Raven
{
	// Compress data.
	extern bool Compress(uint32_t size, const uint8_t* data, uint8_t*& comp, uint32_t& outCompSize);

	// Uncompress data.
	extern bool Uncompress(uint32_t compSize, const uint8_t* comp, uint8_t* data, uint32_t size);

	// Compress data then save it.
	template<class Archive>
	void SaveCompressed(Archive& archive, uint32_t size, const uint8_t* data)
	{
		RAVEN_ASSERT(size != 0 && data != nullptr, "Invalid Input.");

		uint32_t compressedSize = 0;
		uint8_t* compressedData = nullptr;
		bool status = Compress(size, data, compressedData, compressedSize);
		RAVEN_ASSERT(status, "Failed to compress");

		archive(compressedSize);
		archive.saveBinary(compressedData, compressedSize);

		free(compressedData);
	}

	// Loaed Compressed data then uncompress it.
	template<class Archive>
	void LoadCompressed(Archive& archive, uint32_t size, uint8_t* data)
	{
		RAVEN_ASSERT(size != 0 && data != nullptr, "Invalid Input.");

		// Load Size.
		uint32_t compressedSize = 0;
		archive(compressedSize);

		// Allocate and load.
		uint8_t* compressedData = (uint8_t*)malloc((size_t)compressedSize);
		archive.loadBinary(compressedData, compressedSize);

		bool status = Uncompress(compressedSize, compressedData, data, size);
		RAVEN_ASSERT(status, "Failed to compress");

		free(compressedData);
	}

	// Archve save a vector.
	template<class Archive, class T>
	void SaveVector(Archive& archive, const std::vector<T>& v)
	{
		uint32_t size = v.size();
		archive(size);

		// Archive one element at a time.
		for (uint32_t i = 0; i < size; ++i)
		{
			archive(v[i]);
		}
	}

	// Archve load a vector.
	template<class Archive, class T>
	void LoadVector(Archive& archive, std::vector<T>& v)
	{
		uint32_t size = 0;
		archive(size);
		v.resize(size);

		// Archive one element at a time.
		for (uint32_t i = 0; i < size; ++i)
		{
			archive(v[i]);
		}
	}

	// Archve save a vector as binary data.
	template<class Archive, class T>
	void SaveVectorBinary(Archive& archive, const std::vector<T>& v)
	{
		uint32_t size = v.size();
		archive(size);

		if (size != 0)
		{
			archive.saveBinary(v.data(), size * sizeof(T));
		}
	}

	// Archve load a vector as binary data.
	template<class Archive, class T>
	void LoadVectorBinary(Archive& archive, std::vector<T>& v)
	{
		uint32_t size = 0;
		archive(size);
		v.resize(size);

		if (size != 0)
		{
			archive.loadBinary(v.data(), size * sizeof(T));
		}
	}
}



