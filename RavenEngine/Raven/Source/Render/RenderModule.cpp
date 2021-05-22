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
#include "RenderModule.h"


#include "Engine.h"
#include "Window/Window.h"

#include "ResourceManager/ResourceManager.h"
#include "ResourceManager/Resources/MaterialShader.h"
#include "ResourceManager/Resources/Material.h"
#include "ResourceManager/Resources/Texture2D.h"


#include "RenderDebug.h"
#include "RenderTarget.h"
#include "RenderPipeline.h"
#include "RenderTexFilter.h"
#include "Render/RenderResource/Shader/RenderRscShader.h"
#include "Render/RenderResource/Shader/UniformBuffer.h"
#include "Render/RenderResource/RenderRscTexture.h"
#include "RenderObjects/RenderScene.h"
#include "RenderObjects/RenderPass.h"
#include "RenderObjects/RenderScreen.h"
#include "RenderObjects/RenderSphere.h"


#include "OpenGL/GLContext.h"
#include "OpenGL/GLShader.h"
#include "OpenGL/GLFrameBuffer.h"
#include "OpenGL/GLVertexArray.h"
#include "OpenGL/GLRenderBuffer.h"
#include "OpenGL/GLTexture.h"



#include "Scene/Scene.h"
#include "Scene/SceneGraph.h"
#include "Scene/Component/Component.h"
#include "Scene/Component/MeshComponent.h"
#include "Scene/Component/SkinnedMeshComponent.h"
#include "Scene/Component/Transform.h"



#include "entt/entt.hpp"
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/gtx/transform.hpp"


// ~ITERATION_0----------------------------------------------------------------------------
#include "ResourceManager/ResourceManager.h"
// ~ITERATION_0----------------------------------------------------------------------------







namespace Raven {








RenderModule::RenderModule()
	: isRendering(false)
	, isRTToWindow(true)
	, isUpdateSky(true)
{
	rdebug.reset(new RenderDebug());

}


RenderModule::~RenderModule()
{

}



void RenderModule::Initialize()
{
	// Load OpenGL Extensions.
	auto result = glewInit();
	RAVEN_ASSERT(result == 0, "Render - Faield to initialize glew.");

	// OpenGL Context...
	context = std::make_unique<GLContext>();
	context->SetGLFWContext( Engine::GetModule<Window>()->GetNativeWindow() );

	// We are only using one context, so make it current.
	context->MakeCurrent(); 

	// Setup Render Debug.
	rdebug->Setup();


	// ...
	Ptr<RenderScreen> rscreen = Ptr<RenderScreen>(RenderScreen::Create());
	Ptr<RenderSphere> rsphere = Ptr<RenderSphere>(RenderSphere::Create());

	rfilter = Ptr<RenderTexFilter>(new RenderTexFilter(rscreen, rsphere));
	rfilter->Initialize();

	rpipeline = Ptr<RenderPipeline>(new RenderPipeline(rscreen, rsphere));
	rpipeline->rtFilter = rfilter.get();
	rpipeline->Initialize();


	CreateDefaultMaterials();


	// Main Scene Render Target.
	rtScene = std::make_shared<RenderTarget>(EGLTexture::Texture2D, EGLFormat::RGBA);
	rtScene->Resize(glm::ivec2(1920, 1080));
	rtScene->SetClearColor(glm::vec4(0.1f, 0.1f, 0.2f, 1.0f));
	rtScene->SetSampleProeprties(EGLFilter::Nearest, EGLWrap::ClampToEdge);
	rtScene->Update();

	// Render Scene.
	rscene = std::make_shared<RenderScene>();
	rscene->Setup();
	rscene->SetDebugPrimitives( &rdebug->GetRenderPrimitive() );


	// Generate BRDF Lookup table...
	BRDF = Ptr<RenderRscTexture>(new RenderRscTexture());
	rfilter->GenBRDFLUT(BRDF.get() );
	rpipeline->BRDF = BRDF->GetTexture().get();

	// Default Environment Map.
	Engine::GetModule<ResourceManager>()->AddResource(".\\assets\\textures\\T_Default_Environment_Map.raven");
	Ptr<Texture2D> envTexture = Engine::GetModule<ResourceManager>()->GetResource<Texture2D>("./assets/textures/T_Default_Environment_Map.raven");

	GenerateEnvMap(envTexture);
	rpipeline->EnvMap = environmentMap->GetTexture().get();


}


void RenderModule::Destroy()
{
	// Destroy Debug Render.
	rdebug->Destroy();
	
	// Destroy Main Render Target.
	rtScene.reset();

	// Destroy pipeline
	rpipeline->Destroy();

	// Destroy render filter.
	rfilter->Destroy();
}


void RenderModule::Update(float dt)
{
	rdebug->Update(dt);

}


void RenderModule::BeginRender(Scene* scene, const glm::ivec2& extent)
{
	RAVEN_ASSERT(!isRendering, "Render - Trying to begin render before EndRender().");
	isRendering = true;

	// Should we resize with window?
	if (isRTToWindow)
	{
		// Resize our render target, it is fine to call it every frame as it only resize the actuall image when it get larger
		rtScene->Resize(extent);
	}

	// Update Render Target if dirty.
	if (rtScene->IsDirty())
		rtScene->Update();


	// ~TESTING-------------------------------------------------------
#if CAPTURE_SHOT != 0
	float time = Engine::Get().GetEngineTime();
	glm::mat4 view;
	glm::mat4 proj;

#if CAPTURE_SHOT == 1
	if (time > 10.0)
	{
		time -= 10.0f;
		view = glm::lookAt(glm::vec3(time * 20.0, 50.0f, time * 20.0), glm::vec3(512.0f, 0.0f, 512.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		proj = glm::perspective(glm::radians(45.0f), rtScene->GetAspectRatio(), 1.0f, 3200000.0f);
	}
#endif

	rscene->SetView(view);
	rscene->SetProjection(proj, glm::radians(45.0f), rtScene->GetAspectRatio(), 1.0, 3200000.0f);

#endif

	// ~TESTING-------------------------------------------------------

	// Build Render Data form the scene...
	rscene->Build(scene);
}


void RenderModule::Render()
{
	RAVEN_ASSERT(isRendering, "Render - Trying to render before BeginRender().");

	// Engine Time.
	float time = Engine::Get().GetEngineTime();

	// Begin The Pipeline for rendering a scene.
	rpipeline->Begin(rtScene.get(), rscene.get(), time);

	// Does sky needs to be updated?
	if (isUpdateSky)
	{
		rpipeline->RenderEnvSky();
		isUpdateSky = false;
	}

	// Rendering...
	rpipeline->Render();

	// End The Pipeline Render.
	rpipeline->End();


	if (isRTToWindow)
	{
		rtScene->GetFBO()->Blit(
			nullptr,													// Default Framebuffer.
			EGLBufferMask::Color,							// Color Mask Bit. 
			EGLFilter::Nearest,								// Filter.
			FBBlitViewport(0, 0, rtScene->GetSize().x, rtScene->GetSize().y),		// Src Viewport.
			FBBlitViewport(0, 0, rtScene->GetSize().x, rtScene->GetSize().y)		// Dst Viewport.
		);
	}

	// Unbind FBO...
	rtScene->GetFBO()->Unbind(EGLFrameBuffer::Framebuffer);

}


void RenderModule::EndRender()
{
	isRendering = false;

	// Cleanup Render Data...
	rscene->Clear();

}


RenderSurface RenderModule::GetRequiredRenderSurface()
{
	return GLContext::GetSurface();
}


void RenderModule::CreateDefaultMaterials()
{
	// Checker Texture...
	Engine::GetModule<ResourceManager>()->AddResource("./assets/textures/T_Checker.raven");
	Ptr<Texture2D> checkerTexture = Engine::GetModule<ResourceManager>()->GetResource<Texture2D>("./assets/textures/T_Checker.raven");


	// Default Mesh Material...
	{
		Ptr<MaterialShader> matShader( new MaterialShader() );
		matShader->SetName("Default_Mesh");
		matShader->SetDomain(ERenderShaderDomain::Mesh);
		matShader->SetShaderType(ERenderShaderType::Opaque);
		matShader->SetMaterialFunction("shaders/Materials/DefaultMaterial.glsl");
		matShader->AddSampler("inCheckerTexture", ESInputDefaultFlag::White);
		matShader->SetShadowShader(true);
		matShader->LoadRenderResource();

		Ptr<Material> mat(new Material());
		mat->SetMaterialShader(matShader);
		mat->SetTexture("inCheckerTexture", checkerTexture);
		mat->LoadRenderResource();
		
		defaultMaterials.mesh = mat;
	}


	// Default Skinned Mesh Material...
	{
		Ptr<MaterialShader> matShader(new MaterialShader());
		matShader->SetName("Default_Skinned");
		matShader->SetDomain(ERenderShaderDomain::Skinned);
		matShader->SetShaderType(ERenderShaderType::Opaque);
		matShader->SetMaterialFunction("shaders/Materials/DefaultMaterial.glsl");
		matShader->AddSampler("inCheckerTexture", ESInputDefaultFlag::White);
		matShader->SetShadowShader(true);
		matShader->LoadRenderResource();

		Ptr<Material> mat(new Material());
		mat->SetMaterialShader(matShader);
		mat->SetTexture("inCheckerTexture", checkerTexture);
		mat->LoadRenderResource();

		defaultMaterials.skinned = mat;
	}


	// Default Terrain Material...
	{
		Ptr<MaterialShader> matShader(new MaterialShader());
		matShader->SetName("Default_Terrain");
		matShader->SetDomain(ERenderShaderDomain::Terrain);
		matShader->SetShaderType(ERenderShaderType::Opaque);
		matShader->SetMaterialFunction("shaders/Materials/DefaultMaterial.glsl");
		matShader->AddSampler("inCheckerTexture", ESInputDefaultFlag::White);
		matShader->SetShadowShader(true);
		matShader->SetTwoSided(true);
		matShader->LoadRenderResource();

		Ptr<Material> mat(new Material());
		mat->SetMaterialShader(matShader);
		mat->SetTexture("inCheckerTexture", checkerTexture);
		mat->LoadRenderResource();

		defaultMaterials.terrain = mat;
	}


	// Default MeshInstance Material...
	{
		Ptr<MaterialShader> matShader(new MaterialShader());
		matShader->SetName("Default_Mesh");
		matShader->SetDomain(ERenderShaderDomain::MeshInstance);
		matShader->SetShaderType(ERenderShaderType::Opaque);
		matShader->SetMaterialFunction("shaders/Materials/DefaultMaterial.glsl");
		matShader->AddSampler("inCheckerTexture", ESInputDefaultFlag::White);
		matShader->SetShadowShader(true);
		matShader->LoadRenderResource();

		Ptr<Material> mat(new Material());
		mat->SetMaterialShader(matShader);
		mat->SetTexture("inCheckerTexture", checkerTexture);
		mat->LoadRenderResource();

		defaultMaterials.meshInstance = mat;
	}
}


void RenderModule::GenerateEnvMap(Ptr<Texture2D> texture)
{
	Ptr<RenderRscTexture> envCubeMap = Ptr<RenderRscTexture>(new RenderRscTexture());
	rfilter->GenCubeMap(texture->GetRenderRsc(), envCubeMap.get(), true);

	environmentMap = Ptr<RenderRscTexture>(new RenderRscTexture());
	rfilter->FilterSpecularIBL(envCubeMap.get(), environmentMap.get());
}


void RenderModule::RequestUpdateSky()
{
	isUpdateSky = true;
}





} // End of namespace Raven.

