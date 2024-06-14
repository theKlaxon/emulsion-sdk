// ALL SKIP STATEMENTS THAT AFFECT THIS SHADER!!!
// !$TWOTEXTURE && !$MULTIBLEND && $BLENDMODULATE
// $MODEL && $TWOTEXTURE
// $MODEL && $MULTIBLEND
// $TWOTEXTURE && $MULTIBLEND
// $DECAL && $MULTIBLEND
// $DECAL && $BLENDMODULATE
// $DECAL && $BUMPMAP
// $DECAL && $NOCULL
// $DECAL && $PHONGMAP
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
class gbuffer_defshading_ps30_Static_Index
{
	unsigned int m_nALPHATEST : 2;
	unsigned int m_nBUMPMAP : 2;
	unsigned int m_nNOCULL : 2;
	unsigned int m_nPHONGMAP : 2;
	unsigned int m_nTWOTEXTURE : 2;
	unsigned int m_nBLENDMODULATE : 2;
	unsigned int m_nMULTIBLEND : 2;
	unsigned int m_nDECAL : 2;
#ifdef _DEBUG
	bool m_bALPHATEST : 1;
	bool m_bBUMPMAP : 1;
	bool m_bNOCULL : 1;
	bool m_bPHONGMAP : 1;
	bool m_bTWOTEXTURE : 1;
	bool m_bBLENDMODULATE : 1;
	bool m_bMULTIBLEND : 1;
	bool m_bDECAL : 1;
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

	void SetBUMPMAP( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nBUMPMAP = i;
#ifdef _DEBUG
		m_bBUMPMAP = true;
#endif	// _DEBUG
	}

	void SetNOCULL( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nNOCULL = i;
#ifdef _DEBUG
		m_bNOCULL = true;
#endif	// _DEBUG
	}

	void SetPHONGMAP( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nPHONGMAP = i;
#ifdef _DEBUG
		m_bPHONGMAP = true;
#endif	// _DEBUG
	}

	void SetTWOTEXTURE( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nTWOTEXTURE = i;
#ifdef _DEBUG
		m_bTWOTEXTURE = true;
#endif	// _DEBUG
	}

	void SetBLENDMODULATE( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nBLENDMODULATE = i;
#ifdef _DEBUG
		m_bBLENDMODULATE = true;
#endif	// _DEBUG
	}

	void SetMULTIBLEND( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nMULTIBLEND = i;
#ifdef _DEBUG
		m_bMULTIBLEND = true;
#endif	// _DEBUG
	}

	void SetDECAL( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nDECAL = i;
#ifdef _DEBUG
		m_bDECAL = true;
#endif	// _DEBUG
	}

	gbuffer_defshading_ps30_Static_Index( IShaderShadow* pShaderShadow, IMaterialVar** params )
	{
		m_nALPHATEST = 0;
		m_nBUMPMAP = 0;
		m_nNOCULL = 0;
		m_nPHONGMAP = 0;
		m_nTWOTEXTURE = 0;
		m_nBLENDMODULATE = 0;
		m_nMULTIBLEND = 0;
		m_nDECAL = 0;
#ifdef _DEBUG
		m_bALPHATEST = false;
		m_bBUMPMAP = false;
		m_bNOCULL = false;
		m_bPHONGMAP = false;
		m_bTWOTEXTURE = false;
		m_bBLENDMODULATE = false;
		m_bMULTIBLEND = false;
		m_bDECAL = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bALPHATEST && m_bBUMPMAP && m_bNOCULL && m_bPHONGMAP && m_bTWOTEXTURE && m_bBLENDMODULATE && m_bMULTIBLEND && m_bDECAL );
		AssertMsg( !( !m_nTWOTEXTURE && ( !m_nMULTIBLEND && m_nBLENDMODULATE ) ), "Invalid combo combination ( !TWOTEXTURE && ( !MULTIBLEND && BLENDMODULATE ) )" );
		AssertMsg( !( m_nTWOTEXTURE && m_nMULTIBLEND ), "Invalid combo combination ( TWOTEXTURE && MULTIBLEND )" );
		AssertMsg( !( m_nDECAL && m_nMULTIBLEND ), "Invalid combo combination ( DECAL && MULTIBLEND )" );
		AssertMsg( !( m_nDECAL && m_nBLENDMODULATE ), "Invalid combo combination ( DECAL && BLENDMODULATE )" );
		AssertMsg( !( m_nDECAL && m_nBUMPMAP ), "Invalid combo combination ( DECAL && BUMPMAP )" );
		AssertMsg( !( m_nDECAL && m_nNOCULL ), "Invalid combo combination ( DECAL && NOCULL )" );
		AssertMsg( !( m_nDECAL && m_nPHONGMAP ), "Invalid combo combination ( DECAL && PHONGMAP )" );
		return ( 1 * m_nALPHATEST ) + ( 2 * m_nBUMPMAP ) + ( 6 * m_nNOCULL ) + ( 12 * m_nPHONGMAP ) + ( 24 * m_nTWOTEXTURE ) + ( 48 * m_nBLENDMODULATE ) + ( 96 * m_nMULTIBLEND ) + ( 192 * m_nDECAL ) + 0;
	}
};

#define shaderStaticTest_gbuffer_defshading_ps30 psh_forgot_to_set_static_ALPHATEST + psh_forgot_to_set_static_BUMPMAP + psh_forgot_to_set_static_NOCULL + psh_forgot_to_set_static_PHONGMAP + psh_forgot_to_set_static_TWOTEXTURE + psh_forgot_to_set_static_BLENDMODULATE + psh_forgot_to_set_static_MULTIBLEND + psh_forgot_to_set_static_DECAL


class gbuffer_defshading_ps30_Dynamic_Index
{
public:
	gbuffer_defshading_ps30_Dynamic_Index( IShaderDynamicAPI* pShaderAPI )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderDynamicTest_gbuffer_defshading_ps30 1

