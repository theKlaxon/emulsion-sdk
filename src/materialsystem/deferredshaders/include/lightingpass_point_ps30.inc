// ALL SKIP STATEMENTS THAT AFFECT THIS SHADER!!!
// !$NUM_SHADOWED_COOKIE && !$NUM_SHADOWED && !$NUM_COOKIE && !$NUM_SIMPLE
// $USEWORLDTRANSFORM && $NUM_SHADOWED_COOKIE && $NUM_SHADOWED
// $USEWORLDTRANSFORM && $NUM_SHADOWED_COOKIE && $NUM_COOKIE
// $USEWORLDTRANSFORM && $NUM_SHADOWED_COOKIE && $NUM_SIMPLE
// $USEWORLDTRANSFORM && $NUM_SHADOWED && $NUM_COOKIE
// $USEWORLDTRANSFORM && $NUM_SHADOWED && $NUM_SIMPLE
// $USEWORLDTRANSFORM && $NUM_COOKIE && $NUM_SIMPLE
// $USEWORLDTRANSFORM && $NUM_SHADOWED_COOKIE > 1
// $USEWORLDTRANSFORM && $NUM_SHADOWED > 1
// $USEWORLDTRANSFORM && $NUM_COOKIE > 1
// $USEWORLDTRANSFORM && $NUM_SIMPLE > 1
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
class lightingpass_point_ps30_Static_Index
{
	unsigned int m_nUSEWORLDTRANSFORM : 2;
#ifdef _DEBUG
	bool m_bUSEWORLDTRANSFORM : 1;
#endif	// _DEBUG
public:
	void SetUSEWORLDTRANSFORM( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nUSEWORLDTRANSFORM = i;
#ifdef _DEBUG
		m_bUSEWORLDTRANSFORM = true;
#endif	// _DEBUG
	}

	lightingpass_point_ps30_Static_Index( IShaderShadow* pShaderShadow, IMaterialVar** params )
	{
		m_nUSEWORLDTRANSFORM = 0;
#ifdef _DEBUG
		m_bUSEWORLDTRANSFORM = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bUSEWORLDTRANSFORM );
		return ( 528 * m_nUSEWORLDTRANSFORM ) + 0;
	}
};

#define shaderStaticTest_lightingpass_point_ps30 psh_forgot_to_set_static_USEWORLDTRANSFORM


class lightingpass_point_ps30_Dynamic_Index
{
	unsigned int m_nNUM_SHADOWED_COOKIE : 2;
	unsigned int m_nNUM_SHADOWED : 3;
	unsigned int m_nNUM_COOKIE : 3;
	unsigned int m_nNUM_SIMPLE : 4;
#ifdef _DEBUG
	bool m_bNUM_SHADOWED_COOKIE : 1;
	bool m_bNUM_SHADOWED : 1;
	bool m_bNUM_COOKIE : 1;
	bool m_bNUM_SIMPLE : 1;
#endif	// _DEBUG
public:
	void SetNUM_SHADOWED_COOKIE( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nNUM_SHADOWED_COOKIE = i;
#ifdef _DEBUG
		m_bNUM_SHADOWED_COOKIE = true;
#endif	// _DEBUG
	}

	void SetNUM_SHADOWED( int i )
	{
		Assert( i >= 0 && i <= 3 );
		m_nNUM_SHADOWED = i;
#ifdef _DEBUG
		m_bNUM_SHADOWED = true;
#endif	// _DEBUG
	}

	void SetNUM_COOKIE( int i )
	{
		Assert( i >= 0 && i <= 3 );
		m_nNUM_COOKIE = i;
#ifdef _DEBUG
		m_bNUM_COOKIE = true;
#endif	// _DEBUG
	}

	void SetNUM_SIMPLE( int i )
	{
		Assert( i >= 0 && i <= 10 );
		m_nNUM_SIMPLE = i;
#ifdef _DEBUG
		m_bNUM_SIMPLE = true;
#endif	// _DEBUG
	}

	lightingpass_point_ps30_Dynamic_Index( IShaderDynamicAPI* pShaderAPI )
	{
		m_nNUM_SHADOWED_COOKIE = 0;
		m_nNUM_SHADOWED = 0;
		m_nNUM_COOKIE = 0;
		m_nNUM_SIMPLE = 0;
#ifdef _DEBUG
		m_bNUM_SHADOWED_COOKIE = false;
		m_bNUM_SHADOWED = false;
		m_bNUM_COOKIE = false;
		m_bNUM_SIMPLE = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bNUM_SHADOWED_COOKIE && m_bNUM_SHADOWED && m_bNUM_COOKIE && m_bNUM_SIMPLE );
		AssertMsg( !( !m_nNUM_SHADOWED_COOKIE && ( !m_nNUM_SHADOWED && ( !m_nNUM_COOKIE && !m_nNUM_SIMPLE ) ) ), "Invalid combo combination ( !NUM_SHADOWED_COOKIE && ( !NUM_SHADOWED && ( !NUM_COOKIE && !NUM_SIMPLE ) ) )" );
		return ( 1 * m_nNUM_SHADOWED_COOKIE ) + ( 3 * m_nNUM_SHADOWED ) + ( 12 * m_nNUM_COOKIE ) + ( 48 * m_nNUM_SIMPLE ) + 0;
	}
};

#define shaderDynamicTest_lightingpass_point_ps30 psh_forgot_to_set_dynamic_NUM_SHADOWED_COOKIE + psh_forgot_to_set_dynamic_NUM_SHADOWED + psh_forgot_to_set_dynamic_NUM_COOKIE + psh_forgot_to_set_dynamic_NUM_SIMPLE

