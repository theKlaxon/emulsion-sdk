//=========== Copyright © 2014, rHetorical, All rights reserved. =============
//
// Purpose: 
//		
//=============================================================================

#ifndef BASEGLOWANIMATING_H
#define BASEGLOWANIMATING_H
#ifdef _WIN32
#pragma once
#endif

#include "cbase.h"
#include "baseanimating.h"
#include "baseentity.h"
#include "entityoutput.h"
#include "studio.h"
#include "datacache/idatacache.h"
#include "tier0/threadtools.h"

//-----------------------------------------------------------------------------
// Purpose: Base
//-----------------------------------------------------------------------------
class CBaseGlowAnimating : public CBaseAnimating
{
	DECLARE_CLASS( CBaseGlowAnimating, CBaseAnimating );
public:
	
	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();
	DECLARE_PREDICTABLE();

	//Constructor
	CBaseGlowAnimating();

	void Precache( void );
	void Spawn( void );

	// Glows
	void				AddGlowEffect( void );
	void				RemoveGlowEffect( void );
	bool				IsGlowEffectActive( void );

	void InputStartGlow( inputdata_t &inputData );
	void InputEndGlow( inputdata_t &inputData );

protected:
	void SetGlowVector(float r, float g, float b );
	CNetworkVar( bool, m_bGlowEnabled );
	//CNetworkVar(bool, m_bRenderWhenOccluded);
	//CNetworkVar(bool, m_bRenderWhenUnOccluded);
	CNetworkVar( float, m_flRedGlowColor );
	CNetworkVar( float, m_flGreenGlowColor );
	CNetworkVar( float, m_flBlueGlowColor );
	CNetworkVar(float, m_flAlphaGlowColor);
	CNetworkColor32(m_clrGlow);

};


#endif //BASEGLOWANIMATING_H
