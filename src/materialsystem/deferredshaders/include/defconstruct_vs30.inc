// ALL SKIP STATEMENTS THAT AFFECT THIS SHADER!!!
// !$MODEL && $MORPHING_VTEX
// !$MODEL && $COMPRESSED_VERTS
// !$MODEL && $SKINNING
// !$MODEL && $MORPHING_VTEX
// defined $LIGHTING_PREVIEW && defined $FASTPATH && $LIGHTING_PREVIEW && $FASTPATH

#pragma once
#include "shaderlib/cshader.h"
class defconstruct_vs30_Static_Index
{
	unsigned int m_nUSEWORLDTRANSFORM : 2;
	unsigned int m_nSENDWORLDPOS : 2;
#ifdef _DEBUG
	bool m_bUSEWORLDTRANSFORM : 1;
	bool m_bSENDWORLDPOS : 1;
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

	void SetSENDWORLDPOS( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nSENDWORLDPOS = i;
#ifdef _DEBUG
		m_bSENDWORLDPOS = true;
#endif	// _DEBUG
	}

	defconstruct_vs30_Static_Index( IShaderShadow* pShaderShadow, IMaterialVar** params )
	{
		m_nUSEWORLDTRANSFORM = 0;
		m_nSENDWORLDPOS = 0;
#ifdef _DEBUG
		m_bUSEWORLDTRANSFORM = false;
		m_bSENDWORLDPOS = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bUSEWORLDTRANSFORM && m_bSENDWORLDPOS );
		return ( 1 * m_nUSEWORLDTRANSFORM ) + ( 2 * m_nSENDWORLDPOS ) + 0;
	}
};

#define shaderStaticTest_defconstruct_vs30 vsh_forgot_to_set_static_USEWORLDTRANSFORM + vsh_forgot_to_set_static_SENDWORLDPOS


class defconstruct_vs30_Dynamic_Index
{
public:
	defconstruct_vs30_Dynamic_Index( IShaderDynamicAPI* pShaderAPI )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderDynamicTest_defconstruct_vs30 1

