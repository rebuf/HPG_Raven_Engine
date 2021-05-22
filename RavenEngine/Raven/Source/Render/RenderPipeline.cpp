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
#include "RenderPipeline.h"
#include "RenderTexFilter.h"

#include "RenderTarget.h"
#include "RenderObjects/RenderScene.h"
#include "RenderObjects/RenderLight.h"
#include "RenderObjects/RenderScreen.h"
#include "RenderObjects/RenderSphere.h"
#include "RenderObjects/RenderPass.h"
#include "RenderObjects/RenderGrid.h"
#include "RenderObjects/RenderShadow.h"
#include "RenderResource/Shader/UniformBuffer.h"
#include "RenderResource/Shader/RenderRscShader.h"

#include "OpenGL/GLTexture.h"
#include "OpenGL/GLShader.h"
#include "OpenGL/GLFrameBuffer.h"

#include "Engine.h"

#include "GL/glew.h"


#include <random>




namespace Raven {



RenderPipeline::RenderPipeline(Ptr<RenderScreen> screen, Ptr<RenderSphere> sphere)
	: rtarget(nullptr)
	, rscene(nullptr)
	, rscreen(screen)
	, rsphere(sphere)
	, EnvMap(nullptr)
	, BRDF(nullptr)
{

}


RenderPipeline::~RenderPipeline()
{

}


void RenderPipeline::Initialize()
{
	// Targets initial size.
	size = glm::ivec2(1920, 1080);

	// Setup...
	SetupRenderPasses();
	SetupShaders();
	SetupSSAO();
	SetupSky();


	// Uniforms...
	uniforms.common = Ptr<UniformBuffer>( UniformBuffer::Create(RenderShaderInput::CommonBlock, true) );
	uniforms.light_DEFERRED = Ptr<UniformBuffer>( UniformBuffer::Create(RenderShaderInput::LightingBlock_DEFERRED, true) );
	uniforms.light_FORWARD = Ptr<UniformBuffer>(UniformBuffer::Create(RenderShaderInput::LightingBlock_FORWARD, true));
	uniforms.shadow = Ptr<UniformBuffer>( UniformBuffer::Create(RenderShaderInput::ShadowBlock, true) );
	uniforms.lightShadow = Ptr<UniformBuffer>( UniformBuffer::Create(RenderShaderInput::LightShadowBlock, true) );


	// Grid.
	rgrid = Ptr<RenderGrid>( RenderGrid::Create() );

}


void RenderPipeline::Destroy()
{
	uniforms.common.reset();
	uniforms.light_DEFERRED.reset();
	uniforms.light_FORWARD.reset();
	uniforms.shadow.reset();
	uniforms.lightShadow.reset();


}


void RenderPipeline::Begin(RenderTarget* target, RenderScene* scene, float time)
{
	RAVEN_ASSERT(rtarget == nullptr && rscene == nullptr, "Can't Begin a new pipeline render while the previous one is not done.");
	rtarget = target;
	rscene = scene;

	// The Viewport
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.z = (float)rtarget->GetSize().x;
	viewport.w = (float)rtarget->GetSize().y;

	// Render Target Clear Color
	glm::vec4 clearColor = rtarget->GetClearColor();
	clearColor.a = 0.0f;
	gbufferPass->SetClearColor(0, clearColor);


	// Check render passe target size, and resize it if needed.
	if (size.x < viewport.z || size.y < viewport.w)
	{
		glm::ivec2 newSize(viewport.z, viewport.w);
		Resize(newSize);
	}


	// Update Common Uniform Buffer...
	uniforms.common->SetDataValue(0, rscene->GetViewProjection());
	uniforms.common->SetDataValue(1, rscene->GetViewProjectionInverse());
	uniforms.common->SetDataValue(2, rscene->GetViewDir());
	uniforms.common->SetDataValue(3, rscene->GetViewPos());
	uniforms.common->SetDataValue(4, viewport);
	uniforms.common->SetDataValue(5, glm::vec4(rscene->GetEnvironment().sunDir, 0.0f));
	uniforms.common->SetDataValue(6, glm::vec4(rscene->GetEnvironment().sunColor, rscene->GetEnvironment().sunPower));
	uniforms.common->SetDataValue(7, glm::vec2(rscene->GetNear(), rscene->GetFar()));
	uniforms.common->SetDataValue(8, time);
	uniforms.common->Update();
	uniforms.common->BindBase();


	// Set acutal render target scale.
	rscreen->SetRTSize(size);

}


void RenderPipeline::End()
{
	RAVEN_ASSERT(rtarget != nullptr && rscene != nullptr, "Can't End a pipeline render without starting one.");

	// End Passes...
	RenderPass::End();

	rtarget = nullptr;
	rscene = nullptr;
}


void RenderPipeline::Render()
{
	RAVEN_ASSERT(rtarget != nullptr && rscene != nullptr, "Can't render a pipeline without starting one.");

	// Global OpenGL States...
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_BLEND);

	// --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- 
	// Shadow: Draw Shadow Maps.
	if (rscene->GetEnvironment().isSun)
	{
		glEnable(GL_CULL_FACE);
		rscene->DrawShadow(uniforms.shadow.get());
	}



	// --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- 
	// Deferred: G-Buffer Pass
	{
		gbufferPass->Begin(viewport, true);

		// OpenGL States...
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		// Draw deferred pass.
		rscene->DrawDeferred();

		// Draw debug
#if RAVEN_DEBUG
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDisable(GL_CULL_FACE);
			rscene->DrawDebug();
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
#endif
	}



	// --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- 
	// SSAO Pass
	{
		glDisable(GL_BLEND);

		ssaoPass->Begin(viewport, false);
		ssaoShader->GetShader()->Use();

		gbufferPass->GetTexture(1)->Active(0); // Normal.
		gbufferPass->GetDepthTexture()->Active(1); // Depth.
		ssaoNoiseTexture->Active(2); // Noise.
		rscreen->Draw(ssaoShader->GetShader());

		ssaoBlurPass->Begin(viewport, false);
		ssaoBlurShader->GetShader()->Use();
		ssaoPass->GetTexture(0)->Active(0); // SSAO.

		rscreen->Draw(ssaoBlurShader->GetShader());
	}


	// --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- 
	// Deferred: Lighting Pass
	{
		lightingPass->Begin(viewport, false);

		GLShader* shader = lightingShader->GetShader();
		shader->Use();

		// OpenGL States...
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glEnable(GL_BLEND);

		// Additive Blending - Add lighting
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);


		// ...
		UpdateLights_DEFERRED();
		UpdateShadows();
		uniforms.light_DEFERRED->BindBase();
		uniforms.lightShadow->BindBase();

		
		gbufferPass->GetTexture(0)->Active(0); // Albedo + Specular.
		gbufferPass->GetTexture(1)->Active(1); // Normal.
		gbufferPass->GetTexture(2)->Active(2); // BRDF.
		gbufferPass->GetDepthTexture()->Active(3); // Depth.
		hdrTarget[1]->Active(4);

		// IBL...
		if (rscene->GetEnvironment().isSky)
		{
			skyEnv->Active(5);
		}
		else
		{
			EnvMap->Active(5);
		}
		BRDF->Active(6);


		for (int32_t i = 0; i < RENDER_MAX_SHADOW_CASCADE; ++i)
			rscene->GetEnvironment().sunShadow->GetCascade(i).shadowMap->Active(7+i);

		rscreen->Draw(shader);
	}



	// --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- 
	// Forward Pass
	{
		forwardPass->Begin(viewport, false);


		// ------------------
		// Draw Sky...
		if (rscene->GetEnvironment().isSky)
		{
			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);
			glDisable(GL_BLEND);
			GLShader* skys = skyShader->GetShader();
			skys->Use();
			rsphere->DrawSky(skys);
		}


		// OpenGL States...
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glDepthMask(GL_FALSE);
		glEnable(GL_BLEND);



		// ------------------
		// Blending 
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Draw Grid...
		if (rscene->IsGrid())
		{
			rgrid->Draw();
		}


		uniforms.lightShadow->BindBase();
		uniforms.light_FORWARD->BindBase();

		// IBL...
		if (rscene->GetEnvironment().isSky)
		{
			skyEnv->Active(0);
		}
		else
		{
			EnvMap->Active(0);
		}

		BRDF->Active(1);

		for (int32_t i = 0; i < RENDER_MAX_SHADOW_CASCADE; ++i)
			rscene->GetEnvironment().sunShadow->GetCascade(i).shadowMap->Active(2 + i);

		// Draw translucent scene...
		rscene->DrawTranslucent(uniforms.light_FORWARD.get());

		// Reset OpenGL States.
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);
	}






	// --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- 
	// Final Post-Processing.
	DoPostProcessFinal(0);

}


void RenderPipeline::DoPostProcessFinal(int32_t hdrTargetIndex)
{
	// Final Post-Processing Pass...
	finalPostProcessPass->Begin(viewport, false);
	finalPostProcessShader->GetShader()->Use();

	hdrTarget[hdrTargetIndex]->Active(0);

	// ~Testig---------------------------
	finalPostProcessShader->GetShader()->SetUniform("temp", 1);
	ssaoPass->GetTexture(0)->Active(1);
	//rscene->GetEnvironment().sunShadow->GetCascade(0).shadowMap->Active(1);
	// ~Testig---------------------------


	rscreen->Draw(finalPostProcessShader->GetShader());


	// FXAA Pass...
	rtarget->GetFBO()->Bind(EGLFrameBuffer::Framebuffer);
	glViewport(viewport.x, viewport.y, viewport.z, viewport.w);
	glScissor(viewport.x, viewport.y, viewport.z, viewport.w);

	fxaaShader->GetShader()->Use();
	finalPostProcessPass->GetTexture(0)->Active(0);
	rscreen->Draw(fxaaShader->GetShader());
}


void RenderPipeline::Resize(const glm::ivec2& newSize)
{
	size = newSize;

	gbufferPass->ResizeTargets(size);
	forwardPass->ResizeTargets(size);
	lightingPass->ResizeTargets(size);
	finalPostProcessPass->ResizeTargets(size);
	ssaoPass->ResizeTargets(size);
	ssaoBlurPass->ResizeTargets(size);

	// Update unfiroms that need the texture size.
	fxaaShader->GetShader()->Use();
	fxaaShader->GetShader()->SetUniform("fxaaQualityRcpFrame", glm::vec2(1.0f/(float)size.x, 1.0f / (float)size.y));
}


void RenderPipeline::SetupRenderPasses()
{

	// HDR Targets...
	hdrTarget[0] = Ptr<GLTexture>(GLTexture::Create2D(
		EGLFormat::RGB16F,
		size.x, size.y,
		EGLFilter::Nearest,
		EGLWrap::ClampToEdge
	));


	hdrTarget[1] = Ptr<GLTexture>(GLTexture::Create2D(
		EGLFormat::RGB16F,
		size.x, size.y,
		EGLFilter::Nearest,
		EGLWrap::ClampToEdge
	));



	// --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- 
	// G-Buffer Render Pass
	{
		// Albedo(RGB) + Materail Type Index.
		Ptr<GLTexture> target0Albedo = Ptr<GLTexture>(GLTexture::Create2D(
			EGLFormat::RGBA,
			size.x, size.y,
			EGLFilter::Nearest,
			EGLWrap::ClampToEdge
		));

		// Normal
		Ptr<GLTexture> target1Normal = Ptr<GLTexture>(GLTexture::Create2D(
			EGLFormat::RGB16F,
			size.x, size.y,
			EGLFilter::Nearest,
			EGLWrap::ClampToEdge
		));

		// BRDF: Roughness(R) + Metallic(G) + Specular(B)
		Ptr<GLTexture> target2BRDF = Ptr<GLTexture>(GLTexture::Create2D(
			EGLFormat::RGBA,
			size.x, size.y,
			EGLFilter::Nearest,
			EGLWrap::ClampToEdge
		));

		// Depth
		Ptr<GLTexture> targetDepth = Ptr<GLTexture>(GLTexture::Create2D(
			EGLFormat::Depth32,
			size.x, size.y,
			EGLFilter::Nearest,
			EGLWrap::ClampToEdge
		));


		gbufferPass = Ptr<RenderPass>(new RenderPass());
		
		// Targets...
		gbufferPass->AddTexture(0, target0Albedo);
		gbufferPass->AddTexture(1, target1Normal);
		gbufferPass->AddTexture(2, target2BRDF);
		gbufferPass->AddTexture(3, hdrTarget[0]);
		gbufferPass->SetDepthTexture(targetDepth, false);
		gbufferPass->SetSize(size);
		gbufferPass->Build();
	}

	// LDR Target...
	Ptr<GLTexture> ldrTarget = Ptr<GLTexture>(GLTexture::Create2D(
		EGLFormat::RGBA,
		size.x, size.y,
		EGLFilter::Linear,
		EGLWrap::ClampToEdge
	));



	// --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- 
	// Lighting Render Pass
	{
		lightingPass = Ptr<RenderPass>(new RenderPass());
		lightingPass->AddTexture(0, hdrTarget[0]);
		lightingPass->SetSize(size);
		lightingPass->Build();
	}



	// --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- 
	// Forward Render Pass
	{
		forwardPass = Ptr<RenderPass>(new RenderPass());
		forwardPass->AddTexture(0, hdrTarget[0]);
		forwardPass->SetDepthTexture(gbufferPass->GetDepthTexture(), false);
		forwardPass->SetSize(size);
		forwardPass->Build();
	}


	// --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- 
	// SSAO Pass
	{
		// SSAO LDR Target...
		Ptr<GLTexture> ssaoTarget = Ptr<GLTexture>(GLTexture::Create2D(
			EGLFormat::R,
			size.x, size.y,
			EGLFilter::Nearest,
			EGLWrap::ClampToEdge
		));


		ssaoPass = Ptr<RenderPass>(new RenderPass());
		ssaoPass->AddTexture(0, ssaoTarget);
		ssaoPass->SetSize(size);
		ssaoPass->Build();


		ssaoBlurPass = Ptr<RenderPass>(new RenderPass());
		ssaoBlurPass->AddTexture(0, hdrTarget[1]);
		ssaoBlurPass->SetSize(size);
		ssaoBlurPass->Build();
	}


	// --- -- --- -- --- -- --- -- --- -- --- -- --- -- --- -- 
	// Final Post-Processsing Passe
	{
		finalPostProcessPass = Ptr<RenderPass>(new RenderPass());
		finalPostProcessPass->AddTexture(0, ldrTarget);
		finalPostProcessPass->SetSize(size);
		finalPostProcessPass->Build();
	}



	// ---- ---- ---- --- -- -- ---- 
	// Sky Update Reflection Pass.
	{
		glm::ivec2 reflectionSize(256, 256);

		// Sky CubeMap Target...
		skyCubeMap = Ptr<GLTexture>( GLTexture::Create(EGLTexture::CubeMap, EGLFormat::RGB16F) );
		skyCubeMap->Bind();
		for (int32_t i = 0; i < 6; ++i)
			skyCubeMap->UpdateTexData(0, reflectionSize.x, reflectionSize.y, i, nullptr);
		skyCubeMap->SetFilter(EGLFilter::Linear);
		skyCubeMap->SetWrap(EGLWrap::Mirror);
		skyCubeMap->UpdateTexParams();
		skyCubeMap->Unbind();


		skyCubePass = Ptr<RenderPass>(new RenderPass());
		skyCubePass->AddTexture(0, skyCubeMap, 0, -1);
		skyCubePass->SetSize(reflectionSize);
		skyCubePass->Build();

		// Sky Filtered Envionment...
		skyEnv = Ptr<GLTexture>(GLTexture::Create(EGLTexture::CubeMap, EGLFormat::RGB));
		skyEnv->Bind();
		for (int32_t i = 0; i < 6; ++i)
			skyEnv->UpdateTexData(0, reflectionSize.x, reflectionSize.y, i, nullptr);
		skyEnv->Bind();
		skyEnv->SetWrap(EGLWrap::ClampToEdge);
		skyEnv->SetFilter(EGLFilter::TriLinear);
		skyEnv->SetMipLevels(0, NUM_IBL_SPEC_MIPS);
		skyEnv->UpdateTexParams();
		skyEnv->GenerateMipmaps();
		skyEnv->Unbind();
	}


}


void RenderPipeline::SetupShaders()
{
	// Lighting Shader Pass...
	{
		RenderRscShaderDomainCreateData shaderDomainData;
		shaderDomainData.AddSource(EGLShaderStage::Vertex, "shaders/ScreenTriangleVert.glsl");
		shaderDomainData.AddSource(EGLShaderStage::Fragment, "shaders/DeferredLighting.glsl");
		shaderDomainData.AddImport(EGLShaderStageBit::FragmentBit, "shaders/CommonLight.glsl");
		shaderDomainData.AddImport(EGLShaderStageBit::FragmentBit, "shaders/CommonShadow.glsl");
		shaderDomainData.AddImport(EGLShaderStageBit::FragmentBit, "shaders/Lighting.glsl");
		shaderDomainData.AddPreprocessor("#define MAX_LIGHTS " + std::to_string(RENDER_PASS_DEFERRED_MAX_LIGHTS));
		shaderDomainData.AddPreprocessor("#define SCALE_UV_WITH_TARGET 1");
		shaderDomainData.AddPreprocessor("#define MAX_SHADOW_CASCADE " + std::to_string(RENDER_MAX_SHADOW_CASCADE));


		// Shader Type Data
		RenderRscShaderCreateData shaderData;
		shaderData.type = ERenderShaderType::Opaque;
		shaderData.name = "DeferredLighting_Shader";

		lightingShader = Ptr<RenderRscShader>( RenderRscShader::CreateCustom(shaderDomainData, shaderData) );
		lightingShader->GetInput().AddBlockInput(RenderShaderInput::CommonBlock);
		lightingShader->GetInput().AddBlockInput(RenderShaderInput::LightingBlock_DEFERRED);
		lightingShader->GetInput().AddBlockInput(RenderShaderInput::LightShadowBlock);
		lightingShader->GetInput().AddSamplerInput("inAlbedo");
		lightingShader->GetInput().AddSamplerInput("inNormal");
		lightingShader->GetInput().AddSamplerInput("inBRDF");
		lightingShader->GetInput().AddSamplerInput("inDepth");
		lightingShader->GetInput().AddSamplerInput("inAO");
		lightingShader->GetInput().AddSamplerInput("inSkyEnvironment");
		lightingShader->GetInput().AddSamplerInput("inEnvBRDF");

		// Add sun shadow samplers...
		for (int32_t i = 0; i < RENDER_MAX_SHADOW_CASCADE; ++i)
		{
			lightingShader->GetInput().AddSamplerInput("inSunShadow[" + std::to_string(i) + "]");
		}

		lightingShader->BindBlockInputs();
		lightingShader->BindSamplers();
	}



	// Final Post-Processing
	{
		RenderRscShaderDomainCreateData shaderDomainData;
		shaderDomainData.AddSource(EGLShaderStage::Vertex, "shaders/ScreenTriangleVert.glsl");
		shaderDomainData.AddSource(EGLShaderStage::Fragment, "shaders/PostProcessing/FinalPostProcessing.glsl");
		shaderDomainData.AddPreprocessor("#define SCALE_UV_WITH_TARGET 1");

		// Shader Type Data
		RenderRscShaderCreateData shaderData;
		shaderData.type = ERenderShaderType::PostProcessing;
		shaderData.name = "FinalPostProcessing_Shader";

		finalPostProcessShader = Ptr<RenderRscShader>(RenderRscShader::CreateCustom(shaderDomainData, shaderData));
		finalPostProcessShader->GetInput().AddBlockInput(RenderShaderInput::CommonBlock);
		finalPostProcessShader->GetInput().AddSamplerInput("inPPInput0");
		finalPostProcessShader->BindBlockInputs();
		finalPostProcessShader->BindSamplers();
	}


	// FXAA Shader
	{
		RenderRscShaderDomainCreateData shaderDomainData;
		shaderDomainData.AddSource(EGLShaderStage::Vertex, "shaders/ScreenTriangleVert.glsl");
		shaderDomainData.AddSource(EGLShaderStage::Fragment, "shaders/PostProcessing/FXAAPostProcessing.glsl");
		shaderDomainData.AddImport(EGLShaderStageBit::FragmentBit, "shaders/PostProcessing/FXAA_3_11.glsl");
		shaderDomainData.AddPreprocessor("#define SCALE_UV_WITH_TARGET 1");
		shaderDomainData.AddPreprocessor("#define FXAA_PC 1");
		shaderDomainData.AddPreprocessor("#define FXAA_GLSL_130 1");
		shaderDomainData.AddPreprocessor("#define FXAA_QUALITY__PRESET 12");

		// Shader Type Data
		RenderRscShaderCreateData shaderData;
		shaderData.type = ERenderShaderType::PostProcessing;
		shaderData.name = "FXAAPostProcessing_Shader";

		fxaaShader = Ptr<RenderRscShader>(RenderRscShader::CreateCustom(shaderDomainData, shaderData));
		fxaaShader->GetInput().AddBlockInput(RenderShaderInput::CommonBlock);
		fxaaShader->GetInput().AddSamplerInput("inPPInput");
		fxaaShader->BindBlockInputs();
		fxaaShader->BindSamplers();
		fxaaShader->GetShader()->SetUniform("fxaaQualityRcpFrame", glm::vec2(1.0f / (float)size.x, 1.0f / (float)size.y));
	}

}


void RenderPipeline::SetupSSAO()
{
	std::uniform_real_distribution<float> random(0.0, 1.0);
	std::default_random_engine generator;
	std::vector<glm::vec3> ssaoKernel;
	for (unsigned int i = 0; i < 64; ++i)
	{
		glm::vec3 sample(
			random(generator) * 2.0 - 1.0,
			random(generator) * 2.0 - 1.0,
			random(generator)
		);
		sample = glm::normalize(sample);
		sample *= random(generator);

		float scale = (float)i / 64.0;
		scale = glm::mix(0.1f, 1.0f, scale * scale);
		sample *= scale;
		ssaoKernel.push_back(sample);
	}


	std::vector<glm::vec3> ssaoNoise;
	for (unsigned int i = 0; i < 16; i++)
	{
		glm::vec3 noise(
			random(generator) * 2.0 - 1.0,
			random(generator) * 2.0 - 1.0,
			0.0f);
		ssaoNoise.push_back(noise);
	}

	
	ssaoNoiseTexture = Ptr<GLTexture>(GLTexture::Create2D(EGLFormat::RGB16F, 4, 4, ssaoNoise.data(), EGLFilter::Nearest, EGLWrap::Repeat));


	// -- --- --- ---- -----
	// SSAO Shader
	{
		RenderRscShaderDomainCreateData shaderDomainData;
		shaderDomainData.AddSource(EGLShaderStage::Vertex, "shaders/ScreenTriangleVert.glsl");
		shaderDomainData.AddSource(EGLShaderStage::Fragment, "shaders/PostProcessing/SSAO.glsl");
		shaderDomainData.AddPreprocessor("#define SCALE_UV_WITH_TARGET 1");
		shaderDomainData.AddPreprocessor("#define NUM_SAMPLES " + std::to_string(ssaoKernel.size()));

		// Shader Type Data
		RenderRscShaderCreateData shaderData;
		shaderData.type = ERenderShaderType::PostProcessing;
		shaderData.name = "SSAO_Shader";

		ssaoShader = Ptr<RenderRscShader>(RenderRscShader::CreateCustom(shaderDomainData, shaderData));
		ssaoShader->GetInput().AddBlockInput(RenderShaderInput::CommonBlock);
		ssaoShader->GetInput().AddSamplerInput("inNormal");
		ssaoShader->GetInput().AddSamplerInput("inDepth");
		ssaoShader->GetInput().AddSamplerInput("inNoise");
		ssaoShader->BindBlockInputs();
		ssaoShader->BindSamplers();

		for (uint32_t i = 0; i < ssaoKernel.size(); ++i)
		{
			ssaoShader->GetShader()->SetUniform("inSamples[" + std::to_string(i) + "]", ssaoKernel[i]);
		}
	}


	// -- --- --- ---- -----
	// SSAO Blur
	{
		RenderRscShaderDomainCreateData shaderDomainData;
		shaderDomainData.AddSource(EGLShaderStage::Vertex, "shaders/ScreenTriangleVert.glsl");
		shaderDomainData.AddSource(EGLShaderStage::Fragment, "shaders/PostProcessing/SSAOBlur.glsl");
		shaderDomainData.AddPreprocessor("#define SCALE_UV_WITH_TARGET 1");

		// Shader Type Data
		RenderRscShaderCreateData shaderData;
		shaderData.type = ERenderShaderType::PostProcessing;
		shaderData.name = "SSAOBlur_Shader";

		ssaoBlurShader = Ptr<RenderRscShader>(RenderRscShader::CreateCustom(shaderDomainData, shaderData));
		ssaoBlurShader->GetInput().AddBlockInput(RenderShaderInput::CommonBlock);
		ssaoBlurShader->GetInput().AddSamplerInput("inSSAO");
		ssaoBlurShader->BindBlockInputs();
		ssaoBlurShader->BindSamplers();
	}
}


void RenderPipeline::SetupSky()
{

	// -- --- --- ---- -----
	// Sky Shader.
	{
		RenderRscShaderDomainCreateData shaderDomainData;
		shaderDomainData.AddSource(EGLShaderStage::Vertex, "shaders/SphereVert.glsl");
		shaderDomainData.AddSource(EGLShaderStage::Fragment, "shaders/SkyFrag.glsl");
		shaderDomainData.AddPreprocessor("#define DRAW_SKY 1");

		// Shader Type Data
		RenderRscShaderCreateData shaderData;
		shaderData.type = ERenderShaderType::PostProcessing;
		shaderData.name = "Sky_Shader";

		skyShader = Ptr<RenderRscShader>(RenderRscShader::CreateCustom(shaderDomainData, shaderData));
		skyShader->GetInput().AddBlockInput(RenderShaderInput::CommonBlock);
		skyShader->BindBlockInputs();
		skyShader->BindSamplers();
	}

	// -- --- --- ---- -----
  // Sky CubeMap Shader.
	{
		RenderRscShaderDomainCreateData shaderDomainData;
		shaderDomainData.AddSource(EGLShaderStage::Vertex, "shaders/SphereVert.glsl");
		shaderDomainData.AddSource(EGLShaderStage::Geometry, "shaders/SphereGeom.glsl");
		shaderDomainData.AddSource(EGLShaderStage::Fragment, "shaders/SkyFrag.glsl");
		shaderDomainData.AddPreprocessor("#define DRAW_SKY_CUBE_MAP 1");

		// Shader Type Data
		RenderRscShaderCreateData shaderData;
		shaderData.type = ERenderShaderType::PostProcessing;
		shaderData.name = "skyCubeShader_Shader";

		skyCubeShader = Ptr<RenderRscShader>(RenderRscShader::CreateCustom(shaderDomainData, shaderData));
		skyCubeShader->GetInput().AddBlockInput(RenderShaderInput::CommonBlock);
		skyCubeShader->BindBlockInputs();
		skyCubeShader->BindSamplers();

		rtFilter->SetupSkyShader(skyCubeShader.get());
	}


}


void RenderPipeline::UpdateLights_DEFERRED()
{
	static std::vector<glm::vec4> lightPos(RENDER_PASS_DEFERRED_MAX_LIGHTS);
	static std::vector<glm::vec4> lightDir(RENDER_PASS_DEFERRED_MAX_LIGHTS);
	static std::vector<glm::vec4> lightPower(RENDER_PASS_DEFERRED_MAX_LIGHTS);
	static std::vector<glm::vec4> lightData(RENDER_PASS_DEFERRED_MAX_LIGHTS);


	for (int32_t i = 0; i < RENDER_PASS_DEFERRED_MAX_LIGHTS; ++i)
	{
		if (i >= rscene->GetLights().size())
		{
			lightData[i].r = 0.0f; // No Light.
			continue;
		}
		
		const auto& light =  rscene->GetLights()[i];
		lightPos[i] = glm::vec4(light->postion, 0.0f);
		lightDir[i] = glm::vec4(light->dir, 0.0f);
		lightData[i].r = light->GetType();
		lightData[i].g = light->innerAngle;
		lightData[i].b = light->outerAngle;
		lightData[i].a = light->radius;
		lightPower[i] = light->colorAndPower;
	}



	uniforms.light_DEFERRED->SetDataValues(0, lightDir);
	uniforms.light_DEFERRED->SetDataValues(1, lightPos);
	uniforms.light_DEFERRED->SetDataValues(2, lightPower);
	uniforms.light_DEFERRED->SetDataValues(3, lightData);
	uniforms.light_DEFERRED->Update();
}


void RenderPipeline::UpdateShadows()
{
	struct LightShadowBlock
	{
		// View Projection matrix from sun light point of view.
		glm::mat4 sunViewProj[RENDER_MAX_SHADOW_CASCADE];
		glm::vec4 cascadeRanges[RENDER_MAX_SHADOW_CASCADE + 1];
	} shadowData;

	static std::vector<glm::mat4> shadowsViewProj(RENDER_MAX_SHADOW_CASCADE);
	const auto& sunShadow = rscene->GetEnvironment().sunShadow;
	shadowData.cascadeRanges[0].x = sunShadow->GetCascadeRanges()[0];

	for (int32_t i = 0; i < RENDER_MAX_SHADOW_CASCADE; ++i)
	{
		shadowData.sunViewProj[i] = sunShadow->GetCascade(i).viewProj;
		shadowData.cascadeRanges[i+1].x = sunShadow->GetCascadeRanges()[i+1];
	}

	uniforms.lightShadow->UpdateData(sizeof(LightShadowBlock), 0, &shadowData);

}


void RenderPipeline::RenderEnvSky()
{
	rtFilter->FilterSky(skyCubeShader.get(), skyCubePass.get(), skyEnv);
}






} // End of namespace Raven.

