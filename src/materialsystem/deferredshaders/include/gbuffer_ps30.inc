// ALL SKIP STATEMENTS THAT AFFECT THIS SHADER!!!
// !$BUMPMAP2 && !$MULTIBLEND && $BLENDMODULATE
// !$BUMPMAP && $BUMPMAP2
// $MODEL && $BUMPMAP2
// $MODEL && $MULTIBLEND
// $BUMPMAP2 && $MULTIBLEND
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
class gbuffer_ps30_Static_Index
{
	unsigned int m_nALPHATEST : 2;
	unsigned int m_nBUMPMAP : 2;
	unsigned int m_nNOCULL : 2;
	unsigned int m_nPHONGMAP : 2;
	unsigned int m_nBUMPMAP2 : 2;
	unsigned int m_nBLENDMODULATE : 2;
	unsigned int m_nMULTIBLEND : 2;
#ifdef _DEBUG
	bool m_bALPHATEST : 1;
	bool m_bBUMPMAP : 1;
	bool m_bNOCULL : 1;
	bool m_bPHONGMAP : 1;
	bool m_bBUMPMAP2 : 1;
	bool m_bBLENDMODULATE : 1;
	bool m_bMULTIBLEND : 1;
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

	void SetBUMPMAP2( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nBUMPMAP2 = i;
#ifdef _DEBUG
		m_bBUMPMAP2 = true;
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

	gbuffer_ps30_Static_Index( IShaderShadow* pShaderShadow, IMaterialVar** params )
	{
		m_nALPHATEST = 0;
		m_nBUMPMAP = 0;
		m_nNOCULL = 0;
		m_nPHONGMAP = 0;
		m_nBUMPMAP2 = 0;
		m_nBLENDMODULATE = 0;
		m_nMULTIBLEND = 0;
#ifdef _DEBUG
		m_bALPHATEST = false;
		m_bBUMPMAP = false;
		m_bNOCULL = false;
		m_bPHONGMAP = false;
		m_bBUMPMAP2 = false;
		m_bBLENDMODULATE = false;
		m_bMULTIBLEND = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bALPHATEST && m_bBUMPMAP && m_bNOCULL && m_bPHONGMAP && m_bBUMPMAP2 && m_bBLENDMODULATE && m_bMULTIBLEND );
		AssertMsg( !( !m_nBUMPMAP2 && ( !m_nMULTIBLEND && m_nBLENDMODULATE ) ), "Invalid combo combination ( !BUMPMAP2 && ( !MULTIBLEND && BLENDMODULATE ) )" );
		AssertMsg( !( !m_nBUMPMAP && m_nBUMPMAP2 ), "Invalid combo combination ( !BUMPMAP && BUMPMAP2 )" );
		AssertMsg( !( m_nBUMPMAP2 && m_nMULTIBLEND ), "Invalid combo combination ( BUMPMAP2 && MULTIBLEND )" );
		return ( 1 * m_nALPHATEST ) + ( 2 * m_nBUMPMAP ) + ( 6 * m_nNOCULL ) + ( 12 * m_nPHONGMAP ) + ( 24 * m_nBUMPMAP2 ) + ( 48 * m_nBLENDMODULATE ) + ( 96 * m_nMULTIBLEND ) + 0;
	}
};

#define shaderStaticTest_gbuffer_ps30 psh_forgot_to_set_static_ALPHATEST + psh_forgot_to_set_static_BUMPMAP + psh_forgot_to_set_static_NOCULL + psh_forgot_to_set_static_PHONGMAP + psh_forgot_to_set_static_BUMPMAP2 + psh_forgot_to_set_static_BLENDMODULATE + psh_forgot_to_set_static_MULTIBLEND


class gbuffer_ps30_Dynamic_Index
{
public:
	gbuffer_ps30_Dynamic_Index( IShaderDynamicAPI* pShaderAPI )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderDynamicTest_gbuffer_ps30 1

