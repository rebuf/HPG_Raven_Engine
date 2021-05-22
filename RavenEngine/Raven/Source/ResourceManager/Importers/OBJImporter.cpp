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
#include "OBJImporter.h"
#include "Utilities/StringUtils.h"

#include "ResourceManager/Resources/Mesh.h"
#include "ResourceManager/MeshFactory.h"



#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobjloader/tiny_obj_loader.h>
#include <glm/glm.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>




namespace Raven {



OBJImporter::OBJImporter()
{
  type = StaticGetType();
}


void OBJImporter::ListExtensions(std::vector<std::string>& outExt)
{
	outExt.push_back("obj");
}


bool OBJImporter::Import(const std::string& path, std::vector< Ptr<IResource> >& resources)
{
  IResource* newRsc = LoadOBJ(path);

  // Failed?
  if (!newRsc)
  {
    return false;
  }

  resources.emplace_back(newRsc);
	return true;
}


IResource* OBJImporter::LoadOBJ(const std::string& path)
{
  // setup variables to get model info
  tinyobj::attrib_t attrib; // contains all the positions, normals, textures and faces
  std::vector<tinyobj::shape_t> shapes; // all the separate objects and their faces
  std::vector<tinyobj::material_t> materials; // object materials
  std::string err;

  // load the model, show error if not
  if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, path.c_str())) 
  {
    LOGE(err);
    return nullptr;
  }

  // The Mesh.
  Mesh* mesh = new Mesh();

  // loop over the shapes in the model
  for (const auto& shape : shapes)
  {
    // one shape = one mesh section
    Ptr<MeshSection> meshSection(new MeshSection());

    // resize the mesh data
    for (const auto& index : shape.mesh.indices)
    {
      // set vertex data 
      meshSection->positions.push_back(glm::vec3(
        attrib.vertices[3 * index.vertex_index + 0],
        attrib.vertices[3 * index.vertex_index + 1],
        attrib.vertices[3 * index.vertex_index + 2]));

      // Update Bounds.
      meshSection->bounds.Add(meshSection->positions.back());

      // check that normals and texcoords exist, add accordingly
      if (index.normal_index == -1)
      {
        meshSection->normals.push_back(glm::vec3(0.0f));
      }
      else
      {
        meshSection->normals.push_back(glm::vec3(
          attrib.normals[3 * index.normal_index + 0],
          attrib.normals[3 * index.normal_index + 1],
          attrib.normals[3 * index.normal_index + 2]));
      }

      if (index.texcoord_index == -1)
      {
        meshSection->texCoords.push_back(glm::vec2(0.0f));
      }
      else
      {
        meshSection->texCoords.push_back(glm::vec2(
          attrib.texcoords[2 * index.texcoord_index + 0],
          1.0f - attrib.texcoords[2 * index.texcoord_index + 1]));
      }

      meshSection->indices.push_back(static_cast<uint32_t>(meshSection->indices.size()));
    }


    // Generate Tangents...
    meshSection->tangents.resize(meshSection->normals.size());
    Raven::ComputeTangents(meshSection->tangents.data(), meshSection->indices.size(), meshSection->indices.data(),
      meshSection->positions.data(), meshSection->normals.data(), meshSection->texCoords.data());

    mesh->AddMeshSection(meshSection);
  }


  //
  mesh->SetName( "MESH_" + StringUtils::GetFileNameWithoutExtension(path) );

  return mesh;
}



} // End of namespace Raven
