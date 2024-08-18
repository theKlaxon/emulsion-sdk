//========= Copyright © 1996-2007, Valve LLC, All rights reserved. ============
//
// Purpose: Lightmapped paint shader
//
// $Header: $
// $NoKeywords: $
//=============================================================================

#include "pbr_paint_dx9_helper.h"
#include "BaseVSShader.h"
#include "shaderlib/commandbuilder.h"
#include "convar.h"
//#include "lightmappedgeneric_vs20.inc"
//#include "lightmappedpaint_ps20.inc"
#include "include/pbr_paint_ps20b.inc"
#include "include/pbr_vs20b.inc"

#include "tier0/vprof.h"

#include "tier0/memdbgon.h"

ConVar mat_fullbright("mat_fullbright", "0", FCVAR_CHEAT);

ConVar mat_ambient_light_r("mat_ambient_light_r", "0.0", FCVAR_CHEAT);
ConVar mat_ambient_light_g("mat_ambient_light_g", "0.0", FCVAR_CHEAT);
ConVar mat_ambient_light_b("mat_ambient_light_b", "0.0", FCVAR_CHEAT);

void DrawPbrPaint_DX9( CBaseVSShader* pShader, IMaterialVar** params, IShaderDynamicAPI *pShaderAPI, IShaderShadow* pShaderShadow,
								 PBR_Vars_t &info, CPaintContextData*pContextDataPtr )
{
	SHADOW_STATE
	{
		pShader->SetInitialShadowState();
	}

	bool bSinglePassFlashlight = true;
	bool hasFlashlight = pShader->UsingFlashlight( params );
	CPaintContextData* pContextData = pContextDataPtr;

	if ( pShaderShadow || ( ! pContextData ) || pContextData->m_bMaterialVarsChanged )
	{
		//bool bFullyOpaqueWithoutAlphaTest = false; 
		bool bFullyOpaque = false;
		bool bNeedRegenStaticCmds = (! pContextData ) || pShaderShadow;
		bool hasBump = g_pConfig->UseBumpmapping();
				
		if ( pShaderShadow || bNeedRegenStaticCmds )
		{
			bool hasVertexColor = IS_FLAG_SET( MATERIAL_VAR_VERTEXCOLOR );
			bool hasEnvmap = true; //params[info.m_nEnvmap]->IsTexture();
			int envmap_variant; //0 = no envmap, 1 = regular, 2 = darken in shadow mode

			if( hasEnvmap )
			{
				//only enabled darkened cubemap mode when the scale calls for it. And not supported in ps20 when also using a 2nd bumpmap
				envmap_variant = 1;// ((GetFloatParam(info.m_nEnvMapLightScale, params) > 0.0f) && g_pHardwareConfig->SupportsPixelShaders_2_b()) ? 2 : 1;
			}
			else
			{
				envmap_variant = 0; 
			}

			bool bSeamlessMapping = false;// ((info.m_nSeamlessMappingScale != -1) &&
									  //( params[info.m_nSeamlessMappingScale]->GetFloatValue() != 0.0 ) );
			
			if ( bNeedRegenStaticCmds )
			{
				pContextData->ResetStaticCmds();
				CCommandBufferBuilder< CFixedCommandStorageBuffer< 5000 > > staticCmdsBuf;
				
				staticCmdsBuf.BindStandardTexture( SHADER_SAMPLER1, TEXTURE_LIGHTMAP );
				staticCmdsBuf.BindStandardTexture( SHADER_SAMPLER9, TEXTURE_PAINT );

				staticCmdsBuf.StoreEyePosInPixelShaderConstant( 10 );
				staticCmdsBuf.SetPixelShaderFogParams( 11 );

				staticCmdsBuf.End();

				// now, copy buf
				pContextData->m_pStaticCmds = new uint8[staticCmdsBuf.Size()];
				memcpy( pContextData->m_pStaticCmds, staticCmdsBuf.Base(), staticCmdsBuf.Size() );
			}

			if ( pShaderShadow )
			{
				pShader->EnableAlphaBlending( SHADER_BLEND_SRC_ALPHA, SHADER_BLEND_ONE_MINUS_SRC_ALPHA );

				// Alpha test: FIXME: shouldn't this be handled in Shader_t::SetInitialShadowState
				pShaderShadow->EnableAlphaTest( true );
				pShaderShadow->AlphaFunc( SHADER_ALPHAFUNC_GREATER, 0.0f );
				
				
				unsigned int flags = VERTEX_POSITION;

				//paint map
				pShaderShadow->EnableTexture( SHADER_SAMPLER9, true );
				pShaderShadow->EnableSRGBRead( SHADER_SAMPLER9, false );

				pShaderShadow->EnableTexture( SHADER_SAMPLER1, true );
				if( g_pHardwareConfig->GetHDRType() == HDR_TYPE_NONE )
				{
					pShaderShadow->EnableSRGBRead( SHADER_SAMPLER1, true );
				}
				else
				{
					pShaderShadow->EnableSRGBRead( SHADER_SAMPLER1, false );
				}
				
				//splat bump map
				pShaderShadow->EnableTexture( SHADER_SAMPLER7, true );
				pShaderShadow->EnableSRGBRead( SHADER_SAMPLER7, false );

				flags |= VERTEX_TANGENT_S | VERTEX_TANGENT_T | VERTEX_NORMAL;

				if( hasFlashlight )
				{
					pShaderShadow->EnableTexture( SHADER_SAMPLER13, true );
					pShaderShadow->EnableTexture( SHADER_SAMPLER14, true );
					pShaderShadow->EnableTexture( SHADER_SAMPLER15, true );
				}

				if( hasVertexColor )
				{
					flags |= VERTEX_COLOR;
				}

				// texcoord0 : base texcoord
				// texcoord1 : lightmap texcoord
				// texcoord2 : lightmap texcoord offset
				int numTexCoords;
				{
					numTexCoords = 2;
					if( hasBump )
					{
						numTexCoords = 3;
					}
				}
		
				//int nLightingPreviewMode = IS_FLAG2_SET( MATERIAL_VAR2_USE_GBUFFER0 ) + 2 * IS_FLAG2_SET( MATERIAL_VAR2_USE_GBUFFER1 );

				pShaderShadow->VertexShaderVertexFormat( flags, numTexCoords, 0, 0 );

				// Pre-cache pixel shaders

				// Brushes don't need ambient cubes or dynamic lights
				if (!IS_FLAG_SET(MATERIAL_VAR_MODEL))
				{
					pContextData->lightState.m_bAmbientLight = false;
					pContextData->lightState.m_nNumLights = 0;
				}

#define TCOMBINE_NONE 12									// there is no detail texture

				DECLARE_STATIC_PIXEL_SHADER(pbr_paint_ps20b);
				SET_STATIC_PIXEL_SHADER_COMBO(BUMPMAP, 1);
				SET_STATIC_PIXEL_SHADER_COMBO(CUBEMAP, envmap_variant);
				SET_STATIC_PIXEL_SHADER_COMBO(SEAMLESS, bSeamlessMapping);
				SET_STATIC_PIXEL_SHADER_COMBO(FLASHLIGHT, hasFlashlight);
				SET_STATIC_PIXEL_SHADER(pbr_paint_ps20b);

				// HACK HACK HACK - enable alpha writes all the time so that we have them for
				// underwater stuff and writing depth to dest alpha
				// But only do it if we're not using the alpha already for translucency
				pShaderShadow->EnableAlphaWrites( bFullyOpaque );

				pShaderShadow->EnableSRGBWrite( false );

				pShader->DefaultFog();

				float flLScale = pShaderShadow->GetLightMapScaleFactor();
				pShader->PI_BeginCommandBuffer();
				pShader->PI_SetModulationPixelShaderDynamicState( 21 );

				pShader->PI_SetModulationPixelShaderDynamicState_LinearScale_ScaleInW( 12, flLScale );
				pShader->PI_SetModulationVertexShaderDynamicState_LinearScale( flLScale );
				pShader->PI_EndCommandBuffer();
			} // end shadow state
		} // end shadow || regen display list

		if ( pShaderAPI && ( pContextData->m_bMaterialVarsChanged ) )
		{
			// need to regenerate the semistatic cmds
			pContextData->m_SemiStaticCmdsOut.Reset();
			pContextData->m_bMaterialVarsChanged = false;

			bool bHasBlendMaskTransform = false;
			// If we don't have a texture transform, we don't have
			// to set vertex shader constants or run vertex shader instructions
			// for the texture transform.
			bool bHasTextureTransform = false;
			
			bHasTextureTransform |= bHasBlendMaskTransform;
			
			//pContextData->m_bVertexShaderFastPath = !bHasTextureTransform;

			//if( params[info.m_nDetail]->IsTexture() )
			//{
			//	pContextData->m_bVertexShaderFastPath = false;
			//}

			//int nTransformToLoad = info.m_nBlendMaskTransform;
			//if( ( hasBump || hasSSBump ) && !hasSelfIllum && !bHasBlendModulateTexture )
			//{
			//	nTransformToLoad = info.m_nBumpTransform;
			//}

			//pContextData->m_SemiStaticCmdsOut.SetVertexShaderTextureTransform( 
			//	VERTEX_SHADER_SHADER_SPECIFIC_CONST_10, nTransformToLoad );

			//if ( ! pContextData->m_bVertexShaderFastPath )
			//{
			//	bool bSeamlessMapping = ( ( info.m_nSeamlessMappingScale != -1 ) && 
			//							  ( params[info.m_nSeamlessMappingScale]->GetFloatValue() != 0.0 ) );
			//	bool hasEnvmapMask = params[info.m_nEnvmapMask]->IsTexture();
			//	if (!bSeamlessMapping )
			//		pContextData->m_SemiStaticCmdsOut.SetVertexShaderTextureTransform( VERTEX_SHADER_SHADER_SPECIFIC_CONST_0, info.m_nBaseTextureTransform );
			//	// If we have a detail texture, then the bump texcoords are the same as the base texcoords.
			//	if( hasBump )
			//	{
			//		pContextData->m_SemiStaticCmdsOut.SetVertexShaderTextureTransform( VERTEX_SHADER_SHADER_SPECIFIC_CONST_2, info.m_nBumpTransform );
			//	}
			//	if( hasEnvmapMask )
			//	{
			//		pContextData->m_SemiStaticCmdsOut.SetVertexShaderTextureTransform( VERTEX_SHADER_SHADER_SPECIFIC_CONST_4, info.m_nEnvmapMaskTransform );
			//	}
			//}

			//pContextData->m_SemiStaticCmdsOut.SetEnvMapTintPixelShaderDynamicState( 0, info.m_nEnvmapTint );
			
			//float envmapTintVal[4];
			//float selfIllumTintVal[4];
			//params[info.m_nEnvmapTint]->GetVecValue( envmapTintVal, 3 );
			//params[info.m_nSelfIllumTint]->GetVecValue( selfIllumTintVal, 3 );

			//float envmapContrast = params[info.m_nEnvmapContrast]->GetFloatValue();
			//float envmapSaturation = params[info.m_nEnvmapSaturation]->GetFloatValue();
			//float fresnelReflection = params[info.m_nFresnelReflection]->GetFloatValue();

			//bool hasEnvmap = params[info.envMap]->IsTexture();
			//int envmap_variant = 1; //0 = no envmap, 1 = regular, 2 = darken in shadow mode

			//if( hasEnvmap )
			//{
			//	//only enabled darkened cubemap mode when the scale calls for it. And not supported in ps20 when also using a 2nd bumpmap
			//	envmap_variant = ((GetFloatParam( info.m_nEnvMapLightScale, params ) > 0.0f) && g_pHardwareConfig->SupportsPixelShaders_2_b()) ? 2 : 1;
			//}
			//else
			//{
			//	envmap_variant = 0; 
			//}

			//pContextData->m_bPixelShaderFastPath = false;// true;
			//bool bUsingContrastOrSaturation = hasEnvmap && ( ( (envmapContrast != 0.0f) && (envmapContrast != 1.0f) ) || (envmapSaturation != 1.0f) );
			//bool bUsingFresnel = hasEnvmap && (fresnelReflection != 1.0f);
			//bool bUsingSelfIllumTint = IS_FLAG_SET(MATERIAL_VAR_SELFILLUM) && (selfIllumTintVal[0] != 1.0f || selfIllumTintVal[1] != 1.0f || selfIllumTintVal[2] != 1.0f); 
			//if ( bUsingContrastOrSaturation || bUsingFresnel || bUsingSelfIllumTint || !g_pConfig->bShowSpecular )
			//{
			//	pContextData->m_bPixelShaderFastPath = false;
			//}

			//if( !pContextData->m_bPixelShaderFastPath )
			//{
			//	pContextData->m_SemiStaticCmdsOut.SetPixelShaderConstants( 2, 3 );
			//	pContextData->m_SemiStaticCmdsOut.OutputConstantData( params[info.m_nEnvmapContrast]->GetVecValue() );
			//	pContextData->m_SemiStaticCmdsOut.OutputConstantData( params[info.m_nEnvmapSaturation]->GetVecValue() );
			//	float flFresnel = params[info.m_nFresnelReflection]->GetFloatValue();
			//	// [ 0, 0, 1-R(0), R(0) ]
			//	pContextData->m_SemiStaticCmdsOut.OutputConstantData4( 0., 0., 1.0 - flFresnel, flFresnel );
			//	
			//	pContextData->m_SemiStaticCmdsOut.SetPixelShaderConstant( 7, params[info.m_nSelfIllumTint]->GetVecValue() );
			//}

			// parallax and cubemap light scale mapping parms (c20)
			//if ( envmap_variant == 2 )
			//{
			//	pContextData->m_SemiStaticCmdsOut.SetPixelShaderConstant4( 20, GetFloatParam( info.m_nHeightScale, params), GetFloatParam( info.m_nEnvMapLightScale, params), 0, 0 );
			//}

			// texture binds
			
			// handle mat_fullbright 2
			bool bLightingOnly = mat_fullbright.GetInt() == 2 && !IS_FLAG_SET( MATERIAL_VAR_NO_DEBUG_OVERRIDE );
			if( bLightingOnly )
			{
				// disable color modulation
				float color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
				pContextData->m_SemiStaticCmdsOut.SetVertexShaderConstant( VERTEX_SHADER_MODULATION_COLOR, color );

				// turn off environment mapping
				//envmapTintVal[0] = 0.0f;
				//envmapTintVal[1] = 0.0f;
				//envmapTintVal[2] = 0.0f;
			}

			if( IS_PARAM_DEFINED( info.m_nPaintSplatNormal ) )
			{
				pContextData->m_SemiStaticCmdsOut.BindTexture( pShader, SHADER_SAMPLER7, info.m_nPaintSplatNormal );
			}
			else
			{
				pContextData->m_SemiStaticCmdsOut.BindStandardTexture( SHADER_SAMPLER7, TEXTURE_NORMALMAP_FLAT );
			}

			if ( hasFlashlight )
			{
				pContextData->m_SemiStaticCmdsOut.SetVertexShaderFlashlightState( VERTEX_SHADER_SHADER_SPECIFIC_CONST_6 );

				CBCmdSetPixelShaderFlashlightState_t state;
				state.m_LightSampler = SHADER_SAMPLER13;
				state.m_DepthSampler = SHADER_SAMPLER14;
				state.m_ShadowNoiseSampler = SHADER_SAMPLER15;
				state.m_nColorConstant = 28;
				state.m_nAttenConstant = 13;
				state.m_nOriginConstant = 14;
				state.m_nDepthTweakConstant = 19;
				state.m_nScreenScaleConstant = 31;
				state.m_nWorldToTextureConstant = -1;
				state.m_bFlashlightNoLambert = false;
				state.m_bSinglePassFlashlight = bSinglePassFlashlight;
				pContextData->m_SemiStaticCmdsOut.SetPixelShaderFlashlightState( state );
			}

			pContextData->m_SemiStaticCmdsOut.End();
		}
	}
	DYNAMIC_STATE
	{
		pShaderAPI->SetDefaultState();
		CCommandBufferBuilder< CFixedCommandStorageBuffer< 1000 > > DynamicCmdsOut;
		DynamicCmdsOut.Call( pContextData->m_pStaticCmds );
		DynamicCmdsOut.Call( pContextData->m_SemiStaticCmdsOut.Base() );

		bool hasEnvmap = params[info.envMap]->IsTexture();

		if( hasEnvmap )
		{
			if( IS_PARAM_DEFINED( info.envMap ) )
			{
				DynamicCmdsOut.BindTexture( pShader, SHADER_SAMPLER2, info.envMap );
			}
			else
			{
				DynamicCmdsOut.BindTexture( pShader, SHADER_SAMPLER2, info.m_nPaintSplatEnvMap );
			}
		}

		bool bVertexShaderFastPath = false;// pContextData->m_bVertexShaderFastPath;

		int nFixedLightingMode = pShaderAPI->GetIntRenderingParameter( INT_RENDERPARM_ENABLE_FIXED_LIGHTING );
		if( nFixedLightingMode != ENABLE_FIXED_LIGHTING_NONE )
		{
			{
				bVertexShaderFastPath = false;
			}
		}

		bool bWorldNormal = ( nFixedLightingMode == ENABLE_FIXED_LIGHTING_OUTPUTNORMAL_AND_DEPTH );
		if ( bWorldNormal && IsPC() )
		{
			float vEyeDir[4];
			pShaderAPI->GetWorldSpaceCameraDirection( vEyeDir );

			float flFarZ = pShaderAPI->GetFarZ();
			vEyeDir[0] /= flFarZ;	// Divide by farZ for SSAO algorithm
			vEyeDir[1] /= flFarZ;
			vEyeDir[2] /= flFarZ;
			DynamicCmdsOut.SetVertexShaderConstant4( 12, vEyeDir[0], vEyeDir[1], vEyeDir[2], 1.0f );
		}

		//MaterialFogMode_t fogType = pShaderAPI->GetSceneFogMode();
		//DECLARE_DYNAMIC_VERTEX_SHADER( lightmappedgeneric_vs20 );
		//SET_DYNAMIC_VERTEX_SHADER_COMBO( FASTPATH,  bVertexShaderFastPath );
		//SET_DYNAMIC_VERTEX_SHADER_CMD( DynamicCmdsOut, lightmappedgeneric_vs20 );
		//DECLARE_DYNAMIC_VERTEX_SHADER(pbr_vs20);
		//SET_DYNAMIC_VERTEX_SHADER_COMBO(DOWATERFOG, pContextData->fogIndex);
		//SET_DYNAMIC_VERTEX_SHADER_COMBO(SKINNING, pContextData->numBones > 0);
		//SET_DYNAMIC_VERTEX_SHADER_COMBO(LIGHTING_PREVIEW, pContextData->renderParam != 0);
		//SET_DYNAMIC_VERTEX_SHADER_COMBO(COMPRESSED_VERTS, (int)pContextData->vertexCompress);
		//SET_DYNAMIC_VERTEX_SHADER_COMBO(NUM_LIGHTS, pContextData->lightState.m_nNumLights);
		//SET_DYNAMIC_VERTEX_SHADER(pbr_vs20);

		bool bPixelShaderFastPath = false;// pContextData->m_bPixelShaderFastPath;

		if ( nFixedLightingMode != ENABLE_FIXED_LIGHTING_NONE )
		{
			bPixelShaderFastPath = false;
		}
		bool bWriteDepthToAlpha;
		bool bWriteWaterFogToAlpha;
		{
			//can't write a special value to dest alpha if we're actually using as-intended alpha
			bWriteDepthToAlpha = false;
			bWriteWaterFogToAlpha = false;
		}

		bool bFlashlightShadows = false;
		bool bUberlight = false;
		if( hasFlashlight && IsX360() )
		{
			pShaderAPI->GetFlashlightShaderInfo( &bFlashlightShadows, &bUberlight );
		}
		else
		{
			// only do ambient light when not using flashlight
			static ConVarRef mat_ambient_light_r_forced( "mat_ambient_light_r_forced" );
			static ConVarRef mat_ambient_light_g_forced( "mat_ambient_light_g_forced" );
			static ConVarRef mat_ambient_light_b_forced( "mat_ambient_light_b_forced" );

			float vAmbientColor[4] = { mat_ambient_light_r_forced.GetFloat() != -1.0f ? mat_ambient_light_r_forced.GetFloat() : mat_ambient_light_r.GetFloat(), 
				mat_ambient_light_g_forced.GetFloat() != -1.0f ? mat_ambient_light_g_forced.GetFloat() : mat_ambient_light_g.GetFloat(), 
				mat_ambient_light_b_forced.GetFloat() != -1.0f ? mat_ambient_light_b_forced.GetFloat() : mat_ambient_light_b.GetFloat(), 
				0.0f };
			if ( mat_fullbright.GetInt() == 1 )
			{
				vAmbientColor[0] = vAmbientColor[1] = vAmbientColor[2] = 0.0f;
			}
			DynamicCmdsOut.SetPixelShaderConstant( 31, vAmbientColor, 1 );
		}
		
		{
			DECLARE_DYNAMIC_PIXEL_SHADER( pbr_paint_ps20b );
			SET_DYNAMIC_PIXEL_SHADER_COMBO( FASTPATH,  bPixelShaderFastPath );
			SET_DYNAMIC_PIXEL_SHADER_COMBO( FASTPATHENVMAPCONTRAST, bPixelShaderFastPath );
			
			// Don't write fog to alpha if we're using translucency
			SET_DYNAMIC_PIXEL_SHADER_COMBO(FLASHLIGHTSHADOWS, true);
			SET_DYNAMIC_PIXEL_SHADER_CMD( DynamicCmdsOut, pbr_paint_ps20b);
		}

		DynamicCmdsOut.End();
		pShaderAPI->ExecuteCommandBuffer( DynamicCmdsOut.Base() );
	}
	pShader->Draw();
}
