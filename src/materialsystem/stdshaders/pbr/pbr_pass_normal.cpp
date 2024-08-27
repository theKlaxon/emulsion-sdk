#include "BaseVSShader.h"
#include "cpp_shader_constant_register_map.h"
#include "pbr_paint_dx9_helper.h"

#include "pbr_ps30.inc"
//#include "pbr_ps20b.inc"

void DrawPbrPass_Normal(CBaseVSShader* pShader, IMaterialVar** params, IShaderDynamicAPI* pShaderAPI, IShaderShadow* pShaderShadow,
	PBR_Vars_t& info, PBR_Switches_t const& switches, CBasePerMaterialContextData** pContextDataPtr) {

	if (pShader->IsSnapshotting())
	{
		// If alphatest is on, enable it
		pShaderShadow->EnableAlphaTest(switches.bIsAlphaTested);

		if (info.alphaTestReference != -1 && params[info.alphaTestReference]->GetFloatValue() > 0.0f)
		{
			pShaderShadow->AlphaFunc(SHADER_ALPHAFUNC_GEQUAL, params[info.alphaTestReference]->GetFloatValue());
		}

		if (switches.bHasFlashlight)
		{
			pShaderShadow->EnableBlending(true);
			pShaderShadow->BlendFunc(SHADER_BLEND_ONE, SHADER_BLEND_ONE); // Additive blending
		}
		else
		{
			pShader->SetDefaultBlendingShadowState(info.baseTexture, true);
		}
		
		// Setting up samplers
		pShaderShadow->EnableTexture(SAMPLER_BASETEXTURE, true);    // Basecolor texture
		pShaderShadow->EnableSRGBRead(SAMPLER_BASETEXTURE, true);   // Basecolor is sRGB
		pShaderShadow->EnableTexture(SAMPLER_EMISSIVE, true);       // Emission texture
		pShaderShadow->EnableSRGBRead(SAMPLER_EMISSIVE, true);      // Emission is sRGB
		pShaderShadow->EnableTexture(SAMPLER_LIGHTMAP, true);       // Lightmap texture
		pShaderShadow->EnableSRGBRead(SAMPLER_LIGHTMAP, false);     // Lightmaps aren't sRGB
		pShaderShadow->EnableTexture(SAMPLER_MRAO, true);           // MRAO texture
		pShaderShadow->EnableSRGBRead(SAMPLER_MRAO, false);         // MRAO isn't sRGB
		pShaderShadow->EnableTexture(SAMPLER_NORMAL, true);         // Normal texture
		pShaderShadow->EnableSRGBRead(SAMPLER_NORMAL, false);       // Normals aren't sRGB
		pShaderShadow->EnableTexture(SAMPLER_SPECULAR, true);       // Specular F0 texture
		pShaderShadow->EnableSRGBRead(SAMPLER_SPECULAR, true);      // Specular F0 is sRGB

		// If the flashlight is on, set up its textures
		if (switches.bHasFlashlight)
		{
			pShaderShadow->EnableTexture(SAMPLER_SHADOWDEPTH, true);        // Shadow depth map
			//pShaderShadow->SetShadowDepthFiltering(SAMPLER_SHADOWDEPTH);
			pShaderShadow->EnableDepthWrites(true);
			pShaderShadow->EnableSRGBRead(SAMPLER_SHADOWDEPTH, false);
			pShaderShadow->EnableTexture(SAMPLER_RANDOMROTATION, true);     // Noise map
			pShaderShadow->EnableTexture(SAMPLER_FLASHLIGHT, true);         // Flashlight cookie
			pShaderShadow->EnableSRGBRead(SAMPLER_FLASHLIGHT, true);
		}

		// Setting up envmap
		if (switches.bHasEnvTexture)
		{
			pShaderShadow->EnableTexture(SAMPLER_ENVMAP, true); // Envmap
			if (g_pHardwareConfig->GetHDRType() == HDR_TYPE_NONE)
			{
				pShaderShadow->EnableSRGBRead(SAMPLER_ENVMAP, true); // Envmap is only sRGB with HDR disabled?
			}
		}

		// Enabling sRGB writing
		// See common_ps_fxc.h line 349
		// PS2b shaders and up write sRGB
		pShaderShadow->EnableSRGBWrite(true);

		if (IS_FLAG_SET(MATERIAL_VAR_MODEL))
		{
			// We only need the position and surface normal
			unsigned int flags = VERTEX_POSITION | VERTEX_NORMAL | VERTEX_FORMAT_COMPRESSED;
			// We need three texcoords, all in the default float2 size
			pShaderShadow->VertexShaderVertexFormat(flags, 1, 0, 0);
		}
		else
		{
			// We need the position, surface normal, and vertex compression format
			unsigned int flags = VERTEX_POSITION | VERTEX_NORMAL;
			// We only need one texcoord, in the default float2 size
			pShaderShadow->VertexShaderVertexFormat(flags, 3, 0, 0);
		}

		//if (!g_pHardwareConfig->HasFastVertexTextures() || mat_pbr_force_20b.GetBool())
		//{
		//	// Setting up static vertex shader
		//	DECLARE_STATIC_VERTEX_SHADER(pbr_vs20);
		//	SET_STATIC_VERTEX_SHADER(pbr_vs20);

		//	// Setting up static pixel shader
		//	DECLARE_STATIC_PIXEL_SHADER(pbr_ps20b);
		//	SET_STATIC_PIXEL_SHADER_COMBO(FLASHLIGHT, switches.bHasFlashlight);
		//	SET_STATIC_PIXEL_SHADER_COMBO(FLASHLIGHTDEPTHFILTERMODE, switches.nShadowFilterMode);
		//	SET_STATIC_PIXEL_SHADER_COMBO(LIGHTMAPPED, switches.bLightMapped);
		//	SET_STATIC_PIXEL_SHADER_COMBO(EMISSIVE, switches.bHasEmissionTexture);
		//	SET_STATIC_PIXEL_SHADER_COMBO(SPECULAR, 0);
		//	SET_STATIC_PIXEL_SHADER_COMBO(PARALLAXCORRECT, switches.bHasParallaxCorrection);
		//	SET_STATIC_PIXEL_SHADER(pbr_ps20b);
		//}
		//else
		{
			// Setting up static vertex shader
			DECLARE_STATIC_VERTEX_SHADER(pbr_vs30);
			SET_STATIC_VERTEX_SHADER(pbr_vs30);

			// Setting up static pixel shader
			DECLARE_STATIC_PIXEL_SHADER(pbr_ps30);
			SET_STATIC_PIXEL_SHADER_COMBO(FLASHLIGHT, switches.bHasFlashlight);
			SET_STATIC_PIXEL_SHADER_COMBO(FLASHLIGHTDEPTHFILTERMODE, switches.nShadowFilterMode);
			SET_STATIC_PIXEL_SHADER_COMBO(LIGHTMAPPED, switches.bLightMapped);
			SET_STATIC_PIXEL_SHADER_COMBO(USEENVAMBIENT, switches.bUseEnvAmbient);
			SET_STATIC_PIXEL_SHADER_COMBO(EMISSIVE, switches.bHasEmissionTexture);
			SET_STATIC_PIXEL_SHADER_COMBO(SPECULAR, switches.bHasSpecularTexture);
			SET_STATIC_PIXEL_SHADER_COMBO(PARALLAXOCCLUSION, switches.bUseParallax);
			SET_STATIC_PIXEL_SHADER_COMBO(PARALLAXCORRECT, switches.bHasParallaxCorrection);
			SET_STATIC_PIXEL_SHADER(pbr_ps30);
		}

		// Setting up fog
		pShader->DefaultFog(); // I think this is correct

		// HACK HACK HACK - enable alpha writes all the time so that we have them for underwater stuff
		pShaderShadow->EnableAlphaWrites(switches.bFullyOpaque);

		if (switches.bHasParallaxCorrection)
		{
			pShaderAPI->SetPixelShaderConstant(3, params[info.m_nEnvmapOrigin]->GetVecValue());

			const float* vecs[3];
			vecs[0] = params[info.m_nEnvmapParallaxObb1]->GetVecValue();
			vecs[1] = params[info.m_nEnvmapParallaxObb2]->GetVecValue();
			vecs[2] = params[info.m_nEnvmapParallaxObb3]->GetVecValue();
			float matrix[4][4];
			for (int i = 0; i < 3; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					matrix[i][j] = vecs[i][j];
				}
			}
			matrix[3][0] = matrix[3][1] = matrix[3][2] = 0;
			matrix[3][3] = 1;
			pShaderAPI->SetPixelShaderConstant(26, &matrix[0][0], 4);
		}

		pShader->PI_BeginCommandBuffer();

		// Send ambient cube to the pixel shader, force to black if not available
		pShader->PI_SetPixelShaderAmbientLightCube(PSREG_AMBIENT_CUBE);

		// Send lighting array to the pixel shader
		pShader->PI_SetPixelShaderLocalLighting(PSREG_LIGHT_INFO_ARRAY);

		// Set up shader modulation color
		pShader->PI_SetModulationPixelShaderDynamicState(PSREG_DIFFUSE_MODULATION);

		pShader->PI_EndCommandBuffer();
	}
	else // Not snapshotting -- begin dynamic state
	{
		bool bLightingOnly = mat_fullbright.GetInt() == 2 && !IS_FLAG_SET(MATERIAL_VAR_NO_DEBUG_OVERRIDE);

		// Setting up albedo texture
		if (switches.bHasBaseTexture)
		{
			pShader->BindTexture(SAMPLER_BASETEXTURE, info.baseTexture, info.baseTextureFrame);
		}
		else
		{
			pShaderAPI->BindStandardTexture(SAMPLER_BASETEXTURE, 0, TEXTURE_GREY);
		}

		// Setting up vmt color
		Vector color;
		if (switches.bHasColor)
		{
			params[info.baseColor]->GetVecValue(color.Base(), 3);
		}
		else
		{
			color = Vector(1.f, 1.f, 1.f);
		}
		pShaderAPI->SetPixelShaderConstant(PSREG_SELFILLUMTINT, color.Base());

		// Setting up environment map
		if (switches.bHasEnvTexture)
		{
			pShader->BindTexture(SAMPLER_ENVMAP, info.envMap, 0);
		}
		else
		{
			pShaderAPI->BindStandardTexture(SAMPLER_ENVMAP, 0, TEXTURE_BLACK);
		}

		// Setting up emissive texture
		if (switches.bHasEmissionTexture)
		{
			pShader->BindTexture(SAMPLER_EMISSIVE, info.emissionTexture, 0);
		}
		else
		{
			pShaderAPI->BindStandardTexture(SAMPLER_EMISSIVE, 0, TEXTURE_BLACK);
		}

		// Setting up normal map
		if (switches.bHasNormalTexture)
		{
			pShader->BindTexture(SAMPLER_NORMAL, info.bumpMap, 0);
		}
		else
		{
			pShaderAPI->BindStandardTexture(SAMPLER_NORMAL, 0, TEXTURE_NORMALMAP_FLAT);
		}

		// Setting up mrao map
		if (switches.bHasMraoTexture)
		{
			pShader->BindTexture(SAMPLER_MRAO, info.mraoTexture, 0);
		}
		else
		{
			pShaderAPI->BindStandardTexture(SAMPLER_MRAO, 0, TEXTURE_WHITE);
		}

		if (switches.bHasSpecularTexture)
		{
			pShader->BindTexture(SAMPLER_SPECULAR, info.specularTexture, 0);
		}
		else
		{
			pShaderAPI->BindStandardTexture(SAMPLER_SPECULAR, 0, TEXTURE_BLACK);
		}

		// Getting the light state
		LightState_t lightState;
		pShaderAPI->GetDX9LightState(&lightState);

		// Brushes don't need ambient cubes or dynamic lights
		if (!IS_FLAG_SET(MATERIAL_VAR_MODEL))
		{
			lightState.m_bAmbientLight = false;
			lightState.m_nNumLights = 0;
		}

		// Setting up the flashlight related textures and variables
		bool bFlashlightShadows = false;
		if (switches.bHasFlashlight)
		{
			Assert(info.flashlightTexture >= 0 && info.flashlightTextureFrame >= 0);
			Assert(params[info.flashlightTexture]->IsTexture());
			pShader->BindTexture(SAMPLER_FLASHLIGHT, info.flashlightTexture, info.flashlightTextureFrame);

			VMatrix worldToTexture;
			ITexture* pFlashlightDepthTexture;
			FlashlightState_t state = pShaderAPI->GetFlashlightStateEx(worldToTexture, &pFlashlightDepthTexture);
			bFlashlightShadows = state.m_bEnableShadows && (pFlashlightDepthTexture != NULL);

			SetFlashLightColorFromState(state, pShaderAPI, PSREG_FLASHLIGHT_COLOR);

			if (pFlashlightDepthTexture && g_pConfig->ShadowDepthTexture() && state.m_bEnableShadows)
			{
				pShader->BindTexture(SAMPLER_SHADOWDEPTH, pFlashlightDepthTexture, 0);
				pShaderAPI->BindStandardTexture(SAMPLER_RANDOMROTATION, 0, TEXTURE_SHADOW_NOISE_2D);
			}
		}

		// Getting fog info
		MaterialFogMode_t fogType = pShaderAPI->GetSceneFogMode();
		int fogIndex = (fogType == MATERIAL_FOG_LINEAR_BELOW_FOG_Z) ? 1 : 0;

		// Getting skinning info
		int numBones = pShaderAPI->GetCurrentNumBones();

		// Some debugging stuff
		bool bWriteDepthToAlpha = false;
		bool bWriteWaterFogToAlpha = false;
		if (switches.bFullyOpaque)
		{
			bWriteDepthToAlpha = pShaderAPI->ShouldWriteDepthToDestAlpha();
			bWriteWaterFogToAlpha = (fogType == MATERIAL_FOG_LINEAR_BELOW_FOG_Z);
			AssertMsg(!(bWriteDepthToAlpha && bWriteWaterFogToAlpha),
				"Can't write two values to alpha at the same time.");
		}

		float vEyePos_SpecExponent[4];
		pShaderAPI->GetWorldSpaceCameraPosition(vEyePos_SpecExponent);

		// Determining the max level of detail for the envmap
		int iEnvMapLOD = 6;
		auto envTexture = params[info.envMap]->GetTextureValue();
		if (envTexture)
		{
			// Get power of 2 of texture width
			int width = envTexture->GetMappingWidth();
			int mips = 0;
			while (width >>= 1)
				++mips;

			// Cubemap has 4 sides so 2 mips less
			iEnvMapLOD = mips;
		}

		// Dealing with very high and low resolution cubemaps
		if (iEnvMapLOD > 12)
			iEnvMapLOD = 12;
		if (iEnvMapLOD < 4)
			iEnvMapLOD = 4;

		// This has some spare space
		vEyePos_SpecExponent[3] = iEnvMapLOD;
		pShaderAPI->SetPixelShaderConstant(PSREG_EYEPOS_SPEC_EXPONENT, vEyePos_SpecExponent, 1);

		// Setting lightmap texture
		//pShader->s_pShaderAPI->BindStandardTexture(SAMPLER_LIGHTMAP, 0, TEXTURE_LIGHTMAP_BUMPED);
		BindLightmapBumped();

		//if (!g_pHardwareConfig->HasFastVertexTextures() || mat_pbr_force_20b.GetBool())
		//{
		//	// Setting up dynamic vertex shader
		//	DECLARE_DYNAMIC_VERTEX_SHADER(pbr_vs20);
		//	SET_DYNAMIC_VERTEX_SHADER_COMBO(DOWATERFOG, fogIndex);
		//	SET_DYNAMIC_VERTEX_SHADER_COMBO(SKINNING, numBones > 0);
		//	SET_DYNAMIC_VERTEX_SHADER_COMBO(LIGHTING_PREVIEW, pShaderAPI->GetIntRenderingParameter(INT_RENDERPARM_ENABLE_FIXED_LIGHTING) != 0);
		//	SET_DYNAMIC_VERTEX_SHADER_COMBO(COMPRESSED_VERTS, info.nVertexCompression);
		//	SET_DYNAMIC_VERTEX_SHADER_COMBO(NUM_LIGHTS, lightState.m_nNumLights);
		//	SET_DYNAMIC_VERTEX_SHADER(pbr_vs20);

		//	// Setting up dynamic pixel shader
		//	DECLARE_DYNAMIC_PIXEL_SHADER(pbr_ps20b);
		//	SET_DYNAMIC_PIXEL_SHADER_COMBO(NUM_LIGHTS, lightState.m_nNumLights);
		//	SET_DYNAMIC_PIXEL_SHADER_COMBO(WRITEWATERFOGTODESTALPHA, bWriteWaterFogToAlpha);
		//	SET_DYNAMIC_PIXEL_SHADER_COMBO(WRITE_DEPTH_TO_DESTALPHA, bWriteDepthToAlpha);
		//	SET_DYNAMIC_PIXEL_SHADER_COMBO(PIXELFOGTYPE, pShaderAPI->GetPixelFogCombo());
		//	SET_DYNAMIC_PIXEL_SHADER_COMBO(FLASHLIGHTSHADOWS, bFlashlightShadows);
		//	SET_DYNAMIC_PIXEL_SHADER(pbr_ps20b);
		//}
		//else
		{
			// Setting up dynamic vertex shader
			DECLARE_DYNAMIC_VERTEX_SHADER(pbr_vs30);
			SET_DYNAMIC_VERTEX_SHADER_COMBO(DOWATERFOG, fogIndex);
			SET_DYNAMIC_VERTEX_SHADER_COMBO(SKINNING, numBones > 0);
			SET_DYNAMIC_VERTEX_SHADER_COMBO(LIGHTING_PREVIEW, pShaderAPI->GetIntRenderingParameter(INT_RENDERPARM_ENABLE_FIXED_LIGHTING) != 0);
			SET_DYNAMIC_VERTEX_SHADER_COMBO(COMPRESSED_VERTS, (int)info.nVertexCompression);
			SET_DYNAMIC_VERTEX_SHADER_COMBO(NUM_LIGHTS, lightState.m_nNumLights);
			SET_DYNAMIC_VERTEX_SHADER(pbr_vs30);

			// Setting up dynamic pixel shader
			DECLARE_DYNAMIC_PIXEL_SHADER(pbr_ps30);
			SET_DYNAMIC_PIXEL_SHADER_COMBO(NUM_LIGHTS, lightState.m_nNumLights);
			SET_DYNAMIC_PIXEL_SHADER_COMBO(WRITEWATERFOGTODESTALPHA, bWriteWaterFogToAlpha);
			SET_DYNAMIC_PIXEL_SHADER_COMBO(WRITE_DEPTH_TO_DESTALPHA, bWriteDepthToAlpha);
			SET_DYNAMIC_PIXEL_SHADER_COMBO(PIXELFOGTYPE, pShaderAPI->GetPixelFogCombo());
			SET_DYNAMIC_PIXEL_SHADER_COMBO(FLASHLIGHTSHADOWS, bFlashlightShadows);
			SET_DYNAMIC_PIXEL_SHADER(pbr_ps30);
		}

		// Setting up base texture transform
		pShader->SetVertexShaderTextureTransform(VERTEX_SHADER_SHADER_SPECIFIC_CONST_0, info.baseTextureTransform);

		// Handle mat_fullbright 2 (diffuse lighting only)
		if (bLightingOnly)
		{
			pShaderAPI->BindStandardTexture(SAMPLER_BASETEXTURE, 0, TEXTURE_GREY); // Basecolor
		}

		// Handle mat_specular 0 (no envmap reflections)
		if (!mat_specular.GetBool())
		{
			pShaderAPI->BindStandardTexture(SAMPLER_ENVMAP, 0, TEXTURE_BLACK); // Envmap
		}

		// Sending fog info to the pixel shader
		pShaderAPI->SetPixelShaderFogParams(PSREG_FOG_PARAMS);

		// More flashlight related stuff
		if (switches.bHasFlashlight)
		{
			VMatrix worldToTexture;
			float atten[4], pos[4], tweaks[4];

			const FlashlightState_t& flashlightState = pShaderAPI->GetFlashlightState(worldToTexture);
			SetFlashLightColorFromState(flashlightState, pShaderAPI, PSREG_FLASHLIGHT_COLOR);

			pShader->BindTexture(SAMPLER_FLASHLIGHT, flashlightState.m_pSpotlightTexture, flashlightState.m_nSpotlightTextureFrame);

			// Set the flashlight attenuation factors
			atten[0] = flashlightState.m_fConstantAtten;
			atten[1] = flashlightState.m_fLinearAtten;
			atten[2] = flashlightState.m_fQuadraticAtten;
			atten[3] = flashlightState.m_FarZ;
			pShaderAPI->SetPixelShaderConstant(PSREG_FLASHLIGHT_ATTENUATION, atten, 1);

			// Set the flashlight origin
			pos[0] = flashlightState.m_vecLightOrigin[0];
			pos[1] = flashlightState.m_vecLightOrigin[1];
			pos[2] = flashlightState.m_vecLightOrigin[2];
			pShaderAPI->SetPixelShaderConstant(PSREG_FLASHLIGHT_POSITION_RIM_BOOST, pos, 1);

			pShaderAPI->SetPixelShaderConstant(PSREG_FLASHLIGHT_TO_WORLD_TEXTURE, worldToTexture.Base(), 4);

			// Tweaks associated with a given flashlight
			tweaks[0] = ShadowFilterFromState(flashlightState);
			tweaks[1] = ShadowAttenFromState(flashlightState);
			pShader->HashShadow2DJitter(flashlightState.m_flShadowJitterSeed, &tweaks[2], &tweaks[3]);
			pShaderAPI->SetPixelShaderConstant(PSREG_ENVMAP_TINT__SHADOW_TWEAKS, tweaks, 1);
		}

		if (g_pHardwareConfig->HasFastVertexTextures())
		{
			float flParams[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
			// Parallax Depth (the strength of the effect)
			flParams[0] = GetFloatParam(info.parallaxDepth, params, 3.0f);
			// Parallax Center (the height at which it's not moved)
			flParams[1] = GetFloatParam(info.parallaxCenter, params, 3.0f);
			pShaderAPI->SetPixelShaderConstant(32, flParams, 1);
		}
	}

	pShader->Draw();
}