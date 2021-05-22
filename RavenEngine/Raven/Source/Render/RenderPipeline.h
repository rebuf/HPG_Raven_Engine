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




#include "Utilities/Core.h"


#include "glm/vec2.hpp"
#include "glm/vec4.hpp"




namespace Raven
{
	class RenderTarget;
	class RenderScene;
	class RenderPass;
	class RenderScreen;
	class RenderSphere;
	class RenderGrid;
	class RenderRscShader;
	class UniformBuffer;
	class GLTexture;
	class GLShader;
	class RenderTexFilter;





	// Uniforms of the render pipline.
	struct RenderPipelineUniforms
	{
		// Common Uniform Buffer - used by all shaders.
		Ptr<UniformBuffer> common;

		// Light framebuffer used for deferred lighting data.
		Ptr<UniformBuffer> light_DEFERRED;

		// Light framebuffer used for forward lighting data.
		Ptr<UniformBuffer> light_FORWARD;

		// Shadow Uniform Buffer used while drawing the shadow maps.
		Ptr<UniformBuffer> shadow;

		// Shadow Uniform Buffer, used in the lighting pass.
		Ptr<UniformBuffer> lightShadow;
	};





	// RenderPipeline:
	//		- The Engine Render Pipeline, Manage all render passes that is used to render our 
	//			scenes from start to finish.
	//
	//		- Only one instance created/managed by the RenderModule.
	//
	class RenderPipeline
	{
		// Friend...
		friend class RenderModule;

		// Private Construct, @see RenderModule.
		RenderPipeline(Ptr<RenderScreen> screen, Ptr<RenderSphere> sphere);

	public:
		// Destruct.
		~RenderPipeline();

		// Initializie the pipeline.
		void Initialize();

		// Destroy the pipeline.
		void Destroy();

		// Begin Rendering The Pipeline.
		void Begin(RenderTarget* target, RenderScene* scene, float time);

		// End Rendering The Pipeline.
		void End();

		// Render the scene into the target using the pipeline.
		void Render();

	private:
		// Build the render passes of the render pipeline.
		void SetupRenderPasses();

		// Load & Build the shaders of the render pipeline.
		void SetupShaders();

		// Setup Screen Space Ambient Occlusion.
		void SetupSSAO();

		// Setyp Sky & Sky Filter passess.
		void SetupSky();

		// Resize all render passes.
		void Resize(const glm::ivec2& newSize);

		// Update Light Unifrom Buffer for deferred lighting.
		void UpdateLights_DEFERRED();
		
		// Update Shadows Unifrom Buffer..
		void UpdateShadows();

		// Do a final post processing and use the final render target.
		void DoPostProcessFinal(int32_t hdrTargetIndex);

		// Render the environment sky to match the new updated sky.
		void RenderEnvSky();

	private:
		// Deferred Render Pass - G-Buffer.
		Ptr<RenderPass> gbufferPass;

		// Deferred Render Pass - Lighting.
		Ptr<RenderPass> lightingPass;

		// Forward Rendering Pass - used mostly for translucent.
		Ptr<RenderPass> forwardPass;

		// The final post-processing step.
		Ptr<RenderPass> finalPostProcessPass;

		// The SSAO pass.
		Ptr<RenderPass> ssaoPass;
		Ptr<RenderPass> ssaoBlurPass;

		// Pass for drawing the sky into a cubemap.
		Ptr<RenderPass> skyCubePass;

		// Pipeline HDR Render Targets.
		Ptr<GLTexture> hdrTarget[2];

		// Lighting shader for the lighting render pass.
		Ptr<RenderRscShader> lightingShader;

		// Final post processing shader used to do Gamma correct, tone mapping etc...
		Ptr<RenderRscShader> finalPostProcessShader;

		// Fast Approximate Anti-Aliasing executed on the final pos-tprocesing step
		Ptr<RenderRscShader> fxaaShader;

		// SSAO Shader.
		Ptr<RenderRscShader> ssaoShader;
		
		// SSAO Blur Shader.
		Ptr<RenderRscShader> ssaoBlurShader;

		// Sky Shader, Draw the sky in thee scene.
		Ptr<RenderRscShader> skyShader;

		// Sky Cube Map, draw the sky for cube map creation.
		Ptr<RenderRscShader> skyCubeShader;

		// Screen triangle used to render the entire screen, used by render passes and post-processing.
		Ptr<RenderScreen> rscreen;

		// Sphere.
		Ptr<RenderSphere> rsphere;

		// Render filter instance.
		RenderTexFilter* rtFilter;

		// Uniforms of the pipeline.
		RenderPipelineUniforms uniforms;

		// The current render target of the scene we are rendering on.
		RenderTarget* rtarget;

		// The current scene we are rendering.
		RenderScene* rscene;

		// The viewport used for rendering.
		glm::vec4 viewport;

		// Render Pipeline targets size;
		glm::ivec2 size;

		// The Render Grid.
		Ptr<RenderGrid> rgrid;
		
		// Noise texture used in SSAO pass.
		Ptr<GLTexture> ssaoNoiseTexture;

		// Sky Cube Map.
		Ptr<GLTexture> skyCubeMap;

		// Sky Environment - filterd skyTexture for IBL.
		Ptr<GLTexture> skyEnv;

		// Current environment map.
		GLTexture* EnvMap;

		// BRDF Lookup table used form IBL.
		GLTexture* BRDF;
	};


}