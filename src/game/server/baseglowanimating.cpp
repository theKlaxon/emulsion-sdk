//=========== Copyright © 2014, rHetorical, All rights reserved. =============
//
// Purpose: 
//		
//=============================================================================

#include "cbase.h"
#include "baseglowanimating.h"

LINK_ENTITY_TO_CLASS( prop_glow, CBaseGlowAnimating );
PRECACHE_REGISTER(prop_glow);

IMPLEMENT_SERVERCLASS_ST(CBaseGlowAnimating, DT_BaseGlowAnimating)
	SendPropBool( SENDINFO( m_bGlowEnabled ) ),
	//SendPropBool(SENDINFO(m_bRenderWhenOccluded)),
	//SendPropBool(SENDINFO(m_bRenderWhenUnOccluded)),
	SendPropFloat( SENDINFO( m_flRedGlowColor ) ),
	SendPropFloat( SENDINFO( m_flGreenGlowColor ) ),
	SendPropFloat( SENDINFO( m_flBlueGlowColor ) ),
END_SEND_TABLE()

BEGIN_DATADESC( CBaseGlowAnimating )

	DEFINE_KEYFIELD(m_bGlowEnabled, FIELD_BOOLEAN, "glowstate"),
	//DEFINE_KEYFIELD(m_bRenderWhenOccluded, FIELD_BOOLEAN, "glowrenderwhenoccluded"),
	//DEFINE_KEYFIELD(m_bRenderWhenUnOccluded, FIELD_BOOLEAN, "glowrenderwhenunoccluded"),
	DEFINE_KEYFIELD(m_clrGlow, FIELD_COLOR32, "glowcolor"),

	//Save/load
	//DEFINE_FIELD( m_bGlowEnabled, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_flRedGlowColor, FIELD_FLOAT ),
	DEFINE_FIELD( m_flGreenGlowColor, FIELD_FLOAT ),
	DEFINE_FIELD( m_flBlueGlowColor, FIELD_FLOAT ),
	DEFINE_FIELD (m_flAlphaGlowColor, FIELD_FLOAT),


	// I/O
	DEFINE_INPUTFUNC( FIELD_VOID, "StartGlowing", InputStartGlow ),
	DEFINE_INPUTFUNC( FIELD_VOID, "StopGlowing", InputEndGlow ),

END_DATADESC()

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CBaseGlowAnimating::CBaseGlowAnimating()
{
	m_bGlowEnabled.Set( false );

	m_flRedGlowColor = 0.76f;
	m_flGreenGlowColor = 0.76f;
	m_flBlueGlowColor = 0.76f;

}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CBaseGlowAnimating::Precache( void )
{
	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CBaseGlowAnimating::Spawn( void )
{
	m_flRedGlowColor = m_clrGlow->r;
	m_flGreenGlowColor = m_clrGlow->g;
	m_flBlueGlowColor = m_clrGlow->b;
	//m_flAlphaGlowColor = m_clrGlow->a;

	BaseClass::Spawn();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CBaseGlowAnimating::AddGlowEffect( void )
{
	SetTransmitState( FL_EDICT_ALWAYS );
	m_bGlowEnabled.Set( true );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CBaseGlowAnimating::RemoveGlowEffect( void )
{
	m_bGlowEnabled.Set( false );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CBaseGlowAnimating::IsGlowEffectActive( void )
{
	return m_bGlowEnabled;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CBaseGlowAnimating::SetGlowVector(float r, float g, float b )
{
	m_flRedGlowColor = r;
	m_flGreenGlowColor = g;
	m_flBlueGlowColor = b;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CBaseGlowAnimating::InputStartGlow( inputdata_t &inputData )
{
	AddGlowEffect();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CBaseGlowAnimating::InputEndGlow( inputdata_t &inputData )
{
	RemoveGlowEffect();
}