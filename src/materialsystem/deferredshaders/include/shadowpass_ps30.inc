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
class shadowpass_ps30_Static_Index
{
	unsigned int m_nALPHATEST : 2;
	unsigned int m_nMULTITEXTURE : 2;
#ifdef _DEBUG
	bool m_bALPHATEST : 1;
	bool m_bMULTITEXTURE : 1;
#endif	// _DEBUG
public:
	void SetALPHATEST( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nALPHATEST = i;
#ifdef _DEBUG
		m_bALPHATEST = true;
#endif	// _DEBUG
	}

	void SetMULTITEXTURE( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nMULTITEXTURE = i;
#ifdef _DEBUG
		m_bMULTITEXTURE = true;
#endif	// _DEBUG
	}

	shadowpass_ps30_Static_Index( IShaderShadow* pShaderShadow, IMaterialVar** params )
	{
		m_nALPHATEST = 0;
		m_nMULTITEXTURE = 0;
#ifdef _DEBUG
		m_bALPHATEST = false;
		m_bMULTITEXTURE = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bALPHATEST && m_bMULTITEXTURE );
		return ( 6 * m_nALPHATEST ) + ( 12 * m_nMULTITEXTURE ) + 0;
	}
};

#define shaderStaticTest_shadowpass_ps30 psh_forgot_to_set_static_ALPHATEST + psh_forgot_to_set_static_MULTITEXTURE


class shadowpass_ps30_Dynamic_Index
{
	unsigned int m_nSHADOW_MODE : 2;
	unsigned int m_nRADIOSITY : 2;
#ifdef _DEBUG
	bool m_bSHADOW_MODE : 1;
	bool m_bRADIOSITY : 1;
#endif	// _DEBUG
public:
	void SetSHADOW_MODE( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nSHADOW_MODE = i;
#ifdef _DEBUG
		m_bSHADOW_MODE = true;
#endif	// _DEBUG
	}

	void SetRADIOSITY( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nRADIOSITY = i;
#ifdef _DEBUG
		m_bRADIOSITY = true;
#endif	// _DEBUG
	}

	shadowpass_ps30_Dynamic_Index( IShaderDynamicAPI* pShaderAPI )
	{
		m_nSHADOW_MODE = 0;
		m_nRADIOSITY = 0;
#ifdef _DEBUG
		m_bSHADOW_MODE = false;
		m_bRADIOSITY = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bSHADOW_MODE && m_bRADIOSITY );
		return ( 1 * m_nSHADOW_MODE ) + ( 3 * m_nRADIOSITY ) + 0;
	}
};

#define shaderDynamicTest_shadowpass_ps30 psh_forgot_to_set_dynamic_SHADOW_MODE + psh_forgot_to_set_dynamic_RADIOSITY

