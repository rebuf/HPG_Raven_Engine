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
#include "FBXImporter.h"

#include "ResourceManager/FileSystem.h"
#include "ResourceManager/Resources/Mesh.h"
#include "ResourceManager/Resources/SkinnedMesh.h"
#include "ResourceManager/MeshFactory.h"

#include "Animation/Skeleton.h"
#include "Animation/Animation.h"

#include "Utilities/StringUtils.h"


#include "ofbx.h"


#define FBX_CAST(obj, type) static_cast<const type*>(obj)




namespace Raven {


//  ofbx::Vec3 subtract.
static ofbx::Vec3 operator-(const ofbx::Vec3& a, const ofbx::Vec3& b)
{
	return { a.x - b.x, a.y - b.y, a.z - b.z };
}

//  ofbx::Vec2 subtract.
static ofbx::Vec2 operator-(const ofbx::Vec2& a, const ofbx::Vec2& b)
{
	return { a.x - b.x, a.y - b.y };
}


glm::vec3 FbxLoader::FixOrientation(const glm::vec3& v)
{
	switch (fbx_orientation)
	{
	case FbxOrientation::Y_UP:
		return glm::vec3(v.x, v.y, v.z);
	case FbxOrientation::Z_UP:
		return glm::vec3(v.x, v.z, -v.y);
	case FbxOrientation::Z_MINUS_UP:
		return glm::vec3(v.x, -v.z, v.y);
	case FbxOrientation::X_MINUS_UP:
		return glm::vec3(v.y, -v.x, v.z);
	case FbxOrientation::X_UP:
		return glm::vec3(-v.y, v.x, v.z);
	}
	return v;
}


glm::quat FbxLoader::FixOrientation(const glm::quat& v)
{
	switch (fbx_orientation)
	{
	case FbxOrientation::Y_UP:
		return { v.w,v.x, v.y, v.z };
	case FbxOrientation::Z_UP:
		return { v.w,v.x,-v.y, v.z };
	case FbxOrientation::Z_MINUS_UP:
		return { v.w,v.x, -v.z, v.y };
	case FbxOrientation::X_MINUS_UP:
		return { v.w, -v.x, v.z, v.y };
	case FbxOrientation::X_UP:
		return { -v.y, v.x, v.z, v.w };
	}
	return v;
}



FbxLoader::FbxLoader()
	: skeleton(nullptr)
	, fbx_scene(nullptr)
{
	fbx_orientation = FbxOrientation::Y_UP;
	fbx_scale = 1.f;

	fbx_containAnimations = false;
	fbx_containSkin = false;
}


FbxLoader::~FbxLoader()
{
	if (fbx_scene)
	{
		fbx_scene->destroy();
	}
}


bool FbxLoader::Open(const std::string& file)
{
	int64_t size = 0;
	auto data = FileSystem::ReadFile(file, size);

	// Failed to read?
	if (!data)
	{
		return false;
	}

	// Load fbx file onto ofbx scene.
	const uint64_t flags = (uint64_t)ofbx::LoadFlags::TRIANGULATE;
	fbx_scene = ofbx::load(data.get(), uint32_t(size), flags);

	// Failed to load?
	if (!fbx_scene)
	{
		return false;
	}


	// Fbx Global Settings...
	const ofbx::GlobalSettings* settings = fbx_scene->getGlobalSettings();

	switch (settings->UpAxis)
	{
	case ofbx::UpVector_AxisX:
		fbx_orientation = FbxOrientation::X_UP;
		break;
	case ofbx::UpVector_AxisY:
		fbx_orientation = FbxOrientation::Y_UP;
		break;
	case ofbx::UpVector_AxisZ:
		fbx_orientation = FbxOrientation::Z_UP;
		break;
	}


	// Find and Collect the objects we have in the fbx_scene...
	for (int32_t i = 0; i < fbx_scene->getAllObjectCount(); ++i)
	{
		auto obj = fbx_scene->getAllObjects()[i];

		// Is Mesh?
		if (obj->getType() == ofbx::Object::Type::MESH)
		{
			auto meshObj = FBX_CAST(obj, ofbx::Mesh);

			fbx_meshes.push_back(meshObj);

			// Is Skinned?
			if (meshObj->getGeometry()->getSkin())
			{
				fbx_containSkin = true;
			}
		}

		// Is Bone?
		if (obj->getType() == ofbx::Object::Type::LIMB_NODE)
		{
			fbx_bones.push_back(obj);
		}
	}

	fbx_containAnimations = fbx_scene->getAnimationStackCount() > 0;

	return true;
}


bool FbxLoader::HasMesh()
{
	return !fbx_meshes.empty();
}


bool FbxLoader::HasAnimation()
{
	return fbx_containAnimations;
}


bool FbxLoader::HasSkin()
{
	return fbx_containSkin;
}


bool FbxLoader::HasBones()
{
	return fbx_bones.size();
}


glm::mat4 FbxLoader::FbxMatrixToGLM(const ofbx::Matrix& mat)
{
	glm::mat4 result;
	for (int32_t i = 0; i < 4; i++)
		for (int32_t j = 0; j < 4; j++)
			result[i][j] = (float)mat.m[i * 4 + j];

	return result;
}


// Not Implemented
static void ImportWeights(const ofbx::Geometry* fbx_geometry, const std::vector<int32_t>& indexMap,
	Skeleton* skeleton, MeshSection* section) { RAVEN_ASSERT(0, "Not Implemented."); }


// Extract weights for each vertex.
static void ImportWeights(const ofbx::Geometry* fbx_geometry, const std::vector<int32_t>& indexMap,
	Skeleton* skeleton, SkinnedMeshSection* section)
{
	section->blendIndices.resize(section->positions.size(), glm::ivec4(0));
	section->blendWeights.resize(section->positions.size(), glm::vec4(0.0f));

	// Load skeleton before importing weights.
	if (!skeleton)
		return;

	//
	auto fbx_skin = fbx_geometry->getSkin();

	for (int32_t clusterIndex = 0; clusterIndex < fbx_skin->getClusterCount(); clusterIndex++)
	{
		const ofbx::Cluster* cluster = fbx_skin->getCluster(clusterIndex);
		if (cluster->getIndicesCount() == 0)
			continue;

		const auto link = cluster->getLink();

		const int32_t boneIndex = skeleton->GetBoneIndex(link->name);
		if (boneIndex == -1)
		{
			LOGC("Missing bone");
			return;
		}
		const int* clusterIndices = cluster->getIndices();
		const double* clusterWeights = cluster->getWeights();
		for (int j = 0; j < cluster->getIndicesCount(); j++)
		{
			int vtxIndex = indexMap[ clusterIndices[j] ];
			float vtxWeight = (float)clusterWeights[j];

			if (vtxWeight <= 0 || vtxIndex < 0 || vtxIndex >= section->positions.size())
				continue;

			auto& indices = section->blendIndices[vtxIndex];
			auto& weights = section->blendWeights[vtxIndex];

			for (int32_t k = 0; k < 4; k++)
			{
				if (vtxWeight >= weights[k])
				{
					for (int32_t l = 2; l >= k; l--)
					{
						indices[l + 1] = indices[l];
						weights[l + 1] = weights[l];
					}

					indices[k] = boneIndex;
					weights[k] = vtxWeight;
					break;
				}
			}
		}
	}

}




// Generic import mesh function for both mesh and skinned mesh.
template<class TMeshSection, EResourceType MESH_TYPE>
static void ImportMeshImp(const std::vector<const ofbx::Mesh*> fbx_meshes, 
	float fbx_scale, std::vector< Ptr<TMeshSection> >& meshSections, Skeleton* skeleton)
{
	std::unordered_map<const ofbx::Material*, int32_t> matMap;

	// Null Material Section.
	matMap[nullptr] = 0;
	meshSections.emplace_back(new TMeshSection());

	for (int32_t meshIndex = 0; meshIndex < fbx_meshes.size(); ++meshIndex)
	{
		const ofbx::Mesh* fbx_mesh = fbx_meshes[meshIndex];
		const ofbx::Geometry* fbx_geometry = fbx_mesh->getGeometry();

		// Global transform to apply on mesh.
		glm::mat4 fbx_transform;
		{
			auto fbx_globalMatrix = fbx_mesh->getGlobalTransform();
			auto fbx_geometricMatrix = fbx_mesh->getGeometricMatrix();
			fbx_transform = FbxLoader::FbxMatrixToGLM(fbx_globalMatrix) * FbxLoader::FbxMatrixToGLM(fbx_geometricMatrix);
		}


		// Map Material index to its section.
		int32_t matCount = glm::max(1, fbx_mesh->getMaterialCount());

		std::vector<int32_t> matIndexMap;
		matIndexMap.resize(matCount);

		// Create or Map materials to their section.
		for (int32_t im = 0; im < matCount; ++im)
		{
			const ofbx::Material* fbx_materail = matCount == fbx_mesh->getMaterialCount() ? fbx_mesh->getMaterial(im) : nullptr;

			if (matMap.count(fbx_materail))
			{
				matMap[fbx_materail] = meshSections.size();
				matIndexMap[im] = meshSections.size();
				meshSections.emplace_back(new TMeshSection());
			}
			else
			{
				matIndexMap[im] = matMap[fbx_materail];
			}


			// Section
			TMeshSection* section = meshSections[matIndexMap[im]].get();

			// Data.
			auto fbx_vertexCount = fbx_geometry->getVertexCount();
			auto fbx_vertices = fbx_geometry->getVertices();
			auto fbx_normals = fbx_geometry->getNormals();
			auto fbx_uvs = fbx_geometry->getUVs();
			auto fbx_materials = fbx_geometry->getMaterials();
			auto fbx_indices = fbx_geometry->getFaceIndices();
			auto fbx_indicesCount = fbx_geometry->getIndexCount();
			RAVEN_ASSERT(fbx_indicesCount == fbx_vertexCount, "Make sure our assumption is correct.");

			std::vector<int32_t> indexMap;
			indexMap.resize(fbx_vertexCount, -1);

			// Write all vertices for this materail section
			for (int iv = 0; iv < fbx_vertexCount; ++iv)
			{
				if (fbx_materials && fbx_materials[iv / 3] != im)
					continue;

				int32_t vert_index = section->positions.size();
				indexMap[iv] = vert_index;

				ofbx::Vec3 fbx_vertex = fbx_vertices[iv];
				ofbx::Vec3 fbx_normal = fbx_normals[iv];
				ofbx::Vec2 fbx_uv = fbx_uvs[iv];

				glm::vec3 pos(fbx_transform * glm::vec4(fbx_vertex.x, fbx_vertex.y, fbx_vertex.z, 1.0f));
				glm::vec3 normal(fbx_transform * glm::vec4(fbx_normal.x, fbx_normal.y, fbx_normal.z, 0.0f));
				glm::vec2 uv(fbx_uv.x, 1.0f - fbx_uv.y);

				// Add Vertex Data.
				section->positions.push_back(pos * fbx_scale);
				section->normals.push_back(glm::normalize(normal));
				section->texCoords.push_back(uv);

				// Update Bounds.
				section->bounds.Add(section->positions.back());
			}

			// Skinned Mesh?
			if (MESH_TYPE == RT_SkinnedMesh)
			{
				// import skin weights...
				ImportWeights(fbx_geometry, indexMap, skeleton, section);
			}

			// Write all indices for this materail section
			for (int in = 0; in < fbx_vertexCount; ++in)
			{
				if (fbx_materials && fbx_materials[in / 3] != im)
					continue;

				section->indices.push_back(indexMap[in]);
			}

		}
	}

}


Mesh* FbxLoader::ImportMesh()
{
	std::vector< Ptr<MeshSection> > meshSections;
	ImportMeshImp<MeshSection, RT_Mesh>(fbx_meshes, fbx_scale, meshSections, nullptr);

	// The Imported mesh.
	Mesh* mesh = new Mesh();

	// Add the sections to the new mesh.
	for (int32_t i = 0; i < meshSections.size(); ++i)
	{
		auto& section = meshSections[i];

		// Empty Section?
		if (section->positions.empty())
			continue;

		// Generate Tangents...
		section->tangents.resize(section->normals.size());
		Raven::ComputeTangents(section->tangents.data(), section->indices.size(), section->indices.data(),
			section->positions.data(), section->normals.data(), section->texCoords.data());

		// Add.
		mesh->AddMeshSection(section);
	}

	return mesh;
}


SkinnedMesh* FbxLoader::ImportSkinnedMesh()
{
	std::vector< Ptr<SkinnedMeshSection> > meshSections;
	ImportMeshImp<SkinnedMeshSection, RT_SkinnedMesh>(fbx_meshes, fbx_scale, meshSections, skeleton);

	// The Imported mesh.
	SkinnedMesh* mesh = new SkinnedMesh();

	// Add the sections to the new mesh.
	for (int32_t i = 0; i < meshSections.size(); ++i)
	{
		auto& section = meshSections[i];

		// Empty Section?
		if (section->positions.empty())
			continue;

		// Generate Tangents...
		section->tangents.resize(section->normals.size());
		Raven::ComputeTangents(section->tangents.data(), section->indices.size(), section->indices.data(),
			section->positions.data(), section->normals.data(), section->texCoords.data());

		// Add.
		mesh->AddMeshSection(section);
	}

	return mesh;
}


glm::mat4 FbxLoader::GetOffsetMatrix(const ofbx::Mesh* mesh, const ofbx::Object* node)
{
	auto* skin = mesh ? mesh->getGeometry()->getSkin() : nullptr;
	if (skin)
	{
		for (int i = 0, c = skin->getClusterCount(); i < c; i++)
		{
			const ofbx::Cluster* cluster = skin->getCluster(i);
			if (cluster->getLink() == node)
			{
				return FbxMatrixToGLM(cluster->getTransformLinkMatrix());
			}
		}
	}
	return FbxMatrixToGLM(node->getGlobalTransform());
}


Skeleton* FbxLoader::ImportSkeleton()
{
	skeleton = new Skeleton();

	for (auto fbx_mesh : fbx_meshes)
	{
		auto skin = fbx_mesh->getGeometry()->getSkin();

		for (int32_t clusterIndex = 0; clusterIndex < skin->getClusterCount(); clusterIndex++)
		{
			const ofbx::Cluster* cluster = skin->getCluster(clusterIndex);

			if (cluster->getIndicesCount() == 0)
				continue;

			const auto link = cluster->getLink();
			RAVEN_ASSERT(link->getType() == ofbx::Object::Type::LIMB_NODE, "Make sure our assumption is correct.");

			auto boneIndex = skeleton->GetBoneIndex(link->name);//create a bone if missing

			if (boneIndex == -1)
			{
				auto& bone = skeleton->CreateBone(skeleton->GetBoneIndex(link->getParent()->name));
				bone.SetName(link->name);
				bone.SetOffsetMatrix(glm::inverse(GetOffsetMatrix(fbx_mesh, link)));
				bone.SetRestPose(FbxMatrixToGLM(link->getLocalTransform()));
			}
		}
	}

	// Finalize skeleton building...
	skeleton->Build();

	return skeleton;
}


void FbxLoader::ImportAnimation(std::vector< Ptr<AnimationClip> >& clips)
{
	// Load skeleton before importing animation.
	if (!skeleton)
		return;

	float frameRate = fbx_scene->getSceneFrameRate();
	if (frameRate <= 0)
		frameRate = 30.0f;


	const int animCount = fbx_scene->getAnimationStackCount();

	for (int32_t animIndex = 0; animIndex < animCount; animIndex++)
	{
		auto clip = ImportAnimationClip(animIndex, frameRate);

		if (clip != nullptr) 
		{
			clips.emplace_back(clip);
		}
	}
}


static void GetCurveData(AnimationCurveProperty& curve, const ofbx::AnimationCurve* node)
{
	if (node != nullptr)
	{
		auto times = node->getKeyTime();
		for (auto i = 0; i < node->getKeyCount(); i++)
		{
			curve.curve.AddKey(ofbx::fbxTimeToSeconds(times[i]), node->getKeyValue()[i], 1, 1);
		}
	}
}


Ptr<AnimationClip> FbxLoader::ImportAnimationClip(int32_t index, float frameRate)
{
	const ofbx::AnimationStack* stack = fbx_scene->getAnimationStack(index);
	const ofbx::AnimationLayer* layer = stack->getLayer(0);
	const ofbx::TakeInfo* takeInfo = fbx_scene->getTakeInfo(stack->name);
	if (takeInfo == nullptr)
		return nullptr;

	const double localDuration = takeInfo->local_time_to - takeInfo->local_time_from;
	if (localDuration <= MathUtils::EPS)
		return nullptr;
	auto animationDuration = (int32_t)(localDuration * frameRate + 0.5f);

	auto clip = std::make_shared<AnimationClip>();

	clip->wrapMode = AnimationWrapMode::Loop;
	clip->length = localDuration;//animationDuration;

	char name[256];
	takeInfo->name.toString(name);
	clip->clipName = name;

	// Import curves
	for (int32_t i = 0; i < fbx_bones.size(); i++)
	{
		const ofbx::AnimationCurveNode* translationNode = layer->getCurveNode(*fbx_bones[i], "Lcl Translation");
		const ofbx::AnimationCurveNode* rotationNode = layer->getCurveNode(*fbx_bones[i], "Lcl Rotation");

		if (rotationNode)
		{
			auto& curve0 = clip->curves.emplace_back();
			//curve0.path = GetBonePath(bones[i]);
			curve0.index = skeleton->GetBoneIndex(fbx_bones[i]->name);
			RAVEN_ASSERT(curve0.index != -1, "Bone not found.");

			auto& cur0 = curve0.properties.emplace_back();
			GetCurveData(cur0, rotationNode->getCurve(0));
			cur0.type = AnimationCurvePropertyType::LocalRotationX;
			auto& cur1 = curve0.properties.emplace_back();
			GetCurveData(cur1, rotationNode->getCurve(1));
			cur1.type = AnimationCurvePropertyType::LocalRotationY;
			auto& cur2 = curve0.properties.emplace_back();
			GetCurveData(cur2, rotationNode->getCurve(2));
			cur2.type = AnimationCurvePropertyType::LocalRotationZ;

		}

		if (translationNode)
		{
			auto& curve = clip->curves.emplace_back();
			//curve.path = GetBonePath(bones[i]);
			curve.index = skeleton->GetBoneIndex(fbx_bones[i]->name);
			RAVEN_ASSERT(curve.index != -1, "Bone not found.");

			auto& cur0 = curve.properties.emplace_back();
			GetCurveData(cur0, translationNode->getCurve(0));
			cur0.type = AnimationCurvePropertyType::LocalPositionX;
			auto& cur1 = curve.properties.emplace_back();
			GetCurveData(cur1, translationNode->getCurve(1));
			cur1.type = AnimationCurvePropertyType::LocalPositionY;
			auto& cur2 = curve.properties.emplace_back();
			GetCurveData(cur2, translationNode->getCurve(2));
			cur2.type = AnimationCurvePropertyType::LocalPositionZ;
		}

	}
	return clip;
}




// -- - --- - -- - --- - -- - --- - -- - --- - -- - --- - -- - --- - -- - --- - -- - --- - 






FBXImporter::FBXImporter()
{
	type = StaticGetType();
}


void FBXImporter::ListExtensions(std::vector<std::string>& outExt)
{
	outExt.push_back("fbx");
}


bool FBXImporter::Import(const std::string& path, std::vector< Ptr<IResource> >& resources)
{
	FbxLoader fbx;

	// Open?
	if (!fbx.Open(path))
	{
		LOGW("FBXImporter - Failed to open fbx file. ");
		return false;
	}

	std::string name = StringUtils::GetFileNameWithoutExtension(path);
	Ptr<Skeleton> skeleton;

	// Import animation?
	if (settings.importAnimationOnly)
	{
		RAVEN_ASSERT(settings.skeleton, "Invalid Skeleton.");
		ImportAnimation(fbx, settings.skeleton, name, resources);
	}
	else
	{
		// Has skinned mesh & skeleton?
		if (fbx.HasSkin() && fbx.HasBones() && !settings.importMeshOnly)
		{
			skeleton = Ptr<Skeleton>(fbx.ImportSkeleton());
			skeleton->SetName("SKELETON_" + name);
			resources.push_back(skeleton);

			Ptr<SkinnedMesh> mesh(fbx.ImportSkinnedMesh());
			mesh->SetName("SKINNEDMESH_" + name);
			mesh->SetSkeleton(skeleton);
			resources.push_back(mesh);

			if (fbx.HasAnimation())
			{
				ImportAnimation(fbx, skeleton, name, resources);
			}
		}
		else if (fbx.HasMesh()) // Only Mesh?
		{
			Ptr<Mesh> mesh(fbx.ImportMesh());
			mesh->SetName("MESH_" + name);

			resources.push_back(mesh);
		}
	}
	



	// Reset Settings.
	settings = FBXImporterSettings();

	return true;
}


void FBXImporter::ImportAnimation(FbxLoader& fbx, Ptr<Skeleton> skeleton, const std::string& name, std::vector< Ptr<IResource> >& resources)
{
	if (!fbx.HasAnimation())
	{
		LOGW("Fbx no animation to import.");
		return;
	}

	// Set the skeleton to be used for animation importing.
	fbx.skeleton = skeleton.get();

	// Import...
	std::vector< Ptr<AnimationClip> > clips;
	fbx.ImportAnimation(clips);

	for (uint32_t i = 0; i < (uint32_t)clips.size(); ++i)
	{
		clips[i]->skeleton = skeleton;
		clips[i]->SetName("ANIM_CLIP_" + name + "_" + std::to_string(i));

		// Add.
		resources.emplace_back(clips[i]);
	}

}


} // End of namespace Raven
