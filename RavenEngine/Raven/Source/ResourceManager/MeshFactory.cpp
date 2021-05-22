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
#include "MeshFactory.h"

#include "Engine.h"
#include "ResourceManager/ResourceManager.h"
#include "ResourceManager/Resources/Mesh.h"



#define _USE_MATH_DEFINES
#include <math.h>
#include <glm/gtc/quaternion.hpp>





#define BASIC_SHAPES_DIR "./assets/Basic Shapes/"






namespace Raven {




struct MeshFactoryVertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoord;
};


Mesh* MeshFactory::CreateQuad(float x, float y, float width, float height)
{
	Mesh* mesh = new Mesh();
	mesh->SetName("Quad");

	MeshSection* meshSection = new MeshSection();
	meshSection->positions.resize(4);
	meshSection->texCoords.resize(4);

	meshSection->positions[0] = glm::vec3(x, y, 0.0f);
	meshSection->texCoords[0] = glm::vec2(0.0f, 1.0f);

	meshSection->positions[1] = glm::vec3(x + width, y, 0.0f);
	meshSection->texCoords[1] = glm::vec2(0, 0);

	meshSection->positions[2] = glm::vec3(x + width, y + height, 0.0f);
	meshSection->texCoords[2] = glm::vec2(1, 0);

	meshSection->positions[3] = glm::vec3(x, y + height, 0.0f);
	meshSection->texCoords[3] = glm::vec2(1, 1);

	meshSection->indices = { 0, 1, 2, 2, 3, 0, };

	mesh->AddMeshSection(Ptr<MeshSection>(meshSection));

	return mesh;
}


Mesh* MeshFactory::CreateQuad(const glm::vec2& position, const glm::vec2& size)
{
	return CreateQuad(position.x, position.y, size.x, size.y);
}


Mesh* MeshFactory::CreateQuad()
{
	Mesh* mesh = new Mesh();
	mesh->SetName("Quad");

	MeshSection* meshSection = new MeshSection();
	meshSection->positions.resize(4);
	meshSection->texCoords.resize(4);
	meshSection->normals.resize(4);

	meshSection->positions[0] = glm::vec3(-1.0f, -1.0f, 0.0f);
	meshSection->texCoords[0] = glm::vec2(0.0f, 0.0f);
	meshSection->normals[0] = glm::vec3(0.0f, 0.0f, 1.0f);

	meshSection->positions[1] = glm::vec3(1.0f, -1.0f, 0.0f);
	meshSection->texCoords[1] = glm::vec2(1.0f, 0.0f);
	meshSection->normals[1] = glm::vec3(0.0f, 0.0f, 1.0f);

	meshSection->positions[2] = glm::vec3(1.0f, 1.0f, 0.0f);
	meshSection->texCoords[2] = glm::vec2(1.0f, 1.0f);
	meshSection->normals[2] = glm::vec3(0.0f, 0.0f, 1.0f);

	meshSection->positions[3] = glm::vec3(-1.0f, 1.0f, 0.0f);
	meshSection->texCoords[3] = glm::vec2(0.0f, 1.0f);
	meshSection->normals[3] = glm::vec3(0.0f, 0.0f, 1.0f);

	meshSection->indices = { 0, 1, 2, 2, 3, 0, };
	meshSection->bounds = MathUtils::BoundingBox(glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f));

	mesh->AddMeshSection(Ptr<MeshSection>(meshSection));

	return mesh;
}


Mesh* MeshFactory::CreateCube()
{
	//    v6----- v5
	//   /|      /|
	//  v1------v0|
	//  | |     | |
	//  | |v7---|-|v4
	//  |/      |/
	//  v2------v3
	Mesh* mesh = new Mesh();
	mesh->SetName("Cube");

	MeshSection* meshSection = new MeshSection();
	meshSection->texCoords.resize(24);
	meshSection->normals.resize(24);
	meshSection->positions.resize(24);

	meshSection->positions[0] = glm::vec3(1.0f, 1.0f, 1.0f);
	meshSection->normals[0] = glm::vec3(0.0f, 0.0f, 1.0f);

	meshSection->positions[1] = glm::vec3(-1.0f, 1.0f, 1.0f);
	meshSection->normals[1] = glm::vec3(0.0f, 0.0f, 1.0f);

	meshSection->positions[2] = glm::vec3(-1.0f, -1.0f, 1.0f);
	meshSection->normals[2] = glm::vec3(0.0f, 0.0f, 1.0f);

	meshSection->positions[3] = glm::vec3(1.0f, -1.0f, 1.0f);
	meshSection->normals[3] = glm::vec3(0.0f, 0.0f, 1.0f);

	meshSection->positions[4] = glm::vec3(1.0f, 1.0f, 1.0f);
	meshSection->normals[4] = glm::vec3(1.0f, 0.0f, 0.0f);

	meshSection->positions[5] = glm::vec3(1.0f, -1.0f, 1.0f);
	meshSection->normals[5] = glm::vec3(1.0f, 0.0f, 0.0f);

	meshSection->positions[6] = glm::vec3(1.0f, -1.0f, -1.0f);
	meshSection->normals[6] = glm::vec3(1.0f, 0.0f, 0.0f);

	meshSection->positions[7] = glm::vec3(1.0f, 1.0f, -1.0f);
	meshSection->texCoords[7] = glm::vec2(0.0f, 1.0f);
	meshSection->normals[7] = glm::vec3(1.0f, 0.0f, 0.0f);

	meshSection->positions[8] = glm::vec3(1.0f, 1.0f, 1.0f);
	meshSection->normals[8] = glm::vec3(0.0f, 1.0f, 0.0f);

	meshSection->positions[9] = glm::vec3(1.0f, 1.0f, -1.0f);
	meshSection->normals[9] = glm::vec3(0.0f, 1.0f, 0.0f);

	meshSection->positions[10] = glm::vec3(-1.0f, 1.0f, -1.0f);
	meshSection->texCoords[10] = glm::vec2(0.0f, 1.0f);
	meshSection->normals[10] = glm::vec3(0.0f, 1.0f, 0.0f);

	meshSection->positions[11] = glm::vec3(-1.0f, 1.0f, 1.0f);
	meshSection->normals[11] = glm::vec3(0.0f, 1.0f, 0.0f);

	meshSection->positions[12] = glm::vec3(-1.0f, 1.0f, 1.0f);
	meshSection->normals[12] = glm::vec3(-1.0f, 0.0f, 0.0f);

	meshSection->positions[13] = glm::vec3(-1.0f, 1.0f, -1.0f);
	meshSection->normals[13] = glm::vec3(-1.0f, 0.0f, 0.0f);

	meshSection->positions[14] = glm::vec3(-1.0f, -1.0f, -1.0f);
	meshSection->normals[14] = glm::vec3(-1.0f, 0.0f, 0.0f);

	meshSection->positions[15] = glm::vec3(-1.0f, -1.0f, 1.0f);
	meshSection->normals[15] = glm::vec3(-1.0f, 0.0f, 0.0f);

	meshSection->positions[16] = glm::vec3(-1.0f, -1.0f, -1.0f);
	meshSection->normals[16] = glm::vec3(0.0f, -1.0f, 0.0f);

	meshSection->positions[17] = glm::vec3(1.0f, -1.0f, -1.0f);
	meshSection->normals[17] = glm::vec3(0.0f, -1.0f, 0.0f);

	meshSection->positions[18] = glm::vec3(1.0f, -1.0f, 1.0f);
	meshSection->normals[18] = glm::vec3(0.0f, -1.0f, 0.0f);

	meshSection->positions[19] = glm::vec3(-1.0f, -1.0f, 1.0f);
	meshSection->normals[19] = glm::vec3(0.0f, -1.0f, 0.0f);

	meshSection->positions[20] = glm::vec3(1.0f, -1.0f, -1.0f);
	meshSection->normals[20] = glm::vec3(0.0f, 0.0f, -1.0f);

	meshSection->positions[21] = glm::vec3(-1.0f, -1.0f, -1.0f);
	meshSection->normals[21] = glm::vec3(0.0f, 0.0f, -1.0f);

	meshSection->positions[22] = glm::vec3(-1.0f, 1.0f, -1.0f);
	meshSection->normals[22] = glm::vec3(0.0f, 0.0f, -1.0f);

	meshSection->positions[23] = glm::vec3(1.0f, 1.0f, -1.0f);
	meshSection->normals[23] = glm::vec3(0.0f, 0.0f, -1.0f);

	for (int i = 0; i < 6; i++)
	{
		meshSection->texCoords[i * 4 + 0] = glm::vec2(0.0f, 0.0f);
		meshSection->texCoords[i * 4 + 1] = glm::vec2(1.0f, 0.0f);
		meshSection->texCoords[i * 4 + 2] = glm::vec2(1.0f, 1.0f);
		meshSection->texCoords[i * 4 + 3] = glm::vec2(0.0f, 1.0f);
	}

	meshSection->indices = {
		0,1,2,
		0,2,3,
		4,5,6,
		4,6,7,
		8,9,10,
		8,10,11,
		12,13,14,
		12,14,15,
		16,17,18,
		16,18,19,
		20,21,22,
		20,22,23
	};

	meshSection->bounds = MathUtils::BoundingBox(glm::vec3(-1.0f,-1.0f,-1.0f), glm::vec3(1.0f, 1.0f, 1.0f));

	mesh->AddMeshSection(Ptr<MeshSection>(meshSection));

	return mesh;
}


Mesh* MeshFactory::CreatePyramid()
{
	MeshFactoryVertex data[18];


	data[0].position = glm::vec3(1.0f, 1.0f, -1.0f);
	data[0].texCoord = glm::vec2(0.24f, 0.20f);
	data[0].normal = glm::vec3(0.0f, 0.8948f, 0.4464f);

	data[1].position = glm::vec3(-1.0f, 1.0f, -1.0f);
	data[1].texCoord = glm::vec2(0.24f, 0.81f);
	data[1].normal = glm::vec3(0.0f, 0.8948f, 0.4464f);

	data[2].position = glm::vec3(0.0f, 0.0f, 1.0f);
	data[2].texCoord = glm::vec2(0.95f, 0.50f);
	data[2].normal = glm::vec3(0.0f, 0.8948f, 0.4464f);

	data[3].position = glm::vec3(-1.0f, 1.0f, -1.0f);
	data[3].texCoord = glm::vec2(0.24f, 0.21f);
	data[3].normal = glm::vec3(-0.8948f, 0.0f, 0.4464f);

	data[4].position = glm::vec3(-1.0f, -1.0f, -1.0f);
	data[4].texCoord = glm::vec2(0.24f, 0.81f);
	data[4].normal = glm::vec3(-0.8948f, 0.0f, 0.4464f);

	data[5].position = glm::vec3(0.0f, 0.0f, 1.0f);
	data[5].texCoord = glm::vec2(0.95f, 0.50f);
	data[5].normal = glm::vec3(-0.8948f, 0.0f, 0.4464f);

	data[6].position = glm::vec3(1.0f, 1.0f, -1.0f);
	data[6].texCoord = glm::vec2(0.24f, 0.81f);
	data[6].normal = glm::vec3(0.8948f, 0.0f, 0.4475f);

	data[7].position = glm::vec3(0.0f, 0.0f, 1.0f);

	data[7].texCoord = glm::vec2(0.95f, 0.50f);
	data[7].normal = glm::vec3(0.8948f, 0.0f, 0.4475f);

	data[8].position = glm::vec3(1.0f, -1.0f, -1.0f);
	data[8].texCoord = glm::vec2(0.24f, 0.21f);
	data[8].normal = glm::vec3(0.8948f, 0.0f, 0.4475f);

	data[9].position = glm::vec3(-1.0f, -1.0f, -1.0f);
	data[9].texCoord = glm::vec2(0.24f, 0.21f);
	data[9].normal = glm::vec3(0.0f, -0.8948f, 0.448f);

	data[10].position = glm::vec3(1.0f, -1.0f, -1.0f);
	data[10].texCoord = glm::vec2(0.24f, 0.81f);
	data[10].normal = glm::vec3(0.0f, -0.8948f, 0.448f);

	data[11].position = glm::vec3(0.0f, 0.0f, 1.0f);
	data[11].texCoord = glm::vec2(0.95f, 0.50f);
	data[11].normal = glm::vec3(0.0f, -0.8948f, 0.448f);

	data[12].position = glm::vec3(-1.0f, 1.0f, -1.0f);
	data[12].texCoord = glm::vec2(0.0f, 0.0f);
	data[12].normal = glm::vec3(0.0f, 0.0f, -1.0f);

	data[13].position = glm::vec3(1.0f, 1.0f, -1.0f);
	data[13].texCoord = glm::vec2(0.0f, 1.0f);
	data[13].normal = glm::vec3(0.0f, 0.0f, -1.0f);

	data[14].position = glm::vec3(1.0f, -1.0f, -1.0f);
	data[14].texCoord = glm::vec2(1.0f, 1.0f);
	data[14].normal = glm::vec3(0.0f, 0.0f, -1.0f);

	data[15].position = glm::vec3(-1.0f, -1.0f, -1.0f);
	data[15].texCoord = glm::vec2(0.96f, 0.0f);
	data[15].normal = glm::vec3(0.0f, 0.0f, -1.0f);

	data[16].position = glm::vec3(0.0f, 0.0f, 0.0f);
	data[16].texCoord = glm::vec2(0.0f, 0.0f);
	data[16].normal = glm::vec3(0.0f, 0.0f, 0.0f);

	data[17].position = glm::vec3(0.0f, 0.0f, 0.0f);
	data[17].texCoord = glm::vec2(0.0f, 0.0f);
	data[17].normal = glm::vec3(0.0f, 0.0f, 0.0f);



	Mesh* mesh = new Mesh();
	mesh->SetName("Pyramid");

	MeshSection* meshSection = new MeshSection();

	meshSection->indices = {
		0,1,2,
		3,4,5,
		6,7,8,
		9,10,11,
		12,13,14,
		15,12,14
	};


	for (auto i = 0; i < 18; i++)
	{
		meshSection->positions.emplace_back(data[i].position);
		meshSection->texCoords.emplace_back(data[i].texCoord);
		meshSection->normals.emplace_back(data[i].normal);
		meshSection->bounds.Add(data[i].position);
	}

	mesh->AddMeshSection(Ptr<MeshSection>(meshSection));

	return mesh;

}


Mesh* MeshFactory::CreateSphere(uint32_t xSegments, uint32_t ySegments)
{
	auto data = std::vector<MeshFactoryVertex>();

	float sectorCount = static_cast<float>(xSegments);
	float stackCount = static_cast<float>(ySegments);
	float sectorStep = 2 * M_PI / sectorCount;
	float stackStep = M_PI / stackCount;
	float radius = 1.0f;

	Mesh* mesh = new Mesh();
	mesh->SetName("Sphere");

	MeshSection* meshSection = new MeshSection();

	for (int i = 0; i <= stackCount; ++i)
	{
		float stackAngle = M_PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
		float xy = radius * cos(stackAngle);             // r * cos(u)
		float z = radius * sin(stackAngle);              // r * sin(u)


		for (int32_t j = 0; j <= sectorCount; ++j)
		{
			float sectorAngle = j * sectorStep;           // starting from 0 to 2pi

			// vertex position (x, y, z)
			float x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
			float y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)

			// vertex tex coords (s, t) range between [0, 1]
			float s = static_cast<float>(j / sectorCount);
			float t = static_cast<float>(i / stackCount);

			meshSection->positions.emplace_back(glm::vec3(x, y, z));
			meshSection->texCoords.emplace_back(glm::vec2(s, t));
			meshSection->normals.emplace_back(glm::normalize(glm::vec3(x, y, z)));
			meshSection->bounds.Add(glm::vec3(x, y, z));
		}
	}


	uint32_t k1, k2;
	for (uint32_t i = 0; i < stackCount; ++i)
	{
		k1 = i * (static_cast<uint32_t>(sectorCount) + 1U);     // beginning of current stack
		k2 = k1 + static_cast<uint32_t>(sectorCount) + 1U;      // beginning of next stack

		for (uint32_t j = 0; j < sectorCount; ++j, ++k1, ++k2)
		{
			// 2 triangles per sector excluding first and last stacks
			// k1 => k2 => k1+1
			if (i != 0)
			{
				meshSection->indices.emplace_back(k1);
				meshSection->indices.emplace_back(k2);
				meshSection->indices.emplace_back(k1 + 1);
			}

			// k1+1 => k2 => k2+1
			if (i != (stackCount - 1))
			{
				meshSection->indices.emplace_back(k1 + 1);
				meshSection->indices.emplace_back(k2);
				meshSection->indices.emplace_back(k2 + 1);
			}
		}
	}

	mesh->AddMeshSection(Ptr<MeshSection>(meshSection));

	return mesh;
}


Mesh* MeshFactory::CreatePlane(float width, float height, const glm::vec3& normal)
{
	Mesh* mesh = new Mesh();
	mesh->SetName("Plane");
	MeshSection* meshSection = new MeshSection();

	glm::vec3 vec = normal * 90.0f;
	glm::quat rotation =
		glm::quat(vec.z, glm::vec3(1.0f, 0.0f, 0.0f)) *
		glm::quat(vec.y, glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::quat(vec.x, glm::vec3(0.0f, 0.0f, 1.0f));

	MeshFactoryVertex data[4];
	memset(data, 0, 4 * sizeof(MeshFactoryVertex));


	data[0].position = rotation * glm::vec3(-width / 2.0f, -1.0f, -height / 2.0f);
	data[0].normal = normal;
	data[0].texCoord = glm::vec2(0.0f, 0.0f);

	data[1].position = rotation * glm::vec3(-width / 2.0f, -1.0f, height / 2.0f);
	data[1].normal = normal;
	data[1].texCoord = glm::vec2(0.0f, 1.0f);

	data[2].position = rotation * glm::vec3(width / 2.0f, 1.0f, height / 2.0f);
	data[2].normal = normal;
	data[2].texCoord = glm::vec2(1.0f, 1.0f);

	data[3].position = rotation * glm::vec3(width / 2.0f, 1.0f, -height / 2.0f);
	data[3].normal = normal;
	data[3].texCoord = glm::vec2(1.0f, 0.0f);


	for (auto& v : data)
	{
		meshSection->positions.emplace_back(v.position);
		meshSection->texCoords.emplace_back(v.texCoord);
		meshSection->normals.emplace_back(v.normal);
		meshSection->bounds.Add(v.position);
	}

	meshSection->indices =
	{
		0, 1, 2,
		2, 3, 0
	};

	mesh->AddMeshSection(Ptr<MeshSection>(meshSection));

	return mesh;
}


Mesh* MeshFactory::CreateCapsule(float radius, float midHeight, int32_t radialSegments, int32_t rings)
{
	int32_t i, j, prevrow, thisrow, point;
	float x, y, z, u, v, w;
	float onethird = 1.0f / 3.0f;
	float twothirds = 2.0f / 3.0f;


	Mesh* mesh = new Mesh();
	mesh->SetName("Capsule");
	MeshSection* meshSection = new MeshSection();


	point = 0;

	/* top hemisphere */
	thisrow = 0;
	prevrow = 0;
	for (j = 0; j <= (rings + 1); j++)
	{
		v = float(j);

		v /= (rings + 1);
		w = sin(0.5f * M_PI * v);
		z = radius * cos(0.5f * M_PI * v);

		for (i = 0; i <= radialSegments; i++)
		{
			u = float(i);
			u /= radialSegments;

			x = sin(u * (M_PI * 2.0f));
			y = -cos(u * (M_PI * 2.0f));

			glm::vec3 p = glm::vec3(x * radius * w, y * radius * w, z);

			meshSection->positions.emplace_back(p + glm::vec3(0.0f, 0.0f, 0.5f * midHeight));
			meshSection->normals.emplace_back(glm::normalize(p + glm::vec3(0.0f, 0.0f, 0.5f * midHeight)));
			meshSection->texCoords.emplace_back(glm::vec2(u, onethird * v));


			point++;

			if (i > 0 && j > 0)
			{
				meshSection->indices.push_back(thisrow + i - 1);
				meshSection->indices.push_back(prevrow + i);
				meshSection->indices.push_back(prevrow + i - 1);

				meshSection->indices.push_back(thisrow + i - 1);
				meshSection->indices.push_back(thisrow + i);
				meshSection->indices.push_back(prevrow + i);
			};
		};

		prevrow = thisrow;
		thisrow = point;
	};

	/* cylinder */
	thisrow = point;
	prevrow = 0;
	for (j = 0; j <= (rings + 1); j++)
	{
		v = float(j);
		v /= (rings + 1);

		z = midHeight * v;
		z = (midHeight * 0.5f) - z;

		for (i = 0; i <= radialSegments; i++)
		{
			u = float(i);
			u /= radialSegments;

			x = sin(u * (M_PI * 2.0f));
			y = -cos(u * (M_PI * 2.0f));

			glm::vec3 p = glm::vec3(x * radius, y * radius, z);

			meshSection->positions.emplace_back(p);
			meshSection->normals.emplace_back(glm::vec3(x, 0.0f, z));
			meshSection->texCoords.emplace_back(glm::vec2(u, onethird * v));
			meshSection->bounds.Add(p);



			point++;

			if (i > 0 && j > 0)
			{
				meshSection->indices.emplace_back(thisrow + i - 1);
				meshSection->indices.emplace_back(prevrow + i);
				meshSection->indices.emplace_back(prevrow + i - 1);
				meshSection->indices.emplace_back(thisrow + i - 1);
				meshSection->indices.emplace_back(thisrow + i);
				meshSection->indices.emplace_back(prevrow + i);
			};
		};

		prevrow = thisrow;
		thisrow = point;
	};

	/* bottom hemisphere */
	thisrow = point;
	prevrow = 0;

	for (j = 0; j <= (rings + 1); j++)
	{
		v = float(j);

		v /= (rings + 1);
		v += 1.0f;
		w = sin(0.5f * M_PI * v);
		z = radius * cos(0.5f * M_PI * v);

		for (i = 0; i <= radialSegments; i++)
		{
			float u2 = float(i);
			u2 /= radialSegments;

			x = sin(u2 * (M_PI * 2.0f));
			y = -cos(u2 * (M_PI * 2.0f));

			glm::vec3 p = glm::vec3(x * radius * w, y * radius * w, z);

			meshSection->positions.emplace_back(p + glm::vec3(0.0f, 0.0f, -0.5f * midHeight));
			meshSection->normals.emplace_back(glm::normalize(p + glm::vec3(0.0f, 0.0f, -0.5f * midHeight)));
			meshSection->texCoords.emplace_back(glm::vec2(u2, twothirds + ((v - 1.0f) * onethird)));


			point++;

			if (i > 0 && j > 0)
			{
				meshSection->indices.emplace_back(thisrow + i - 1);
				meshSection->indices.emplace_back(prevrow + i);
				meshSection->indices.emplace_back(prevrow + i - 1);
				meshSection->indices.emplace_back(thisrow + i - 1);
				meshSection->indices.emplace_back(thisrow + i);
				meshSection->indices.emplace_back(prevrow + i);
			};
		};

		prevrow = thisrow;
		thisrow = point;
	}

	mesh->AddMeshSection(Ptr<MeshSection>(meshSection));

	return mesh;
}


Mesh* MeshFactory::CreateCylinder(float bottomRadius, float topRadius, float height, int32_t radialSegments, int32_t rings)
{
	int32_t i, j, prevrow, thisrow, point = 0;
	float x, y, z, u, v, radius;


	Mesh* mesh = new Mesh();
	mesh->SetName("Cylinder");
	MeshSection* meshSection = new MeshSection();


	thisrow = 0;
	prevrow = 0;
	for (j = 0; j <= (rings + 1); j++)
	{
		v = float(j);
		v /= (rings + 1);

		radius = topRadius + ((bottomRadius - topRadius) * v);

		y = height * v;
		y = (height * 0.5f) - y;

		for (i = 0; i <= radialSegments; i++)
		{
			u = float(i);
			u /= radialSegments;

			x = sin(u * (M_PI * 2.0f));
			z = cos(u * (M_PI * 2.0f));

			glm::vec3 p = glm::vec3(x * radius, y, z * radius);

			meshSection->positions.emplace_back(p);
			meshSection->normals.emplace_back(glm::vec3(x, 0.0f, z));
			meshSection->texCoords.emplace_back(glm::vec2(u, v * 0.5f));
			meshSection->bounds.Add(p);

			point++;

			if (i > 0 && j > 0)
			{
				meshSection->indices.emplace_back(thisrow + i - 1);
				meshSection->indices.emplace_back(prevrow + i);
				meshSection->indices.emplace_back(prevrow + i - 1);
				meshSection->indices.emplace_back(thisrow + i - 1);
				meshSection->indices.emplace_back(thisrow + i);
				meshSection->indices.emplace_back(prevrow + i);
			};
		};

		prevrow = thisrow;
		thisrow = point;
	};

	// add top
	if (topRadius > 0.0f)
	{
		y = height * 0.5f;

		meshSection->positions.emplace_back(glm::vec3(0.0f, y, 0.0f));
		meshSection->normals.emplace_back(glm::vec3(0.0f, 1.0f, 0.0f));
		meshSection->texCoords.emplace_back(glm::vec2(0.25f, 0.75f));

		point++;

		for (i = 0; i <= radialSegments; i++)
		{
			float r = float(i);
			r /= radialSegments;

			x = sin(r * (M_PI * 2.0f));
			z = cos(r * (M_PI * 2.0f));

			u = ((x + 1.0f) * 0.25f);
			v = 0.5f + ((z + 1.0f) * 0.25f);

			glm::vec3 p = glm::vec3(x * topRadius, y, z * topRadius);

			meshSection->positions.emplace_back(p);
			meshSection->normals.emplace_back(glm::vec3(0.0f, 1.0f, 0.0f));
			meshSection->texCoords.emplace_back(glm::vec2(u, v));

			point++;

			if (i > 0)
			{
				meshSection->indices.emplace_back(point - 2);
				meshSection->indices.emplace_back(point - 1);
				meshSection->indices.emplace_back(thisrow);
			};
		};
	};

	// add bottom
	if (bottomRadius > 0.0f)
	{
		y = height * -0.5f;

		thisrow = point;


		meshSection->positions.emplace_back(glm::vec3(0.0f, y, 0.0f));
		meshSection->normals.emplace_back(glm::vec3(0.0f, -1.0f, 0.0f));
		meshSection->texCoords.emplace_back(glm::vec2(0.75f, 0.75f));

		point++;

		for (i = 0; i <= radialSegments; i++)
		{
			float r = float(i);
			r /= radialSegments;

			x = std::sin(r * (M_PI * 2.0f));
			z = std::cos(r * (M_PI * 2.0f));

			u = 0.5f + ((x + 1.0f) * 0.25f);
			v = 1.0f - ((z + 1.0f) * 0.25f);

			glm::vec3 p = glm::vec3(x * bottomRadius, y, z * bottomRadius);

			meshSection->positions.emplace_back(p);
			meshSection->normals.emplace_back(glm::vec3(0.0f, -1.0f, 0.0f));
			meshSection->texCoords.emplace_back(glm::vec2(u, v));

			point++;

			if (i > 0)
			{
				meshSection->indices.emplace_back(point - 1);
				meshSection->indices.emplace_back(point - 2);
				meshSection->indices.emplace_back(thisrow);
			};
		};
	};


	mesh->AddMeshSection(Ptr<MeshSection>(meshSection));

	return mesh;
}


std::string MeshFactory::GetBasicShapeResroucePath(EBasicShape::Type type)
{
	return BASIC_SHAPES_DIR + EBasicShape::NAMES[type] + ".raven";
}


Ptr<Mesh> MeshFactory::GetBasicShape(EBasicShape::Type type)
{
	std::string sahpeRscPath = MeshFactory::GetBasicShapeResroucePath(type);

	// Try to get the resource.
	Ptr<Mesh> shape = Engine::GetModule<ResourceManager>()->GetResource<Mesh>(sahpeRscPath);

	// No shape resrouce found?
	if (shape != nullptr)
		return shape;


	// Create new shape...
	Mesh* newShape;

	switch (type)
	{
	case EBasicShape::Cube:
		newShape = CreateCube();
		break;

	case EBasicShape::Plane: 
		newShape = CreatePlane(1.0f, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		break;

	case EBasicShape::Quad: 
		newShape = CreateQuad();
		break;

	case EBasicShape::Sphere:
		newShape = CreateSphere();
		break;

	case EBasicShape::Pyramid:
		newShape = CreatePyramid();
		break;

	case EBasicShape::Capsule:
		newShape = CreateCapsule();
		break;

	case EBasicShape::Cylinder:
		newShape = CreateCylinder();
		break;
	}

	// Generate Tangents..
	auto& meshSetions = newShape->GetMeshLOD(0).sections;

	for (uint32_t i = 0; i < meshSetions.size(); ++i)
	{
		MeshSection* meshSection = meshSetions[i].get();

		meshSection->tangents.resize( meshSection->normals.size() );
		Raven::ComputeTangents(meshSection->tangents.data(), meshSection->indices.size(), meshSection->indices.data(),
			meshSection->positions.data(), meshSection->normals.data(), meshSection->texCoords.data());
	}

	// Save the resrouce...
	shape = Ptr<Mesh>(newShape);
	Engine::GetModule<ResourceManager>()->SaveNewResource(shape, sahpeRscPath);

	// Load Render Data...
	shape->LoadRenderResource();

	return shape;
}


std::string EBasicShape::GetBasicShapeName(EBasicShape::Type type)
{
	if (type >= NAMES.size() || type < 0){
		return "";
	}
	return NAMES[(uint32_t)type];
}


EBasicShape::Type EBasicShape::GetPrimativeName(const std::string& name)
{
	for (uint32_t i = 0; i < EBasicShape::LENGTH; ++i)
	{
		if (NAMES[i] == name)
			return (EBasicShape::Type)i;
	}

	return  EBasicShape::LENGTH;
}


void ComputeTangents(glm::vec3* out, uint32_t indices_count, uint32_t* indices,
	const glm::vec3* vertices, const glm::vec3* normals, const glm::vec2* uvs)
{
	for (int i = 0; i < indices_count; i += 3)
	{
		const glm::vec3 v0 = vertices[ indices[i + 0] ];
		const glm::vec3 v1 = vertices[ indices[i + 1] ];
		const glm::vec3 v2 = vertices[ indices[i + 2] ];
		const glm::vec2 uv0 = uvs[ indices[i + 0] ];
		const glm::vec2 uv1 = uvs[ indices[i + 1] ];
		const glm::vec2 uv2 = uvs[ indices[i + 2] ];

		const glm::vec3 dv10 = v1 - v0;
		const glm::vec3 dv20 = v2 - v0;
		const glm::vec2 duv10 = uv1 - uv0;
		const glm::vec2 duv20 = uv2 - uv0;

		const float dir = duv20.x * duv10.y - duv20.y * duv10.x < 0 ? -1.f : 1.f;
		glm::vec3 tangent;
		tangent.x = (dv20.x * duv10.y - dv10.x * duv20.y) * dir;
		tangent.y = (dv20.y * duv10.y - dv10.y * duv20.y) * dir;
		tangent.z = (dv20.z * duv10.y - dv10.z * duv20.y) * dir;

		const float l = 1 / sqrtf(float(tangent.x * tangent.x + tangent.y * tangent.y + tangent.z * tangent.z));
		tangent.x *= l;
		tangent.y *= l;
		tangent.z *= l;

		out[ indices[i] + 0 ] = tangent;
		out[ indices[i] + 1 ] = tangent;
		out[ indices[i] + 2 ] = tangent;
	}
}



} // End of namespace Raven.

