// ALL SKIP STATEMENTS THAT AFFECT THIS SHADER!!!
// !$NUM_SHADOWED_COOKIE && !$NUM_SHADOWED
// $NUM_SHADOWED_COOKIE && $NUM_SHADOWED
// defined $PIXELFOGTYPE && defined $WRITEWATERFOGTODESTALPHA && ( $PIXELFOGTYPE != 1 ) && $WRITEWATERFOGTODESTALPHA
// defined $LIGHTING_PREVIEW && defined $FASTPATHENVMAPTINT && $LIGHTING_PREVIEW && $FASTPATHENVMAPTINT
// defined $LIGHTING_PREVIEW && defined $FASTPATHENVMAPCONTRAST && $LIGHTING_PREVIEW && $FASTPATHENVMAPCONTRAST
// defined $LIGHTING_PREVIEW && defined $FASTPATH && $LIGHTING_PREVIEW && $FASTPATH
// ($FLASHLIGHT || $FLASHLIGHTSHADOWS) && $LIGHTING_PREVIEW

#pragma once
#include "shaderlib/cshader.h"
class volumpass_spot_ps30_Static_Index
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

	volumpass_spot_ps30_Static_Index( IShaderShadow* pShaderShadow, IMaterialVar** params )
	{
		m_nUSEWORLDTRANSFORM = 0;
#ifdef _DEBUG
		m_bUSEWORLDTRANSFORM = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bUSEWORLDTRANSFORM );
		return ( 2000 * m_nUSEWORLDTRANSFORM ) + 0;
	}
};

#define shaderStaticTest_volumpass_spot_ps30 psh_forgot_to_set_static_USEWORLDTRANSFORM


class volumpass_spot_ps30_Dynamic_Index
{
	unsigned int m_nNUM_SHADOWED_COOKIE : 2;
	unsigned int m_nNUM_SHADOWED : 2;
	unsigned int m_nVOLUME_LOD : 3;
	unsigned int m_nVOLUME_SAMPLES : 7;
#ifdef _DEBUG
	bool m_bNUM_SHADOWED_COOKIE : 1;
	bool m_bNUM_SHADOWED : 1;
	bool m_bVOLUME_LOD : 1;
	bool m_bVOLUME_SAMPLES : 1;
#endif	// _DEBUG
public:
	void SetNUM_SHADOWED_COOKIE( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nNUM_SHADOWED_COOKIE = i;
#ifdef _DEBUG
		m_bNUM_SHADOWED_COOKIE = true;
#endif	// _DEBUG
	}

	void SetNUM_SHADOWED( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nNUM_SHADOWED = i;
#ifdef _DEBUG
		m_bNUM_SHADOWED = true;
#endif	// _DEBUG
	}

	void SetVOLUME_LOD( int i )
	{
		Assert( i >= 0 && i <= 4 );
		m_nVOLUME_LOD = i;
#ifdef _DEBUG
		m_bVOLUME_LOD = true;
#endif	// _DEBUG
	}

	void SetVOLUME_SAMPLES( int i )
	{
		Assert( i >= 1 && i <= 100 );
		m_nVOLUME_SAMPLES = i - 1;
#ifdef _DEBUG
		m_bVOLUME_SAMPLES = true;
#endif	// _DEBUG
	}

	volumpass_spot_ps30_Dynamic_Index( IShaderDynamicAPI* pShaderAPI )
	{
		m_nNUM_SHADOWED_COOKIE = 0;
		m_nNUM_SHADOWED = 0;
		m_nVOLUME_LOD = 0;
		m_nVOLUME_SAMPLES = 0;
#ifdef _DEBUG
		m_bNUM_SHADOWED_COOKIE = false;
		m_bNUM_SHADOWED = false;
		m_bVOLUME_LOD = false;
		m_bVOLUME_SAMPLES = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bNUM_SHADOWED_COOKIE && m_bNUM_SHADOWED && m_bVOLUME_LOD && m_bVOLUME_SAMPLES );
		AssertMsg( !( !m_nNUM_SHADOWED_COOKIE && !m_nNUM_SHADOWED ), "Invalid combo combination ( !NUM_SHADOWED_COOKIE && !NUM_SHADOWED )" );
		AssertMsg( !( m_nNUM_SHADOWED_COOKIE && m_nNUM_SHADOWED ), "Invalid combo combination ( NUM_SHADOWED_COOKIE && NUM_SHADOWED )" );
		return ( 1 * m_nNUM_SHADOWED_COOKIE ) + ( 2 * m_nNUM_SHADOWED ) + ( 4 * m_nVOLUME_LOD ) + ( 20 * m_nVOLUME_SAMPLES ) + 0;
	}
};

#define shaderDynamicTest_volumpass_spot_ps30 psh_forgot_to_set_dynamic_NUM_SHADOWED_COOKIE + psh_forgot_to_set_dynamic_NUM_SHADOWED + psh_forgot_to_set_dynamic_VOLUME_LOD + psh_forgot_to_set_dynamic_VOLUME_SAMPLES

