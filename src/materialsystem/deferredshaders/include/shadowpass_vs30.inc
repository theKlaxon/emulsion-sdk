// ALL SKIP STATEMENTS THAT AFFECT THIS SHADER!!!
// defined $LIGHTING_PREVIEW && defined $FASTPATH && $LIGHTING_PREVIEW && $FASTPATH
// !$MODEL && $MORPHING_VTEX
// !$MODEL && $COMPRESSED_VERTS
// !$MODEL && $SKINNING
// !$MODEL && $MORPHING_VTEX

#pragma once
#include "shaderlib/cshader.h"
class shadowpass_vs30_Static_Index
{
	unsigned int m_nMODEL : 2;
	unsigned int m_nMORPHING_VTEX : 2;
	unsigned int m_nMULTITEXTURE : 2;
#ifdef _DEBUG
	bool m_bMODEL : 1;
	bool m_bMORPHING_VTEX : 1;
	bool m_bMULTITEXTURE : 1;
#endif	// _DEBUG
public:
	void SetMODEL( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nMODEL = i;
#ifdef _DEBUG
		m_bMODEL = true;
#endif	// _DEBUG
	}

	void SetMORPHING_VTEX( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nMORPHING_VTEX = i;
#ifdef _DEBUG
		m_bMORPHING_VTEX = true;
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

	shadowpass_vs30_Static_Index( IShaderShadow* pShaderShadow, IMaterialVar** params )
	{
		m_nMODEL = 0;
		m_nMORPHING_VTEX = 0;
		m_nMULTITEXTURE = 0;
#ifdef _DEBUG
		m_bMODEL = false;
		m_bMORPHING_VTEX = false;
		m_bMULTITEXTURE = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bMODEL && m_bMORPHING_VTEX && m_bMULTITEXTURE );
		AssertMsg( !( !m_nMODEL && m_nMORPHING_VTEX ), "Invalid combo combination ( !MODEL && MORPHING_VTEX )" );
		AssertMsg( !( !m_nMODEL && m_nMORPHING_VTEX ), "Invalid combo combination ( !MODEL && MORPHING_VTEX )" );
		return ( 48 * m_nMODEL ) + ( 96 * m_nMORPHING_VTEX ) + ( 192 * m_nMULTITEXTURE ) + 0;
	}
};

#define shaderStaticTest_shadowpass_vs30 vsh_forgot_to_set_static_MODEL + vsh_forgot_to_set_static_MORPHING_VTEX + vsh_forgot_to_set_static_MULTITEXTURE


class shadowpass_vs30_Dynamic_Index
{
	unsigned int m_nCOMPRESSED_VERTS : 2;
	unsigned int m_nSKINNING : 2;
	unsigned int m_nMORPHING : 2;
	unsigned int m_nSHADOW_MODE : 2;
	unsigned int m_nRADIOSITY : 2;
#ifdef _DEBUG
	bool m_bCOMPRESSED_VERTS : 1;
	bool m_bSKINNING : 1;
	bool m_bMORPHING : 1;
	bool m_bSHADOW_MODE : 1;
	bool m_bRADIOSITY : 1;
#endif	// _DEBUG
public:
	void SetCOMPRESSED_VERTS( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nCOMPRESSED_VERTS = i;
#ifdef _DEBUG
		m_bCOMPRESSED_VERTS = true;
#endif	// _DEBUG
	}

	void SetSKINNING( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nSKINNING = i;
#ifdef _DEBUG
		m_bSKINNING = true;
#endif	// _DEBUG
	}

	void SetMORPHING( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nMORPHING = i;
#ifdef _DEBUG
		m_bMORPHING = true;
#endif	// _DEBUG
	}

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

	shadowpass_vs30_Dynamic_Index( IShaderDynamicAPI* pShaderAPI )
	{
		m_nCOMPRESSED_VERTS = 0;
		m_nSKINNING = 0;
		m_nMORPHING = 0;
		m_nSHADOW_MODE = 0;
		m_nRADIOSITY = 0;
#ifdef _DEBUG
		m_bCOMPRESSED_VERTS = false;
		m_bSKINNING = false;
		m_bMORPHING = false;
		m_bSHADOW_MODE = false;
		m_bRADIOSITY = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bCOMPRESSED_VERTS && m_bSKINNING && m_bMORPHING && m_bSHADOW_MODE && m_bRADIOSITY );
		return ( 1 * m_nCOMPRESSED_VERTS ) + ( 2 * m_nSKINNING ) + ( 4 * m_nMORPHING ) + ( 8 * m_nSHADOW_MODE ) + ( 24 * m_nRADIOSITY ) + 0;
	}
};

#define shaderDynamicTest_shadowpass_vs30 vsh_forgot_to_set_dynamic_COMPRESSED_VERTS + vsh_forgot_to_set_dynamic_SKINNING + vsh_forgot_to_set_dynamic_MORPHING + vsh_forgot_to_set_dynamic_SHADOW_MODE + vsh_forgot_to_set_dynamic_RADIOSITY

