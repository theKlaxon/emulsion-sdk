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
class radiosity_gen_global_ps30_Static_Index
{
public:
	radiosity_gen_global_ps30_Static_Index( IShaderShadow* pShaderShadow, IMaterialVar** params )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderStaticTest_radiosity_gen_global_ps30 1


class radiosity_gen_global_ps30_Dynamic_Index
{
	unsigned int m_nCASCADE : 2;
#ifdef _DEBUG
	bool m_bCASCADE : 1;
#endif	// _DEBUG
public:
	void SetCASCADE( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nCASCADE = i;
#ifdef _DEBUG
		m_bCASCADE = true;
#endif	// _DEBUG
	}

	radiosity_gen_global_ps30_Dynamic_Index( IShaderDynamicAPI* pShaderAPI )
	{
		m_nCASCADE = 0;
#ifdef _DEBUG
		m_bCASCADE = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bCASCADE );
		return ( 1 * m_nCASCADE ) + 0;
	}
};

#define shaderDynamicTest_radiosity_gen_global_ps30 psh_forgot_to_set_dynamic_CASCADE

