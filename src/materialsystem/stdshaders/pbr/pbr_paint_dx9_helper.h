//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#pragma once
#include "BaseVSShader.h"
#include "shaderlib/commandbuilder.h"

// Includes for PS30
#include "pbr_vs30.inc"


// Includes for PS20b
//#include "pbr_vs20.inc"


// Defining samplers
static const Sampler_t SAMPLER_BASETEXTURE = SHADER_SAMPLER0;
static const Sampler_t SAMPLER_NORMAL = SHADER_SAMPLER1;
static const Sampler_t SAMPLER_ENVMAP = SHADER_SAMPLER2;
static const Sampler_t SAMPLER_SHADOWDEPTH = SHADER_SAMPLER4;
static const Sampler_t SAMPLER_RANDOMROTATION = SHADER_SAMPLER5;
static const Sampler_t SAMPLER_FLASHLIGHT = SHADER_SAMPLER6;
static const Sampler_t SAMPLER_LIGHTMAP = SHADER_SAMPLER7;
//const Sampler_t SAMPLER_PAINTMAP = SHADER_SAMPLER8; // the use_paint flag2 var appears to be under the use_flashlight var, so in theory this is correct? -Klaxon
static const Sampler_t SAMPLER_MRAO = SHADER_SAMPLER10;
static const Sampler_t SAMPLER_EMISSIVE = SHADER_SAMPLER11;
static const Sampler_t SAMPLER_SPECULAR = SHADER_SAMPLER12;

// paint samplers (only in paint pass)
static const Sampler_t SAMPLER_SPLATNORMAL = SHADER_SAMPLER13;
static const Sampler_t SAMPLER_PAINT = SHADER_SAMPLER14;

// Convars
static ConVar mat_fullbright("mat_fullbright", "0", FCVAR_CHEAT);
static ConVar mat_specular("mat_specular", "1");
static ConVar mat_pbr_force_20b("mat_pbr_force_20b", "0", FCVAR_CHEAT);
static ConVar mat_pbr_parallaxmap("mat_pbr_parallaxmap", "1");
static ConVar mat_allow_parallax_cubemaps("mat_allow_parallax_cubemaps", "0"); // not a thing in emulson -- yet :)

// Variables for this shader
struct PBR_Vars_t
{
	PBR_Vars_t()
	{
		memset(this, 0xFF, sizeof(*this));
	}

	int baseTexture;
	int baseColor;
	int normalTexture;
	int bumpMap;
	int envMap;
	int baseTextureFrame;
	int baseTextureTransform;
	int useParallax;
	int parallaxDepth;
	int parallaxCenter;
	int alphaTestReference;
	int flashlightTexture;
	int flashlightTextureFrame;
	int emissionTexture;
	int mraoTexture;
	int useEnvAmbient;
	int specularTexture;

	int m_nEnvmapParallax;
	int m_nEnvmapParallaxObb1;
	int m_nEnvmapParallaxObb2;
	int m_nEnvmapParallaxObb3;
	int m_nEnvmapOrigin;

	// SCIENCE!
	// portal2 paint -- names correspond to texture names in InitLightmappedGeneric_DX9. 
	int m_nPaintSplatNormal; // asw -- assuming these 2 remained the same?
	int m_nPaintSplatEnvMap; // ^^

	int m_nPaintBubble;
	int m_nPaintBubbleLayout;
	int m_nPaintMRAO;

	int nVertexCompression;
};

struct PBR_Switches_t {
	bool bHasBaseTexture;
	bool bHasNormalTexture;
	bool bHasMraoTexture;
	bool bHasEmissionTexture;
	bool bHasEnvTexture;
	bool bIsAlphaTested;
	bool bHasFlashlight;
	bool bHasColor;
	bool bLightMapped; 
	bool bUseEnvAmbient;
	bool bHasSpecularTexture; 
	bool bHasParallaxCorrection; 
	int bUseParallax;
	bool bFullyOpaque;
	int nShadowFilterMode;
	BlendType_t nBlendType;
};

class CPaintContextData : public CBasePerMaterialContextData {
public:

	uint8* m_pStaticCmds;
	CCommandBufferBuilder< CFixedCommandStorageBuffer< 900 > > m_SemiStaticCmdsOut;
	LightState_t lightState;

	int vertexCompress;
	int numBones;
	int fogIndex;
	int renderParam;

	CPaintContextData(void)
	{
		m_pStaticCmds = NULL;
	}

	~CPaintContextData(void)
	{
		ResetStaticCmds();
	}

	void ResetStaticCmds(void)
	{
		if (m_pStaticCmds)
		{
			delete[] m_pStaticCmds;
			m_pStaticCmds = NULL;
		}
	}
};

void BindLightmapBumped();

// the standard pbr pass
void DrawPbrPass_Normal(CBaseVSShader* pShader, IMaterialVar** params, IShaderDynamicAPI* pShaderAPI, IShaderShadow* pShaderShadow,
	PBR_Vars_t& info, PBR_Switches_t const& switches, CBasePerMaterialContextData** pContextDataPtr);

// the paint pbr pass
void DrawPbrPass_Paint(CBaseVSShader* pShader, IMaterialVar** params, IShaderDynamicAPI* pShaderAPI, IShaderShadow* pShaderShadow,
	PBR_Vars_t& info, PBR_Switches_t const& switches, CBasePerMaterialContextData** pContextDataPtr);