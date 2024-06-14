// ALL SKIP STATEMENTS THAT AFFECT THIS SHADER!!!
// $ALPHATEST && $TRANSLUCENT
// !$ENVMAP && $ENVMAPFRESNEL
// !$ENVMAP && $ENVMAPMASK
// $ENVMAP && !$READNORMAL
// $ENVMAPFRESNEL && !$READNORMAL
// $PHONGFRESNEL && !$READNORMAL
// $RIMLIGHT && !$READNORMAL
// !$RIMLIGHT && $RIMLIGHTMODULATELIGHT
// !$BASETEXTURE2 && !$MULTIBLEND && $BLENDMODULATE
// $MODEL && $BASETEXTURE2
// $MODEL && $MULTIBLEND
// $BASETEXTURE2 && $MULTIBLEND
// $ENVMAPMASK && $MULTIBLEND
// $SELFILLUM_MASK && $MULTIBLEND
// !$SELFILLUM && $SELFILLUM_MASK
// !$SELFILLUM && $SELFILLUM_ENVMAP_ALPHA
// $SELFILLUM_MASK && $SELFILLUM_ENVMAP_ALPHA
// $SELFILLUM_MASK && $ENVMAPMASK
// $SELFILLUM_ENVMAP_ALPHA && !$ENVMAPMASK
// defined $PIXELFOGTYPE && defined $WRITEWATERFOGTODESTALPHA && ( $PIXELFOGTYPE != 1 ) && $WRITEWATERFOGTODESTALPHA
// defined $LIGHTING_PREVIEW && defined $FASTPATHENVMAPTINT && $LIGHTING_PREVIEW && $FASTPATHENVMAPTINT
// defined $LIGHTING_PREVIEW && defined $FASTPATHENVMAPCONTRAST && $LIGHTING_PREVIEW && $FASTPATHENVMAPCONTRAST
// defined $LIGHTING_PREVIEW && defined $FASTPATH && $LIGHTING_PREVIEW && $FASTPATH
// ($FLASHLIGHT || $FLASHLIGHTSHADOWS) && $LIGHTING_PREVIEW
// !$MODEL && $MORPHING_VTEX
// !$MODEL && $COMPRESSED_VERTS
// !$MODEL && $SKINNING
// !$MODEL && $MORPHING_VTEX
// defined $PIXELFOGTYPE && defined $WRITEWATERFOGTODESTALPHA && ( $PIXELFOGTYPE != 1 ) && $WRITEWATERFOGTODESTALPHA
// defined $LIGHTING_PREVIEW && defined $FASTPATHENVMAPTINT && $LIGHTING_PREVIEW && $FASTPATHENVMAPTINT
// defined $LIGHTING_PREVIEW && defined $FASTPATHENVMAPCONTRAST && $LIGHTING_PREVIEW && $FASTPATHENVMAPCONTRAST
// defined $LIGHTING_PREVIEW && defined $FASTPATH && $LIGHTING_PREVIEW && $FASTPATH
// ($FLASHLIGHT || $FLASHLIGHTSHADOWS) && $LIGHTING_PREVIEW

#pragma once
#include "shaderlib/cshader.h"
class composite_ps30_Static_Index
{
	unsigned int m_nALPHATEST : 2;
	unsigned int m_nTRANSLUCENT : 2;
	unsigned int m_nREADNORMAL : 2;
	unsigned int m_nNOCULL : 2;
	unsigned int m_nPHONGFRESNEL : 2;
	unsigned int m_nENVMAP : 2;
	unsigned int m_nENVMAPMASK : 2;
	unsigned int m_nENVMAPFRESNEL : 2;
	unsigned int m_nRIMLIGHT : 2;
	unsigned int m_nRIMLIGHTMODULATELIGHT : 2;
	unsigned int m_nBASETEXTURE2 : 2;
	unsigned int m_nBLENDMODULATE : 2;
	unsigned int m_nMULTIBLEND : 2;
	unsigned int m_nSELFILLUM : 2;
	unsigned int m_nSELFILLUM_MASK : 2;
	unsigned int m_nSELFILLUM_ENVMAP_ALPHA : 2;
#ifdef _DEBUG
	bool m_bALPHATEST : 1;
	bool m_bTRANSLUCENT : 1;
	bool m_bREADNORMAL : 1;
	bool m_bNOCULL : 1;
	bool m_bPHONGFRESNEL : 1;
	bool m_bENVMAP : 1;
	bool m_bENVMAPMASK : 1;
	bool m_bENVMAPFRESNEL : 1;
	bool m_bRIMLIGHT : 1;
	bool m_bRIMLIGHTMODULATELIGHT : 1;
	bool m_bBASETEXTURE2 : 1;
	bool m_bBLENDMODULATE : 1;
	bool m_bMULTIBLEND : 1;
	bool m_bSELFILLUM : 1;
	bool m_bSELFILLUM_MASK : 1;
	bool m_bSELFILLUM_ENVMAP_ALPHA : 1;
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

	void SetTRANSLUCENT( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nTRANSLUCENT = i;
#ifdef _DEBUG
		m_bTRANSLUCENT = true;
#endif	// _DEBUG
	}

	void SetREADNORMAL( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nREADNORMAL = i;
#ifdef _DEBUG
		m_bREADNORMAL = true;
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

	void SetPHONGFRESNEL( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nPHONGFRESNEL = i;
#ifdef _DEBUG
		m_bPHONGFRESNEL = true;
#endif	// _DEBUG
	}

	void SetENVMAP( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nENVMAP = i;
#ifdef _DEBUG
		m_bENVMAP = true;
#endif	// _DEBUG
	}

	void SetENVMAPMASK( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nENVMAPMASK = i;
#ifdef _DEBUG
		m_bENVMAPMASK = true;
#endif	// _DEBUG
	}

	void SetENVMAPFRESNEL( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nENVMAPFRESNEL = i;
#ifdef _DEBUG
		m_bENVMAPFRESNEL = true;
#endif	// _DEBUG
	}

	void SetRIMLIGHT( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nRIMLIGHT = i;
#ifdef _DEBUG
		m_bRIMLIGHT = true;
#endif	// _DEBUG
	}

	void SetRIMLIGHTMODULATELIGHT( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nRIMLIGHTMODULATELIGHT = i;
#ifdef _DEBUG
		m_bRIMLIGHTMODULATELIGHT = true;
#endif	// _DEBUG
	}

	void SetBASETEXTURE2( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nBASETEXTURE2 = i;
#ifdef _DEBUG
		m_bBASETEXTURE2 = true;
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

	void SetSELFILLUM( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nSELFILLUM = i;
#ifdef _DEBUG
		m_bSELFILLUM = true;
#endif	// _DEBUG
	}

	void SetSELFILLUM_MASK( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nSELFILLUM_MASK = i;
#ifdef _DEBUG
		m_bSELFILLUM_MASK = true;
#endif	// _DEBUG
	}

	void SetSELFILLUM_ENVMAP_ALPHA( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nSELFILLUM_ENVMAP_ALPHA = i;
#ifdef _DEBUG
		m_bSELFILLUM_ENVMAP_ALPHA = true;
#endif	// _DEBUG
	}

	composite_ps30_Static_Index( IShaderShadow* pShaderShadow, IMaterialVar** params )
	{
		m_nALPHATEST = 0;
		m_nTRANSLUCENT = 0;
		m_nREADNORMAL = 0;
		m_nNOCULL = 0;
		m_nPHONGFRESNEL = 0;
		m_nENVMAP = 0;
		m_nENVMAPMASK = 0;
		m_nENVMAPFRESNEL = 0;
		m_nRIMLIGHT = 0;
		m_nRIMLIGHTMODULATELIGHT = 0;
		m_nBASETEXTURE2 = 0;
		m_nBLENDMODULATE = 0;
		m_nMULTIBLEND = 0;
		m_nSELFILLUM = 0;
		m_nSELFILLUM_MASK = 0;
		m_nSELFILLUM_ENVMAP_ALPHA = 0;
#ifdef _DEBUG
		m_bALPHATEST = false;
		m_bTRANSLUCENT = false;
		m_bREADNORMAL = false;
		m_bNOCULL = false;
		m_bPHONGFRESNEL = false;
		m_bENVMAP = false;
		m_bENVMAPMASK = false;
		m_bENVMAPFRESNEL = false;
		m_bRIMLIGHT = false;
		m_bRIMLIGHTMODULATELIGHT = false;
		m_bBASETEXTURE2 = false;
		m_bBLENDMODULATE = false;
		m_bMULTIBLEND = false;
		m_bSELFILLUM = false;
		m_bSELFILLUM_MASK = false;
		m_bSELFILLUM_ENVMAP_ALPHA = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bALPHATEST && m_bTRANSLUCENT && m_bREADNORMAL && m_bNOCULL && m_bPHONGFRESNEL && m_bENVMAP && m_bENVMAPMASK && m_bENVMAPFRESNEL && m_bRIMLIGHT && m_bRIMLIGHTMODULATELIGHT && m_bBASETEXTURE2 && m_bBLENDMODULATE && m_bMULTIBLEND && m_bSELFILLUM && m_bSELFILLUM_MASK && m_bSELFILLUM_ENVMAP_ALPHA );
		AssertMsg( !( m_nALPHATEST && m_nTRANSLUCENT ), "Invalid combo combination ( ALPHATEST && TRANSLUCENT )" );
		AssertMsg( !( !m_nENVMAP && m_nENVMAPFRESNEL ), "Invalid combo combination ( !ENVMAP && ENVMAPFRESNEL )" );
		AssertMsg( !( !m_nENVMAP && m_nENVMAPMASK ), "Invalid combo combination ( !ENVMAP && ENVMAPMASK )" );
		AssertMsg( !( m_nENVMAP && !m_nREADNORMAL ), "Invalid combo combination ( ENVMAP && !READNORMAL )" );
		AssertMsg( !( m_nENVMAPFRESNEL && !m_nREADNORMAL ), "Invalid combo combination ( ENVMAPFRESNEL && !READNORMAL )" );
		AssertMsg( !( m_nPHONGFRESNEL && !m_nREADNORMAL ), "Invalid combo combination ( PHONGFRESNEL && !READNORMAL )" );
		AssertMsg( !( m_nRIMLIGHT && !m_nREADNORMAL ), "Invalid combo combination ( RIMLIGHT && !READNORMAL )" );
		AssertMsg( !( !m_nRIMLIGHT && m_nRIMLIGHTMODULATELIGHT ), "Invalid combo combination ( !RIMLIGHT && RIMLIGHTMODULATELIGHT )" );
		AssertMsg( !( !m_nBASETEXTURE2 && ( !m_nMULTIBLEND && m_nBLENDMODULATE ) ), "Invalid combo combination ( !BASETEXTURE2 && ( !MULTIBLEND && BLENDMODULATE ) )" );
		AssertMsg( !( m_nBASETEXTURE2 && m_nMULTIBLEND ), "Invalid combo combination ( BASETEXTURE2 && MULTIBLEND )" );
		AssertMsg( !( m_nENVMAPMASK && m_nMULTIBLEND ), "Invalid combo combination ( ENVMAPMASK && MULTIBLEND )" );
		AssertMsg( !( m_nSELFILLUM_MASK && m_nMULTIBLEND ), "Invalid combo combination ( SELFILLUM_MASK && MULTIBLEND )" );
		AssertMsg( !( !m_nSELFILLUM && m_nSELFILLUM_MASK ), "Invalid combo combination ( !SELFILLUM && SELFILLUM_MASK )" );
		AssertMsg( !( !m_nSELFILLUM && m_nSELFILLUM_ENVMAP_ALPHA ), "Invalid combo combination ( !SELFILLUM && SELFILLUM_ENVMAP_ALPHA )" );
		AssertMsg( !( m_nSELFILLUM_MASK && m_nSELFILLUM_ENVMAP_ALPHA ), "Invalid combo combination ( SELFILLUM_MASK && SELFILLUM_ENVMAP_ALPHA )" );
		AssertMsg( !( m_nSELFILLUM_MASK && m_nENVMAPMASK ), "Invalid combo combination ( SELFILLUM_MASK && ENVMAPMASK )" );
		AssertMsg( !( m_nSELFILLUM_ENVMAP_ALPHA && !m_nENVMAPMASK ), "Invalid combo combination ( SELFILLUM_ENVMAP_ALPHA && !ENVMAPMASK )" );
		return ( 2 * m_nALPHATEST ) + ( 4 * m_nTRANSLUCENT ) + ( 8 * m_nREADNORMAL ) + ( 16 * m_nNOCULL ) + ( 32 * m_nPHONGFRESNEL ) + ( 64 * m_nENVMAP ) + ( 128 * m_nENVMAPMASK ) + ( 256 * m_nENVMAPFRESNEL ) + ( 512 * m_nRIMLIGHT ) + ( 1024 * m_nRIMLIGHTMODULATELIGHT ) + ( 2048 * m_nBASETEXTURE2 ) + ( 4096 * m_nBLENDMODULATE ) + ( 8192 * m_nMULTIBLEND ) + ( 16384 * m_nSELFILLUM ) + ( 32768 * m_nSELFILLUM_MASK ) + ( 65536 * m_nSELFILLUM_ENVMAP_ALPHA ) + 0;
	}
};

#define shaderStaticTest_composite_ps30 psh_forgot_to_set_static_ALPHATEST + psh_forgot_to_set_static_TRANSLUCENT + psh_forgot_to_set_static_READNORMAL + psh_forgot_to_set_static_NOCULL + psh_forgot_to_set_static_PHONGFRESNEL + psh_forgot_to_set_static_ENVMAP + psh_forgot_to_set_static_ENVMAPMASK + psh_forgot_to_set_static_ENVMAPFRESNEL + psh_forgot_to_set_static_RIMLIGHT + psh_forgot_to_set_static_RIMLIGHTMODULATELIGHT + psh_forgot_to_set_static_BASETEXTURE2 + psh_forgot_to_set_static_BLENDMODULATE + psh_forgot_to_set_static_MULTIBLEND + psh_forgot_to_set_static_SELFILLUM + psh_forgot_to_set_static_SELFILLUM_MASK + psh_forgot_to_set_static_SELFILLUM_ENVMAP_ALPHA


class composite_ps30_Dynamic_Index
{
	unsigned int m_nPIXELFOGTYPE : 2;
#ifdef _DEBUG
	bool m_bPIXELFOGTYPE : 1;
#endif	// _DEBUG
public:
	void SetPIXELFOGTYPE( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nPIXELFOGTYPE = i;
#ifdef _DEBUG
		m_bPIXELFOGTYPE = true;
#endif	// _DEBUG
	}

	composite_ps30_Dynamic_Index( IShaderDynamicAPI* pShaderAPI )
	{
		m_nPIXELFOGTYPE = 0;
#ifdef _DEBUG
		m_bPIXELFOGTYPE = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bPIXELFOGTYPE );
		return ( 1 * m_nPIXELFOGTYPE ) + 0;
	}
};

#define shaderDynamicTest_composite_ps30 psh_forgot_to_set_dynamic_PIXELFOGTYPE

