//=========== Copyright © 2014, rHetorical, All rights reserved. =============
//
// Purpose: 
//		
//=============================================================================

#include "cbase.h"
#include "vectronic_gamerules.h"
#include "vectronic_vecall_system.h"
#include "vectronic_player.h"
#include "weapon_vecgun.h"

extern ConVar vectronic_vecball3_color_r;
extern ConVar vectronic_vecball3_color_g;
extern ConVar vectronic_vecball3_color_b;

extern ConVar vectronic_vecball4_color_r;
extern ConVar vectronic_vecball4_color_g;
extern ConVar vectronic_vecball4_color_b;

extern ConVar vectronic_vecball5_color_r;
extern ConVar vectronic_vecball5_color_g;
extern ConVar vectronic_vecball5_color_b;

extern ConVar vectronic_vecball3_sprite;
extern ConVar vectronic_vecball4_sprite;
extern ConVar vectronic_vecball5_sprite;

ConVar	vectronic_realtime_color_change	( "vectronic_realtime_color_change", "0" );

//LINK_ENTITY_TO_CLASS( prop_vectronic, CVectronicProp );

BEGIN_DATADESC( CVectronicProp )

	DEFINE_THINKFUNC( ThinkCheckBallColors ),

	DEFINE_FIELD( n_intBall3_r, FIELD_INTEGER ),
	DEFINE_FIELD( n_intBall3_g, FIELD_INTEGER ),
	DEFINE_FIELD( n_intBall3_b, FIELD_INTEGER ),

	DEFINE_FIELD( n_intBall4_r, FIELD_INTEGER ),
	DEFINE_FIELD( n_intBall4_g, FIELD_INTEGER ),
	DEFINE_FIELD( n_intBall4_b, FIELD_INTEGER ),

	DEFINE_FIELD( n_intBall5_r, FIELD_INTEGER ),
	DEFINE_FIELD( n_intBall5_g, FIELD_INTEGER ),
	DEFINE_FIELD( n_intBall5_b, FIELD_INTEGER ),

	DEFINE_KEYFIELD( m_intType, FIELD_INTEGER, "BallType"),

END_DATADESC()

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVectronicProp::Precache( void )
{
	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVectronicProp::Spawn( void )
{
	BaseClass::Spawn();

	// We are crashing. Make it only do a realtime color check if the convar is set.
	if (vectronic_realtime_color_change.GetBool())
	{
		if ( FClassnameIs( this, "prop_vecbox" ) || m_intType >= BALL3)
		{
			RegisterThinkContext( "ThinkCheckBallColors" );
			SetContextThink( &CVectronicProp::ThinkCheckBallColors, gpGlobals->curtime, "ThinkCheckBallColors" );
			SetNextThink(gpGlobals->curtime, "ThinkCheckBallColors" );
		}
	}
	else
	{
		DevMsg("CVectronicProp: Setting static colors for %s.\n", this->GetClassname());
		n_intBall3_r = vectronic_vecball3_color_r.GetInt();
		n_intBall3_g = vectronic_vecball3_color_g.GetInt();
		n_intBall3_b = vectronic_vecball3_color_b.GetInt();

		n_intBall4_r = vectronic_vecball4_color_r.GetInt();
		n_intBall4_g = vectronic_vecball4_color_g.GetInt();
		n_intBall4_b = vectronic_vecball4_color_b.GetInt();

		n_intBall5_r = vectronic_vecball5_color_r.GetInt();
		n_intBall5_g = vectronic_vecball5_color_g.GetInt();
		n_intBall5_b = vectronic_vecball5_color_b.GetInt();

		SetRenderColorToBalls();
	}
}

//-----------------------------------------------------------------------------
// Purpose: Only called if vectronic_realtime_color_change is set to true!
//-----------------------------------------------------------------------------
void CVectronicProp::ThinkCheckBallColors()
{
	if (n_intBall3_r != vectronic_vecball3_color_r.GetInt() || 
		n_intBall3_g != vectronic_vecball3_color_g.GetInt() ||
		n_intBall3_b != vectronic_vecball3_color_b.GetInt() )
	{

		n_intBall3_r = vectronic_vecball3_color_r.GetInt();
		n_intBall3_g = vectronic_vecball3_color_g.GetInt();
		n_intBall3_b = vectronic_vecball3_color_b.GetInt();

		// refresh our colors.
		SetRenderColorToBalls();
	}

	if (n_intBall4_r != vectronic_vecball4_color_r.GetInt() || 
		n_intBall4_g != vectronic_vecball4_color_g.GetInt() ||
		n_intBall4_b != vectronic_vecball4_color_b.GetInt()  )
	{
		n_intBall4_r = vectronic_vecball4_color_r.GetInt();
		n_intBall4_g = vectronic_vecball4_color_g.GetInt();
		n_intBall4_b = vectronic_vecball4_color_b.GetInt();

		// refresh our colors.
		SetRenderColorToBalls();
	}

	if (n_intBall5_r != vectronic_vecball5_color_r.GetInt() || 
		n_intBall5_g != vectronic_vecball5_color_g.GetInt() ||
		n_intBall5_b != vectronic_vecball5_color_b.GetInt()  )
	{
		n_intBall5_r = vectronic_vecball5_color_r.GetInt();
		n_intBall5_g = vectronic_vecball5_color_g.GetInt();
		n_intBall5_b = vectronic_vecball5_color_b.GetInt();

		// refresh our colors.
		SetRenderColorToBalls();
	}

	// Think again.
	if (vectronic_realtime_color_change.GetBool())
	{
		SetNextThink(gpGlobals->curtime + .1, "ThinkCheckBallColors" );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CVectronicProp::GetOurBallColor_R()
{
	if (GetOurType() == BALL0)
	{
		return BALL0_COLOR_R;
	}

	if (GetOurType() == BALL1)
	{
		return BALL1_COLOR_R;
	}

	if (GetOurType() == BALL2)
	{
		return BALL2_COLOR_R;
	}

	if (GetOurType() == BALL3)
	{
		return n_intBall3_r;
	}

	if (GetOurType() == BALL4)
	{
		return n_intBall4_r;
	}

	if (GetOurType() == BALL5)
	{
		return n_intBall5_r;
	}

	if (GetOurType() == BALL_ANY)
	{
		return 255;
	}

	return 255;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CVectronicProp::GetOurBallColor_G()
{
	if (GetOurType() == BALL0)
	{
		return BALL0_COLOR_G;
	}

	if (GetOurType() == BALL1)
	{
		return BALL1_COLOR_G;
	}

	if (GetOurType() == BALL2)
	{
		return BALL2_COLOR_G;
	}

	if (GetOurType() == BALL3)
	{
		return n_intBall3_g;
	}

	if (GetOurType() == BALL4)
	{
		return n_intBall4_g;
	}

	if (GetOurType() == BALL5)
	{
		return n_intBall5_g;
	}

	if (GetOurType() == BALL_ANY)
	{
		return 255;
	}

	return 255;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CVectronicProp::GetOurBallColor_B()
{
	if (GetOurType() == BALL0)
	{
		return BALL0_COLOR_B;
	}

	if (GetOurType() == BALL1)
	{
		return BALL1_COLOR_B;
	}

	if (GetOurType() == BALL2)
	{
		return BALL2_COLOR_B;
	}

	if (GetOurType() == BALL3)
	{
		return n_intBall3_b;
	}

	if (GetOurType() == BALL4)
	{
		return n_intBall4_b;
	}

	if (GetOurType() == BALL5)
	{
		return n_intBall5_b;
	}

	if (GetOurType() == BALL_ANY)
	{
		return 255;
	}

	return 255;

}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVectronicProp::SetRenderColorToBalls()
{
	SetRenderColor( GetOurBallColor_R(), GetOurBallColor_G(), GetOurBallColor_B() );

	// Our children classes have these funcions defined more.
	RefreshParticleColors();
	RefreshSpriteColors();
}

//-----------------------------------------------------------------------------
// Purpose: Add a glow based on the type color.
//-----------------------------------------------------------------------------
void CVectronicProp::MakeGlowBasedOnBallColor()
{
	float r = GetOurBallColor_R() * FLOAT_TO_VECTOR_MULTIPLER;
	float g = GetOurBallColor_G() * FLOAT_TO_VECTOR_MULTIPLER;
	float b = GetOurBallColor_B() * FLOAT_TO_VECTOR_MULTIPLER;

	SetGlowVector( r, g, b );
	AddGlowEffect();
}

//-----------------------------------
//-----------------------------------
//-----------------------------------
//-----------------------------------
// !!! CLEANSER STUFF STARTS HERE !!!
//-----------------------------------
//-----------------------------------
//-----------------------------------

// Particle.
LINK_ENTITY_TO_CLASS( info_vecball_cleanser, CPointCleanser );

BEGIN_DATADESC( CPointCleanser )
	DEFINE_FIELD( m_bDispatched, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_hParticleEffect, FIELD_EHANDLE ),

	DEFINE_FIELD( n_intBall3_r, FIELD_INTEGER ),
	DEFINE_FIELD( n_intBall3_g, FIELD_INTEGER ),
	DEFINE_FIELD( n_intBall3_b, FIELD_INTEGER ),

	DEFINE_FIELD( n_intBall4_r, FIELD_INTEGER ),
	DEFINE_FIELD( n_intBall4_g, FIELD_INTEGER ),
	DEFINE_FIELD( n_intBall4_b, FIELD_INTEGER ),

	DEFINE_FIELD( n_intBall5_r, FIELD_INTEGER ),
	DEFINE_FIELD( n_intBall5_g, FIELD_INTEGER ),
	DEFINE_FIELD( n_intBall5_b, FIELD_INTEGER ),

	DEFINE_KEYFIELD( m_bDisabled, FIELD_BOOLEAN, "startdisabled"),
	DEFINE_KEYFIELD( m_bWide, FIELD_BOOLEAN, "wide"),
	DEFINE_KEYFIELD( m_nFilterType,	FIELD_INTEGER,	"filtertype" ),

	//DEFINE_KEYFIELD( m_iszPropName,	FIELD_STRING,	"propnames" ),

	DEFINE_INPUTFUNC( FIELD_VOID, "TurnOn", InputTurnOn ),
	DEFINE_INPUTFUNC( FIELD_VOID, "TurnOff", InputTurnOff ),

END_DATADESC()

//-----------------------------------------------------------------------------
// Purpose: Sets up the entity's initial state
//-----------------------------------------------------------------------------
void CPointCleanser::Spawn( void )
{
	if (!m_bDisabled && !m_bDispatched)
	{
		TurnOn();

		CBaseEntity *pEntity = gEntList.FindEntityByName( NULL, m_target );
	
		while( pEntity ) 
		{
			pEntity->SetRenderColor(GetOurBallColor_R(),GetOurBallColor_G(),GetOurBallColor_B());
			pEntity = gEntList.FindEntityByName( pEntity, m_target );  
		}

		// We are crashing. Make it only do a realtime color check if the convar is set.
		if (vectronic_realtime_color_change.GetBool())
		{
			RegisterThinkContext( "ThinkCheckBallColors" );
			SetContextThink( &CVectronicProp::ThinkCheckBallColors, gpGlobals->curtime, "ThinkCheckBallColors" );
			SetNextThink(gpGlobals->curtime + .1, "ThinkCheckBallColors" );
		}
		else
		{
			DevMsg("CPointCleanser: Setting static colors for %s.\n", this->GetEntityName() );
			n_intBall3_r = vectronic_vecball3_color_r.GetInt();
			n_intBall3_g = vectronic_vecball3_color_g.GetInt();
			n_intBall3_b = vectronic_vecball3_color_b.GetInt();

			n_intBall4_r = vectronic_vecball4_color_r.GetInt();
			n_intBall4_g = vectronic_vecball4_color_g.GetInt();
			n_intBall4_b = vectronic_vecball4_color_b.GetInt();

			n_intBall5_r = vectronic_vecball5_color_r.GetInt();
			n_intBall5_g = vectronic_vecball5_color_g.GetInt();
			n_intBall5_b = vectronic_vecball5_color_b.GetInt();

			ChangeColor();
		}

		/*
		// Find our props!
		if (!m_hProp)
		{
			m_hProp = dynamic_cast<CBaseGlowAnimating*>( gEntList.FindEntityByName(NULL, STRING(m_iszPropName) ) );

			if (m_hProp)
			{
				m_hProp->SetRenderColor(GetOurBallColor_R(),GetOurBallColor_G(),GetOurBallColor_B());
			}
		}
		*/

		BaseClass::Spawn();
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPointCleanser::ThinkCheckBallColors()
{
	if (m_bDisabled)
		return;

	if (n_intBall3_r != vectronic_vecball3_color_r.GetInt() || 
		n_intBall3_g != vectronic_vecball3_color_g.GetInt() ||
		n_intBall3_b != vectronic_vecball3_color_b.GetInt() )
	{
		n_intBall3_r = vectronic_vecball3_color_r.GetInt();
		n_intBall3_g = vectronic_vecball3_color_g.GetInt();
		n_intBall3_b = vectronic_vecball3_color_b.GetInt();

		// refresh our colors.
		ChangeColor();
	}

	if (n_intBall4_r != vectronic_vecball4_color_r.GetInt() || 
		n_intBall4_g != vectronic_vecball4_color_g.GetInt() ||
		n_intBall4_b != vectronic_vecball4_color_b.GetInt()  )
	{
		n_intBall4_r = vectronic_vecball4_color_r.GetInt();
		n_intBall4_g = vectronic_vecball4_color_g.GetInt();
		n_intBall4_b = vectronic_vecball4_color_b.GetInt();

		// refresh our colors.
		ChangeColor();
	}

	if (n_intBall5_r != vectronic_vecball5_color_r.GetInt() || 
		n_intBall5_g != vectronic_vecball5_color_g.GetInt() ||
		n_intBall5_b != vectronic_vecball5_color_b.GetInt()  )
	{
		n_intBall5_r = vectronic_vecball5_color_r.GetInt();
		n_intBall5_g = vectronic_vecball5_color_g.GetInt();
		n_intBall5_b = vectronic_vecball5_color_b.GetInt();

		// refresh our colors.
		ChangeColor();
	}

	// We are crashing. Make it only do a realtime color check if the convar is set.
	if (vectronic_realtime_color_change.GetBool())
	{
		SetNextThink(gpGlobals->curtime + .1, "ThinkCheckBallColors" );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CPointCleanser::GetOurBallColor_R()
{
	if (GetOurType() == BALL0)
	{
		return BALL0_COLOR_R;
	}

	if (GetOurType() == BALL1)
	{
		return BALL1_COLOR_R;
	}

	if (GetOurType() == BALL2)
	{
		return BALL2_COLOR_R;
	}

	if (GetOurType() == BALL3)
	{
		return n_intBall3_r;
	}

	if (GetOurType() == BALL4)
	{
		return n_intBall4_r;
	}

	if (GetOurType() == BALL5)
	{
		return n_intBall5_r;
	}

	if (GetOurType() == BALL_ANY)
	{
		return 255;
	}

	return 255;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CPointCleanser::GetOurBallColor_G()
{
	if (GetOurType() == BALL0)
	{
		return BALL0_COLOR_G;
	}

	if (GetOurType() == BALL1)
	{
		return BALL1_COLOR_G;
	}

	if (GetOurType() == BALL2)
	{
		return BALL2_COLOR_G;
	}

	if (GetOurType() == BALL3)
	{
		return n_intBall3_g;
	}

	if (GetOurType() == BALL4)
	{
		return n_intBall4_g;
	}

	if (GetOurType() == BALL5)
	{
		return n_intBall5_g;
	}

	if (GetOurType() == BALL_ANY)
	{
		return 255;
	}

	return 255;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CPointCleanser::GetOurBallColor_B()
{
	if (GetOurType() == BALL0)
	{
		return BALL0_COLOR_B;
	}

	if (GetOurType() == BALL1)
	{
		return BALL1_COLOR_B;
	}

	if (GetOurType() == BALL2)
	{
		return BALL2_COLOR_B;
	}

	if (GetOurType() == BALL3)
	{
		return n_intBall3_b;
	}

	if (GetOurType() == BALL4)
	{
		return n_intBall4_b;
	}

	if (GetOurType() == BALL5)
	{
		return n_intBall5_b;
	}

	if (GetOurType() == BALL_ANY)
	{
		return 255;
	}

	return 255;

}

void CPointCleanser::ChangeColor()
{
	if (m_hParticleEffect != NULL )
	{
		if (m_nFilterType == BALL_ANY )
		{
			Vector ParticleColor (255, 255, 255);
			m_hParticleEffect->SetControlPointValue( 7, ParticleColor );
		}
		else
		{
			Vector ParticleColor (GetOurBallColor_R(), GetOurBallColor_G(), GetOurBallColor_B());
			m_hParticleEffect->SetControlPointValue( 7, ParticleColor );
		}
	}
}
//-----------------------------------------------------------------------------
// Purpose: Sets up the entity's initial state
//-----------------------------------------------------------------------------
void CPointCleanser::TurnOn( void )
{
	m_hParticleEffect = (CParticleSystem *) CreateEntityByName( "info_particle_system" );
	if ( m_hParticleEffect != NULL )
	{
		QAngle angles = GetAbsAngles();
		// Setup our basic parameters
		m_hParticleEffect->KeyValue( "start_active", "1" );

		if (m_bWide)
		{
			m_hParticleEffect->KeyValue( "effect_name", BALL_CLEANSER );
		}
		else
		{
			m_hParticleEffect->KeyValue( "effect_name", BALL_CLEANSER_MED );
		}

		m_hParticleEffect->SetParent( this );
		m_hParticleEffect->SetAbsOrigin( WorldSpaceCenter());
		m_hParticleEffect->SetAbsAngles( angles );

		ChangeColor();

		DispatchSpawn( m_hParticleEffect );
		m_hParticleEffect->Activate();
		m_hParticleEffect->StartParticleSystem();
		m_bDispatched = true;
	}
}

void CPointCleanser::InputTurnOn( inputdata_t &inputData )
{
	if (!m_bDispatched)
	{
		TurnOn();

		// We are crashing. Make it only do a realtime color check if the convar is set.
		if (vectronic_realtime_color_change.GetBool())
		{
			// Check the colors again.
			SetNextThink(gpGlobals->curtime, "ThinkCheckBallColors" );
		}
	}

	m_bDisabled = false;
	m_hParticleEffect->Activate();
	m_hParticleEffect->StartParticleSystem();
}

void CPointCleanser::InputTurnOff( inputdata_t &inputData )
{
	m_hParticleEffect->StopParticleSystem();
	m_bDisabled = true;
}