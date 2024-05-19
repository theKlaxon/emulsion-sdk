//=========== Copyright © 2014, rHetorical, All rights reserved. =============
//
// Purpose: 
//		
//=============================================================================

#include "cbase.h"
#include "c_prop_vecbox.h"
#include "c_baseanimating.h"

#ifdef GLOWS_ENABLE
#include "glow_outline_effect.h"
#endif // GLOWS_ENABLE

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

EXTERN_RECV_TABLE(DT_VecBox);
IMPLEMENT_CLIENTCLASS(C_VecBox, DT_VecBox, CVecBox);

BEGIN_RECV_TABLE(C_VecBox, DT_VecBox)
#ifdef GLOWS_ENABLE
	RecvPropBool( RECVINFO( m_bGlowEnabled ) ),
	RecvPropFloat( RECVINFO( m_flRedGlowColor ) ),
	RecvPropFloat( RECVINFO( m_flGreenGlowColor ) ),
	RecvPropFloat( RECVINFO( m_flBlueGlowColor ) ),
#endif // GLOWS_ENABLE
END_RECV_TABLE()

BEGIN_PREDICTION_DATA( C_VecBox )
END_PREDICTION_DATA()


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C_VecBox::C_VecBox()
{
	#ifdef GLOWS_ENABLE
		m_pGlowEffect = NULL;
		m_bGlowEnabled = false;
		m_bOldGlowEnabled = false;
	#endif // GLOWS_ENABLE
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C_VecBox::~C_VecBox()
{
	#ifdef GLOWS_ENABLE
		DestroyGlowEffect();
	#endif // GLOWS_ENABLE
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_VecBox::OnPreDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnPreDataChanged( updateType );

#ifdef GLOWS_ENABLE
	m_bOldGlowEnabled = m_bGlowEnabled;
#endif // GLOWS_ENABLE
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_VecBox::OnDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnDataChanged( updateType );

#ifdef GLOWS_ENABLE
	if ( m_bOldGlowEnabled != m_bGlowEnabled )
	{
		UpdateGlowEffect();
	}
#endif // GLOWS_ENABLE
}

#ifdef GLOWS_ENABLE
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_VecBox::GetGlowEffectColor( float *r, float *g, float *b )
{
	*r = m_flRedGlowColor;
	*g = m_flGreenGlowColor;
	*b = m_flBlueGlowColor;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_VecBox::UpdateGlowEffect( void )
{
	// destroy the existing effect
	if ( m_pGlowEffect )
	{
		DestroyGlowEffect();
	}

	// create a new effect
	if ( m_bGlowEnabled )
	{
		float r, g, b;
		GetGlowEffectColor( &r, &g, &b );

		m_pGlowEffect = new CGlowObject( this, Vector( r, g, b ), 1.0, true );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_VecBox::DestroyGlowEffect( void )
{
	if ( m_pGlowEffect )
	{
		delete m_pGlowEffect;
		m_pGlowEffect = NULL;
	}
}

#endif // GLOWS_ENABLE
