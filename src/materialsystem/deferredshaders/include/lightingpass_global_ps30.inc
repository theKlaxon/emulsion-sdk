// ALL SKIP STATEMENTS THAT AFFECT THIS SHADER!!!
// defined $PIXELFOGTYPE && defined $WRITEWATERFOGTODESTALPHA && ( $PIXELFOGTYPE != 1 ) && $WRITEWATERFOGTODESTALPHA
// defined $LIGHTING_PREVIEW && defined $FASTPATHENVMAPTINT && $LIGHTING_PREVIEW && $FASTPATHENVMAPTINT
// defined $LIGHTING_PREVIEW && defined $FASTPATHENVMAPCONTRAST && $LIGHTING_PREVIEW && $FASTPATHENVMAPCONTRAST
// defined $LIGHTING_PREVIEW && defined $FASTPATH && $LIGHTING_PREVIEW && $FASTPATH
// ($FLASHLIGHT || $FLASHLIGHTSHADOWS) && $LIGHTING_PREVIEW
// !$MODEL && $MORPHING_VTEX
// !$MODEL && $COMPRESSED_VERTS
// !$MODEL && $SKINNING
// !$MODEL && $MORPHING_VTEX

#pragma once
#include "shaderlib/cshader.h"
class lightingpass_global_ps30_Static_Index
{
public:
	lightingpass_global_ps30_Static_Index( IShaderShadow* pShaderShadow, IMaterialVar** params )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderStaticTest_lightingpass_global_ps30 1


class lightingpass_global_ps30_Dynamic_Index
{
	unsigned int m_nHAS_SHADOW : 2;
#ifdef _DEBUG
	bool m_bHAS_SHADOW : 1;
#endif	// _DEBUG
public:
	void SetHAS_SHADOW( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nHAS_SHADOW = i;
#ifdef _DEBUG
		m_bHAS_SHADOW = true;
#endif	// _DEBUG
	}

	lightingpass_global_ps30_Dynamic_Index( IShaderDynamicAPI* pShaderAPI )
	{
		m_nHAS_SHADOW = 0;
#ifdef _DEBUG
		m_bHAS_SHADOW = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bHAS_SHADOW );
		return ( 1 * m_nHAS_SHADOW ) + 0;
	}
};

#define shaderDynamicTest_lightingpass_global_ps30 psh_forgot_to_set_dynamic_HAS_SHADOW

