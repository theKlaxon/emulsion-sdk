//=========== Copyright © 2014, rHetorical, All rights reserved. =============
//
// Purpose: 
//		
//=============================================================================

#include "cbase.h"
#include "c_baseglowanimating.h"
#include "c_baseanimating.h"
#include "glow_outline_effect.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

EXTERN_RECV_TABLE(DT_BaseGlowAnimating);
IMPLEMENT_CLIENTCLASS(C_BaseGlowAnimating, DT_BaseGlowAnimating, CBaseGlowAnimating);

BEGIN_RECV_TABLE(C_BaseGlowAnimating, DT_BaseGlowAnimating)
	RecvPropBool( RECVINFO( m_bGlowEnabled ) ),
	//RecvPropBool(RECVINFO(m_bRenderWhenOccluded)),
	//RecvPropBool(RECVINFO(m_bRenderWhenUnOccluded)),
	RecvPropFloat( RECVINFO( m_flRedGlowColor ) ),
	RecvPropFloat( RECVINFO( m_flGreenGlowColor ) ),
	RecvPropFloat( RECVINFO( m_flBlueGlowColor ) ),
	RecvPropFloat(RECVINFO(m_flAlphaGlowColor)),
END_RECV_TABLE()

BEGIN_PREDICTION_DATA( C_BaseGlowAnimating )
END_PREDICTION_DATA()

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C_BaseGlowAnimating::C_BaseGlowAnimating()
{
	m_pGlowEffect = NULL;
	m_bGlowEnabled = false;
	m_bOldGlowEnabled = false;
	//m_bRenderWhenOccluded = true;
	//m_bRenderWhenUnOccluded = false;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C_BaseGlowAnimating::~C_BaseGlowAnimating()
{
	DestroyGlowEffect();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_BaseGlowAnimating::OnPreDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnPreDataChanged( updateType );

	m_bOldGlowEnabled = m_bGlowEnabled;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_BaseGlowAnimating::OnDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnDataChanged( updateType );

	if ( m_bOldGlowEnabled != m_bGlowEnabled )
	{
		UpdateGlowEffect();
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_BaseGlowAnimating::GetGlowEffectColor( float *r, float *g, float *b )
{
	*r = m_flRedGlowColor;
	*g = m_flGreenGlowColor;
	*b = m_flBlueGlowColor;
}


//ConVar cl_glow_range_exp("cl_glow_range_exp", "1.0");
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_BaseGlowAnimating::UpdateGlowEffect( void )
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

		m_pGlowEffect = new CGlowObject(this, Vector(r, g, b), 1.0, true);

		/*
		C_BasePlayer *localPlayer = C_BasePlayer::GetLocalPlayer();
		float cameraDistance = (localPlayer->EyePosition() - EyePosition()).Length();
		cameraDistance *= localPlayer->GetFOVDistanceAdjustFactor();

		float rangeAlpha = 0.0f;
		float NoGlowAlpha = 0.0f;
		float NoGlowDistance = 500.0f;
		float FullGlowDistance = 1500.0f;
		if (cameraDistance <= NoGlowDistance)
		{
			rangeAlpha = 0.0f;
		}
		else if (cameraDistance >= FullGlowDistance)
		{
			rangeAlpha = 1.0f;
		}
		else
		{
			rangeAlpha = 1.0f - (FullGlowDistance - cameraDistance) / (FullGlowDistance - NoGlowDistance);	// 0..1
			rangeAlpha = powf(rangeAlpha, cl_glow_range_exp.GetFloat());
			rangeAlpha = NoGlowAlpha + rangeAlpha * (1 - NoGlowAlpha);	// NoGlowAlpha..1
		}

		m_pGlowEffect->SetAlpha(rangeAlpha);
		*/
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_BaseGlowAnimating::DestroyGlowEffect( void )
{
	if ( m_pGlowEffect )
	{
		delete m_pGlowEffect;
		m_pGlowEffect = NULL;
	}
}