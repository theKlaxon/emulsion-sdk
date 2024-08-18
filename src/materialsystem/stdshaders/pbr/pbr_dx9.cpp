//==================================================================================================
//
// Physically Based Rendering shader for brushes and models
//
//==================================================================================================

// Includes for all shaders
#include "BaseVSShader.h"
#include "cpp_shader_constant_register_map.h"
#include "pbr_paint_dx9_helper.h"

// Beginning the shader
BEGIN_VS_SHADER(PBR, "PBR shader")

	// Setting up vmt parameters
	BEGIN_SHADER_PARAMS;
		SHADER_PARAM(ALPHATESTREFERENCE, SHADER_PARAM_TYPE_FLOAT, "0", "");
		SHADER_PARAM(ENVMAP, SHADER_PARAM_TYPE_ENVMAP, "", "Set the cubemap for this material.");
		SHADER_PARAM(MRAOTEXTURE, SHADER_PARAM_TYPE_TEXTURE, "", "Texture with metalness in R, roughness in G, ambient occlusion in B.");
		SHADER_PARAM(EMISSIONTEXTURE, SHADER_PARAM_TYPE_TEXTURE, "", "Emission texture");
		SHADER_PARAM(NORMALTEXTURE, SHADER_PARAM_TYPE_TEXTURE, "", "Normal texture (deprecated, use $bumpmap)");
		SHADER_PARAM(BUMPMAP, SHADER_PARAM_TYPE_TEXTURE, "", "Normal texture");
		SHADER_PARAM(USEENVAMBIENT, SHADER_PARAM_TYPE_BOOL, "0", "Use the cubemaps to compute ambient light.");
		SHADER_PARAM(SPECULARTEXTURE, SHADER_PARAM_TYPE_TEXTURE, "", "Specular F0 RGB map");
		SHADER_PARAM(PARALLAX, SHADER_PARAM_TYPE_BOOL, "0", "Use Parallax Occlusion Mapping.");
		SHADER_PARAM(PARALLAXDEPTH, SHADER_PARAM_TYPE_FLOAT, "0.0030", "Depth of the Parallax Map");
		SHADER_PARAM(PARALLAXCENTER, SHADER_PARAM_TYPE_FLOAT, "0.5", "Center depth of the Parallax Map");

		// Parallax-Corrected Cubemaps (ported from LightmappedGeneric)
		SHADER_PARAM( ENVMAPPARALLAX, SHADER_PARAM_TYPE_BOOL, "0", "Should envmap reflections be parallax-corrected?" )
		SHADER_PARAM( ENVMAPPARALLAXOBB1, SHADER_PARAM_TYPE_VEC4, "[1 0 0 0]", "The first line of the parallax correction OBB matrix" )
		SHADER_PARAM( ENVMAPPARALLAXOBB2, SHADER_PARAM_TYPE_VEC4, "[0 1 0 0]", "The second line of the parallax correction OBB matrix" )
		SHADER_PARAM( ENVMAPPARALLAXOBB3, SHADER_PARAM_TYPE_VEC4, "[0 0 1 0]", "The third line of the parallax correction OBB matrix" )
		SHADER_PARAM( ENVMAPORIGIN, SHADER_PARAM_TYPE_VEC3, "[0 0 0]", "The world space position of the env_cubemap being corrected" )

		// paint
		SHADER_PARAM(PAINTNORMAL, SHADER_PARAM_TYPE_TEXTURE, "", "Paintmap normal.") 
		SHADER_PARAM(PAINTENVMAP, SHADER_PARAM_TYPE_TEXTURE, "", "Paintmap env_map.") 
		SHADER_PARAM(PAINTBUBBLE, SHADER_PARAM_TYPE_TEXTURE, "", "Paintmap bubbles!") // da bubbbles (they;re neat!)
		SHADER_PARAM(PAINTBUBBLELAYOUT, SHADER_PARAM_TYPE_TEXTURE, "", "Paintmap bubbles layout / map.") 
		SHADER_PARAM(PAINTMRAO, SHADER_PARAM_TYPE_TEXTURE, "", "Paintmap MRAO - PBR time :)") 
		
	END_SHADER_PARAMS;

	// Setting up variables for this shader
	void SetupVars(PBR_Vars_t &info)
	{
		info.baseTexture = BASETEXTURE;
		info.baseColor = COLOR;
		info.normalTexture = NORMALTEXTURE;
		info.bumpMap = BUMPMAP;
		info.baseTextureFrame = FRAME;
		info.baseTextureTransform = BASETEXTURETRANSFORM;
		info.alphaTestReference = ALPHATESTREFERENCE;
		info.flashlightTexture = FLASHLIGHTTEXTURE;
		info.flashlightTextureFrame = FLASHLIGHTTEXTUREFRAME;
		info.envMap = ENVMAP;
		info.emissionTexture = EMISSIONTEXTURE;
		info.mraoTexture = MRAOTEXTURE;
		info.useEnvAmbient = USEENVAMBIENT;
		info.specularTexture = SPECULARTEXTURE;
		info.useParallax = PARALLAX;
		info.parallaxDepth = PARALLAXDEPTH;
		info.parallaxCenter = PARALLAXCENTER;

		info.m_nEnvmapParallax = ENVMAPPARALLAX;
		info.m_nEnvmapParallaxObb1 = ENVMAPPARALLAXOBB1;
		info.m_nEnvmapParallaxObb2 = ENVMAPPARALLAXOBB2;
		info.m_nEnvmapParallaxObb3 = ENVMAPPARALLAXOBB3;
		info.m_nEnvmapOrigin = ENVMAPORIGIN;

		// paint
		info.m_nPaintSplatNormal = PAINTNORMAL;
		info.m_nPaintSplatEnvMap = PAINTENVMAP;
		info.m_nPaintBubble = PAINTBUBBLE;
		info.m_nPaintBubbleLayout = PAINTBUBBLELAYOUT;
		info.m_nPaintMRAO = PAINTMRAO;
	};

	// Initializing parameters
	SHADER_INIT_PARAMS()
	{
		// Fallback for changed parameter
		if (params[NORMALTEXTURE]->IsDefined())
			params[BUMPMAP]->SetStringValue(params[NORMALTEXTURE]->GetStringValue());

		// Dynamic lights need a bumpmap
		if (!params[BUMPMAP]->IsDefined())
			params[BUMPMAP]->SetStringValue("dev/flat_normal");

		// Set a good default mrao texture
		if (!params[MRAOTEXTURE]->IsDefined())
			params[MRAOTEXTURE]->SetStringValue("dev/pbr_mraotexture");

		// PBR relies heavily on envmaps
		if (!params[ENVMAP]->IsDefined())
			params[ENVMAP]->SetStringValue("env_cubemap");

		// Check if the hardware supports flashlight border color
		if (g_pHardwareConfig->SupportsBorderColor())
		{
			params[FLASHLIGHTTEXTURE]->SetStringValue("effects/flashlight_border");
		}
		else
		{
			params[FLASHLIGHTTEXTURE]->SetStringValue("effects/flashlight001");
		}

		// paint
		if (g_pConfig->m_bPaintInGame) {

			params[PAINTNORMAL]->SetStringValue("paint/splatnormal_default");
			params[PAINTBUBBLELAYOUT]->SetStringValue("paint/bubblelayout");
			params[PAINTBUBBLE]->SetStringValue("paint/bubble");
			params[PAINTENVMAP]->SetStringValue("paint/paint_envmap_hdr");
			params[PAINTMRAO]->SetStringValue("paint/splat_mrao");

		}
		else {
			params[PAINTNORMAL]->SetUndefined();
			params[PAINTENVMAP]->SetUndefined();
			params[PAINTBUBBLE]->SetUndefined();
			params[PAINTBUBBLELAYOUT]->SetUndefined();
			params[PAINTMRAO]->SetUndefined();
		}
	};
	
	// Define shader fallback
	SHADER_FALLBACK
	{
		return 0;
	};

	SHADER_INIT
	{
		PBR_Vars_t info;
		SetupVars(info);

		Assert(info.flashlightTexture >= 0);
		LoadTexture(info.flashlightTexture);

		Assert(info.bumpMap >= 0);
		LoadBumpMap(info.bumpMap);

		Assert(info.envMap >= 0);
		LoadCubeMap(info.envMap);

		if (info.emissionTexture >= 0 && params[EMISSIONTEXTURE]->IsDefined())
			LoadTexture(info.emissionTexture);

		Assert(info.mraoTexture >= 0);
		LoadTexture(info.mraoTexture);

		if (params[info.baseTexture]->IsDefined())
		{
			LoadTexture(info.baseTexture);
		}

		if (params[info.specularTexture]->IsDefined())
		{
			LoadTexture(info.specularTexture);
		}

		if (IS_FLAG_SET(MATERIAL_VAR_MODEL)) // Set material var2 flags specific to models
		{
			SET_FLAGS2(MATERIAL_VAR2_SUPPORTS_HW_SKINNING);             // Required for skinning
			SET_FLAGS2(MATERIAL_VAR2_DIFFUSE_BUMPMAPPED_MODEL);         // Required for dynamic lighting
			SET_FLAGS2(MATERIAL_VAR2_NEEDS_TANGENT_SPACES);             // Required for dynamic lighting
			SET_FLAGS2(MATERIAL_VAR2_LIGHTING_VERTEX_LIT);              // Required for dynamic lighting
			//SET_FLAGS2(MATERIAL_VAR2_NEEDS_BAKED_LIGHTING_SNAPSHOTS);   // Required for ambient cube
			SET_FLAGS2(MATERIAL_VAR2_SUPPORTS_FLASHLIGHT);              // Required for flashlight
			SET_FLAGS2(MATERIAL_VAR2_USE_FLASHLIGHT);                   // Required for flashlight
		}
		else // Set material var2 flags specific to brushes
		{
			SET_FLAGS2(MATERIAL_VAR2_LIGHTING_LIGHTMAP);                // Required for lightmaps
			SET_FLAGS2(MATERIAL_VAR2_LIGHTING_BUMPED_LIGHTMAP);         // Required for lightmaps
			SET_FLAGS2(MATERIAL_VAR2_SUPPORTS_FLASHLIGHT);              // Required for flashlight
			SET_FLAGS2(MATERIAL_VAR2_USE_FLASHLIGHT);                   // Required for flashlight
			//SET_FLAGS2(MATERIAL_VAR2_USE_PAINTMAPS);					// required for paint (emulsion)
		}

		// Cubemap parallax correction requires all 4 lines
		if ( info.m_nEnvmapParallax != -1 && !( mat_allow_parallax_cubemaps.GetBool() && params[info.m_nEnvmapParallax]->IsDefined() && IS_PARAM_DEFINED( info.m_nEnvmapParallaxObb1 ) && IS_PARAM_DEFINED( info.m_nEnvmapParallaxObb2 ) && IS_PARAM_DEFINED( info.m_nEnvmapParallaxObb3 ) && IS_PARAM_DEFINED( info.m_nEnvmapOrigin ) ) )
		{
			params[info.m_nEnvmapParallax]->SetIntValue( 0 );
		}

		// paint
		LoadBumpMap(info.m_nPaintSplatNormal);
		LoadCubeMap(info.m_nPaintSplatEnvMap);
		LoadTexture(info.m_nPaintBubble);
		LoadTexture(info.m_nPaintBubbleLayout);
		LoadTexture(info.m_nPaintMRAO);
	};

	// Drawing the shader
	SHADER_DRAW
	{
		PBR_Vars_t info;
		SetupVars(info);
		info.nVertexCompression = vertexCompression;

		// like vars, but these wont change during the passes.
		PBR_Switches_t switches;

		// Setting up booleans
		switches.bHasBaseTexture = (info.baseTexture != -1) && params[info.baseTexture]->IsTexture();
		switches.bHasNormalTexture = (info.bumpMap != -1) && params[info.bumpMap]->IsTexture();
		switches.bHasMraoTexture = (info.mraoTexture != -1) && params[info.mraoTexture]->IsTexture();
		switches.bHasEmissionTexture = (info.emissionTexture != -1) && params[info.emissionTexture]->IsTexture();
		switches.bHasEnvTexture = (info.envMap != -1) && params[info.envMap]->IsTexture();
		switches.bIsAlphaTested = IS_FLAG_SET(MATERIAL_VAR_ALPHATEST) != 0;
		switches.bHasFlashlight = UsingFlashlight(params);
		switches.bHasColor = (info.baseColor != -1) && params[info.baseColor]->IsDefined();
		switches.bLightMapped = !IS_FLAG_SET(MATERIAL_VAR_MODEL);
		switches.bUseEnvAmbient = (info.useEnvAmbient != -1) && (params[info.useEnvAmbient]->GetIntValue() == 1);
		switches.bHasSpecularTexture = (info.specularTexture != -1) && params[info.specularTexture]->IsTexture();
		switches.bHasParallaxCorrection = info.m_nEnvmapParallax != -1 && params[info.m_nEnvmapParallax]->GetIntValue() != 0;

		switches.bUseParallax = params[info.useParallax]->GetIntValue();
		//if ( !mat_pbr_parallaxmap.GetBool() || !g_pHardwareConfig->HasFastVertexTextures() || mat_pbr_force_20b.GetBool() )
		//{
		//	switches.bUseParallax = 0;
		//}

		// Determining whether we're dealing with a fully opaque material
		switches.nBlendType = EvaluateBlendRequirements(info.baseTexture, true);
		switches.bFullyOpaque = true;// (switches.nBlendType != BT_BLENDADD) && (switches.nBlendType != BT_BLEND) && !switches.bIsAlphaTested;
		switches.nShadowFilterMode = switches.bHasFlashlight ? g_pHardwareConfig->GetShadowFilterMode() : 0;
		
		DrawPbrPass_Normal(this, params, pShaderAPI, pShaderShadow, info, switches, pContextDataPtr);

		if (g_pConfig->m_bPaintInGame) {
			DrawPbrPass_Paint(this, params, pShaderAPI, pShaderShadow, info, switches, pContextDataPtr);
		}
		else {
			Draw(false);
		}

		
	};

	// for part of the pbr pass
	static void _BindLightmapBumped() {
		s_pShaderAPI->BindStandardTexture(SAMPLER_LIGHTMAP, 0, TEXTURE_LIGHTMAP_BUMPED);
	}

// Closing it off
END_SHADER;

// NOTE: i know this is stupid and hacky, but it works and it works good. pls leave it alone.
void BindLightmapBumped() {
	PBR::CShader::_BindLightmapBumped();
}