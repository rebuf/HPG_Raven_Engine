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

#include "FbxLoader_deprecated.h"

#include "ResourceManager/FileSystem.h"
#include "Logger/Console.h"
#include "ResourceManager/Resources/Mesh.h"
#include "ResourceManager/ResourceManager.h"
#include "Scene/Component/Model_deprecated.h"
#include "Scene/Component/Transform.h"
#include "Scene/Entity/EntityManager.h"
#include "Utilities/StringUtils.h"
#include "Scene/SceneManager.h"
#include "Scene/Entity/Entity.h"
#include "Engine.h"
#include "Animation/Skeleton.h"
#include "Animation/Animation.h"
#include <glm/gtx/matrix_decompose.hpp>

#include <memory>

namespace Raven {

	static std::string GetBonePath(const ofbx::Object* bone)
	{
		if (bone == nullptr) {
			return "";
		}
		return GetBonePath(bone->getParent()) + "/" + bone->name;
	}


	enum class Orientation
	{
		Y_UP,
		Z_UP,
		Z_MINUS_UP,
		X_MINUS_UP,
		X_UP
	};

	Orientation orientation = Orientation::Y_UP;
	float fbx_scale = 1.f;

	static ofbx::Vec3 operator-(const ofbx::Vec3& a, const ofbx::Vec3& b)
	{
		return { a.x - b.x, a.y - b.y, a.z - b.z };
	}

	static ofbx::Vec2 operator-(const ofbx::Vec2& a, const ofbx::Vec2& b)
	{
		return { a.x - b.x, a.y - b.y };
	}

	glm::vec3 FixOrientation(const glm::vec3& v)
	{
		switch (orientation)
		{
		case Orientation::Y_UP:
			return glm::vec3(v.x, v.y, v.z);
		case Orientation::Z_UP:
			return glm::vec3(v.x, v.z, -v.y);
		case Orientation::Z_MINUS_UP:
			return glm::vec3(v.x, -v.z, v.y);
		case Orientation::X_MINUS_UP:
			return glm::vec3(v.y, -v.x, v.z);
		case Orientation::X_UP:
			return glm::vec3(-v.y, v.x, v.z);
		}
		return v;
	}

	glm::quat FixOrientation(const glm::quat& v)
	{
		switch (orientation)
		{
		case Orientation::Y_UP:
			return { v.w,v.x, v.y, v.z };
		case Orientation::Z_UP:
			return { v.w,v.x,-v.y, v.z };
		case Orientation::Z_MINUS_UP:
			return { v.w,v.x, -v.z, v.y };
		case Orientation::X_MINUS_UP:
			return { v.w, -v.x, v.z, v.y };
		case Orientation::X_UP:
			return { -v.y, v.x, v.z, v.w };
		}
		return v;
	}
	static void SetTransform(const ofbx::Object* object, Transform& transform) {
		ofbx::Vec3 p = object->getLocalTranslation();
		glm::vec3 pos = (glm::vec3(static_cast<float>(p.x), static_cast<float>(p.y), static_cast<float>(p.z)));
		transform.SetPosition(FixOrientation(pos));
		ofbx::Vec3 r = object->getLocalRotation();
		glm::vec3 rot = FixOrientation(glm::vec3(static_cast<float>(r.x), static_cast<float>(r.y), static_cast<float>(r.z)));
		transform.SetRotation(rot);
		ofbx::Vec3 s = object->getLocalScaling();
		glm::vec3 scl = glm::vec3(static_cast<float>(s.x), static_cast<float>(s.y), static_cast<float>(s.z));
		transform.SetScale(scl);
	}

	static void computeTangents(ofbx::Vec3* out, int vertex_count, const ofbx::Vec3* vertices, const ofbx::Vec3* normals, const ofbx::Vec2* uvs)
	{
		for (int i = 0; i < vertex_count; i += 3)
		{
			const ofbx::Vec3 v0 = vertices[i + 0];
			const ofbx::Vec3 v1 = vertices[i + 1];
			const ofbx::Vec3 v2 = vertices[i + 2];
			const ofbx::Vec2 uv0 = uvs[i + 0];
			const ofbx::Vec2 uv1 = uvs[i + 1];
			const ofbx::Vec2 uv2 = uvs[i + 2];

			const ofbx::Vec3 dv10 = v1 - v0;
			const ofbx::Vec3 dv20 = v2 - v0;
			const ofbx::Vec2 duv10 = uv1 - uv0;
			const ofbx::Vec2 duv20 = uv2 - uv0;

			const float dir = duv20.x * duv10.y - duv20.y * duv10.x < 0 ? -1.f : 1.f;
			ofbx::Vec3 tangent;
			tangent.x = (dv20.x * duv10.y - dv10.x * duv20.y) * dir;
			tangent.y = (dv20.y * duv10.y - dv10.y * duv20.y) * dir;
			tangent.z = (dv20.z * duv10.y - dv10.z * duv20.y) * dir;
			const float l = 1 / sqrtf(float(tangent.x * tangent.x + tangent.y * tangent.y + tangent.z * tangent.z));
			tangent.x *= l;
			tangent.y *= l;
			tangent.z *= l;
			out[i + 0] = tangent;
			out[i + 1] = tangent;
			out[i + 2] = tangent;
		}
	}

	static void LoadTransforms(const ofbx::Object* node, Entity& parent,std::vector<const ofbx::Object*> & bones)
	{
		int32_t i = 0;
		while (ofbx::Object* child = node->resolveObjectLink(i++))
		{
			if (child->getType() == ofbx::Object::Type::LIMB_NODE || 
				child->getType() == ofbx::Object::Type::NULL_NODE ||
				child->getType() == ofbx::Object::Type::MESH
				)
			{
				auto currentScene = Engine::Get().GetModule<SceneManager>()->GetCurrentScene();
				auto entity = currentScene->CreateEntity(child->name);
				auto& transform = entity.AddComponent<Transform>();
				SetTransform(child, transform);
				LoadTransforms(child, entity, bones);
				entity.SetParent(parent);
				if (child->getType() == ofbx::Object::Type::LIMB_NODE)
				{
					bones.emplace_back(child);
				}
			}
		}
	}

	glm::mat4 ToMatrix(const ofbx::Matrix& mat)
	{
		glm::mat4  result;
		for (int32_t i = 0; i < 4; i++)
			for (int32_t j = 0; j < 4; j++)
				result[i][j] = (float)mat.m[i * 4 + j];
		return result;
	}

	glm::mat4 GetOffsetMatrix(const ofbx::Mesh* mesh, const ofbx::Object* node)
	{
		auto* skin = mesh ? mesh->getGeometry()->getSkin() : nullptr;
		if (skin)
		{
			for (int i = 0, c = skin->getClusterCount(); i < c; i++)
			{
				const ofbx::Cluster* cluster = skin->getCluster(i);
				if (cluster->getLink() == node)
				{
					return ToMatrix(cluster->getTransformLinkMatrix());
				}
			}
		}
		return ToMatrix(node->getGlobalTransform());
	}


	FbxLoader_deprecated::~FbxLoader_deprecated()
	{
		if (scene) 
		{
			scene->destroy();
		}
	}


	void FbxLoader_deprecated::Load(const std::string& path, Model_deprecated* model)
	{
		LoadHierarchy(path, model);
		LoadMeshes(model);
		LoadAnimation(model);
	}

	void FbxLoader_deprecated::LoadHierarchy(const std::string& path, Model_deprecated* model)
	{
		file = path;
		std::string err;
		std::string name = StringUtils::GetFileName(path);
		std::string ext = StringUtils::GetExtension(name);
		int64_t size = 0;
		auto data = FileSystem::ReadFile(path, size);

		if (data == nullptr)
		{
			LOGW("Failed to load fbx file"); return;
		}
		const bool ignoreGeometry = false;
		const uint64_t flags = ignoreGeometry ? (uint64_t)ofbx::LoadFlags::IGNORE_GEOMETRY : (uint64_t)ofbx::LoadFlags::TRIANGULATE;
		scene = ofbx::load(data.get(), uint32_t(size), flags);

		const ofbx::GlobalSettings* settings = scene->getGlobalSettings();
		switch (settings->UpAxis)
		{
		case ofbx::UpVector_AxisX:
			orientation = Orientation::X_UP;
			break;
		case ofbx::UpVector_AxisY:
			orientation = Orientation::Y_UP;
			break;
		case ofbx::UpVector_AxisZ:
			orientation = Orientation::Z_UP;
			break;
		}
		auto root = scene->getRoot();
		if (root != nullptr && model != nullptr)
		{
			int32_t i = 0;
			auto currentScene = Engine::Get().GetModule<SceneManager>()->GetCurrentScene();
			Entity parent = model->GetEntity();

			while (ofbx::Object* child = root->resolveObjectLink(i++))
			{
				if (child->getType() == ofbx::Object::Type::LIMB_NODE ||
					child->getType() == ofbx::Object::Type::NULL_NODE ||
					child->getType() == ofbx::Object::Type::MESH)
				{

					auto entity = currentScene->CreateEntity(child->name);
					auto& transform = entity.AddComponent<Transform>();
					SetTransform(child, transform);
					LoadTransforms(child, entity, bones);
					entity.SetParent(parent);
					if (child->getType() == ofbx::Object::Type::LIMB_NODE)
					{
						bones.emplace_back(child);
					}
				}
			}
		}
	}

	void FbxLoader_deprecated::LoadBones(const ofbx::Mesh* fbxMesh, Model_deprecated* model)
	{
		if (skeleton == nullptr)
		{
#if 0
			skeleton = SkeletonCache::Get().Save(file, {});
			//create a skeleton in cache
			//just keep the skeleton structure
			//in every skinnedMeshRenderer, it will be copied.
#endif
		}

		bones.clear();
		auto currentScene = Engine::Get().GetModule<SceneManager>()->GetCurrentScene();
		auto skin = fbxMesh->getGeometry()->getSkin();

		Entity rootEntity = model == nullptr ? Entity{} : model->GetEntity();
			

		for (int32_t clusterIndex = 0; clusterIndex < skin->getClusterCount(); clusterIndex++)
		{
			const ofbx::Cluster* cluster = skin->getCluster(clusterIndex);

			if (cluster->getIndicesCount() == 0)
				continue;
			const auto link = cluster->getLink();
			auto boneIndex = skeleton->GetBoneIndex(link->name);//create a bone if missing
			if (boneIndex == -1)
			{
#if 0
				auto & bone = skeleton->AddBone(skeleton->IndexOf(link->getParent()->name));
				bone.name = link->name;
				bone.offsetMatrix = glm::inverse(GetOffsetMatrix(fbxMesh, link));
				if (rootEntity.Valid()) 
				{
					auto entity = rootEntity.FindByPath(GetBonePath(link));
					bone.localTransform = entity.TryGetComponent<Transform>();
					if (entity.Valid() && bone.localTransform)
					{
						bone.localTransform->SetOffsetTransform(bone.offsetMatrix);
					}
				}
				bones.emplace_back(link);
#endif
			}
		}
	}


	static void LoadBonesRec(const ofbx::Object* obj, std::vector<const ofbx::Object*>& bones)
	{
		int32_t i = 0;
		while (const ofbx::Object* child = obj->resolveObjectLink(i++))
		{
			if (child->getType() == ofbx::Object::Type::LIMB_NODE)
			{
				bones.emplace_back(child);
				LoadBonesRec(child, bones);
			}
		}
	}

	void FbxLoader_deprecated::LoadBones()
	{
		int32_t i = 0;
		while (const ofbx::Object* child = scene->getRoot()->resolveObjectLink(i++))
		{
			if (child->getType() == ofbx::Object::Type::LIMB_NODE)
			{
				bones.emplace_back(child);
				LoadBonesRec(child, bones);
			}
		}
	}

	void FbxLoader_deprecated::LoadBones(const std::string& file)
	{

	}

	void FbxLoader_deprecated::LoadMeshes(Model_deprecated* model)
	{
		int32_t c = scene->getMeshCount();
	
		for (int32_t i = 0; i < c; ++i)
		{
			auto fbxMesh = scene->getMesh(i);
			auto geom = fbxMesh->getGeometry();
			auto numIndices = geom->getIndexCount();
			auto vertexCount = geom->getVertexCount();
			auto vertices = geom->getVertices();
			auto normals = geom->getNormals();
			auto tangents = geom->getTangents();
			auto colours = geom->getColors();
			auto uvs = geom->getUVs();
			const auto skin = geom->getSkin();
			const auto trianglesCount = vertexCount / 3;

			if (skin != nullptr) {
				LoadBones(fbxMesh, model);
			}

			const auto materials = geom->getMaterials();
			if (fbxMesh->getMaterialCount() < 2)
			{
				ImportMesh(model, fbxMesh, 0, trianglesCount - 1);
			}
			else
			{
				int32_t rangeStart = 0;
				int32_t rangeStartVal = materials[rangeStart];
				for (int32_t triangleIndex = 1; triangleIndex < trianglesCount; triangleIndex++)
				{
					if (rangeStartVal != materials[triangleIndex])
					{
						ImportMesh(model, fbxMesh, rangeStart, triangleIndex - 1);
						// Start a new range
						rangeStart = triangleIndex;
						rangeStartVal = materials[triangleIndex];
					}
				}
				
				ImportMesh(model, fbxMesh, rangeStart, trianglesCount - 1);
			}
		}
	}
	
	void FbxLoader_deprecated::ImportMesh(Model_deprecated* model, const ofbx::Mesh* fbxMesh, int32_t triangleStart, int32_t triangleEnd)
	{
		const int32_t firstVertexOffset = triangleStart * 3;
		const int32_t lastVertexOffset = triangleEnd * 3;

		auto geom = fbxMesh->getGeometry();
		auto numIndices = geom->getIndexCount();
		auto vertexCount = geom->getVertexCount();
		auto vertices = geom->getVertices();
		auto normals = geom->getNormals();
		auto uvs = geom->getUVs();
		vertexCount = lastVertexOffset - firstVertexOffset + 3;
		auto meshSection = Ptr<SkinnedMeshSection>(new SkinnedMeshSection());
		//meshSection->SetName(fbxMesh->name);
		meshSection->positions.resize(vertexCount);
		meshSection->normals.resize(vertexCount);
		meshSection->texCoords.resize(vertexCount);
		meshSection->indices.resize(vertexCount);
		meshSection->blendIndices.resize(vertexCount);
		meshSection->blendWeights.resize(vertexCount);
		for (auto i = 0; i < vertexCount; ++i)
		{
			ofbx::Vec3 cp = vertices[i + firstVertexOffset];

			meshSection->positions[i] = glm::vec3(float(cp.x), float(cp.y), float(cp.z));
			meshSection->bounds.Add(meshSection->positions[i]);

			if (normals)
				meshSection->normals[i] = { float(normals[i + firstVertexOffset].x), float(normals[i + firstVertexOffset].y), float(normals[i + firstVertexOffset].z) };
			if (uvs)
				meshSection->texCoords[i] = { float(uvs[i + firstVertexOffset].x), 1.0f - float(uvs[i + firstVertexOffset].y) };

		}

		for (int i = 0; i < vertexCount; i++)
		{
			meshSection->indices[i] = i;
		}

		if (geom->getSkin() != nullptr)
		{
			LoadWeight(geom->getSkin(),firstVertexOffset, meshSection.get());
		}
		
#if 0
		auto ret = std::static_pointer_cast<Mesh>(
			Engine::Get().GetModule<ResourceManager>()->AddResource(file, mesh)
		);
#endif


		if (model != nullptr)  // bind Components to model when model is not null
		{
			model->AddMesh(meshSection);
			auto currentScene = Engine::Get().GetModule<SceneManager>()->GetCurrentScene();
			Entity rootEntity = model->GetEntity();

#if 0
			auto ent = rootEntity.GetChildInChildren(fbxMesh->name);
			if (ent.Valid())
			{
				if (geom->getSkin() != nullptr)
				{
					auto& sk = ent.AddComponent<SkinnedMeshRenderer>();
					sk.mesh = ret;
					sk.meshIndex = model->GetMeshes().size() - 1;
					sk.skeleton = *skeleton;//copy a skeleton
				}
				else
				{
					auto& ren = ent.AddComponent<MeshRenderer>();
					ren.mesh = ret;
					ren.meshIndex = model->GetMeshes().size() - 1;
				}
			}
#endif

		}

	}



	void FbxLoader_deprecated::LoadWeight(const ofbx::Skin* skin, int32_t firstVertexOffset, SkinnedMeshSection* mesh)
	{
		if (skeleton != nullptr) 
		{
			for (int32_t clusterIndex = 0; clusterIndex < skin->getClusterCount(); clusterIndex++)
			{
				const ofbx::Cluster* cluster = skin->getCluster(clusterIndex);
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
					int vtxIndex = clusterIndices[j] - firstVertexOffset;
					float vtxWeight = (float)clusterWeights[j];

					if (vtxWeight <= 0 || vtxIndex < 0 || vtxIndex >= mesh->positions.size())
						continue;

					auto& indices = mesh->blendIndices[vtxIndex];
					auto& weights = mesh->blendWeights[vtxIndex];

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
	}

	

	void FbxLoader_deprecated::LoadAnimation(Model_deprecated* model)
	{
		float frameRate = scene->getSceneFrameRate();
		if (frameRate <= 0)
				frameRate = 30.0f;
		animation = std::make_shared<Animation>();

		if (bones.empty()) 
		{
			LoadBones();
		}

		const int animCount = scene->getAnimationStackCount();
		for (int32_t animIndex = 0; animIndex < animCount; animIndex++)
		{
			auto clip = ImportAnimation(model, animIndex, frameRate);
			if (clip != nullptr) {
				animation->AddClip(clip);
			}
		}
	}

	void FbxLoader_deprecated::LoadAnimation(const std::string& path, Model_deprecated* model)
	{
		std::string err;
		std::string name = StringUtils::GetFileName(path);
		std::string ext = StringUtils::GetExtension(name);
		int64_t size = 0;
		auto data = FileSystem::ReadFile(path, size);

		if (data == nullptr)
		{
			LOGW("Failed to load fbx file"); return;
		}
		const bool ignoreGeometry = false;
		const uint64_t flags = ignoreGeometry ? (uint64_t)ofbx::LoadFlags::IGNORE_GEOMETRY : (uint64_t)ofbx::LoadFlags::TRIANGULATE;
		scene = ofbx::load(data.get(), uint32_t(size), flags);
		LoadAnimation(model);
	}

	static void GetCurveData(AnimationCurveProperty& curve, const ofbx::AnimationCurve* node)
	{
		if (node != nullptr) 
		{
			auto times = node->getKeyTime();
			for (auto i = 0;i<node->getKeyCount();i++)
			{
				curve.curve.AddKey(ofbx::fbxTimeToSeconds(times[i]), node->getKeyValue()[i], 1, 1);
			}
		}
	}


	std::shared_ptr<AnimationClip> FbxLoader_deprecated::ImportAnimation(Model_deprecated * model,int32_t index,float frameRate)
	{
		const ofbx::AnimationStack* stack = scene->getAnimationStack(index);
		const ofbx::AnimationLayer* layer = stack->getLayer(0);
		const ofbx::TakeInfo* takeInfo = scene->getTakeInfo(stack->name);
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
		for (int32_t i = 0; i < bones.size(); i++)
		{
			const ofbx::AnimationCurveNode* translationNode = layer->getCurveNode(*bones[i], "Lcl Translation");
			const ofbx::AnimationCurveNode* rotationNode = layer->getCurveNode(*bones[i], "Lcl Rotation");
			
			if ( rotationNode) 
			{
				auto& curve0 = clip->curves.emplace_back();
				//curve0.path = GetBonePath(bones[i]);
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

}; // namespace Raven