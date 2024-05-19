//=========== Copyright © 2014, rHetorical, All rights reserved. =============
//
// Purpose: 
//		
//=============================================================================

#include "cbase.h"
#include "prop_vecball_dispenser.h"
#include "vectronic_gamerules.h"
#include "vectronic_player.h"
#include "weapon_vecgun.h"
#include "vectronic_shareddefs.h"
#include "soundenvelope.h"

#define ENTITY_MODEL "models/props/balldispenser.mdl"

//extern ConVar vectronic_enable_respawn;
//----------------------------------------------------------
// Particle attachment spawning. Read InputTestEntity for info. 
//----------------------------------------------------------
LINK_ENTITY_TO_CLASS( prop_vecball_dispenser, CVecBallDispenser );

BEGIN_DATADESC( CVecBallDispenser )

	// Save/load
	DEFINE_INPUTFUNC( FIELD_VOID, "RespawnBall", InputRespawn ),
	DEFINE_INPUTFUNC(FIELD_VOID, "Enable", InputEnable),
	DEFINE_INPUTFUNC(FIELD_VOID, "Disable", InputDisable),

	DEFINE_SOUNDPATCH( m_pLoopSound ),
	DEFINE_FIELD( m_hParticleEffect, FIELD_EHANDLE ),
	DEFINE_FIELD( m_hBaseParticleEffect, FIELD_EHANDLE ),
	DEFINE_FIELD( m_hBaseParticleEffect2, FIELD_EHANDLE ),
	DEFINE_FIELD( m_bDispatched, FIELD_BOOLEAN ),

	//DEFINE_KEYFIELD( m_intType, FIELD_INTEGER, "skin"),
	DEFINE_KEYFIELD( m_bDisabled, FIELD_BOOLEAN, "startdisabled"),
	DEFINE_KEYFIELD( m_bAutoRegen, FIELD_BOOLEAN, "autoregen"),

	DEFINE_OUTPUT( m_OnBallEquipped, "OnBallEquipped" ),

END_DATADESC()

//-----------------------------------------------------------------------------
// Purpose: Precache assets used by the entity
//-----------------------------------------------------------------------------
void CVecBallDispenser::Precache( void )
{
	PrecacheModel( ENTITY_MODEL );

	PrecacheScriptSound ( "Vectronic.Dispenser_Start" );
	PrecacheScriptSound ( "Vectronic.Dispenser_loop" );
	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose: Sets up the entity's initial state
//-----------------------------------------------------------------------------
void CVecBallDispenser::Spawn( void )
{
	Precache();
	SetModel( ENTITY_MODEL );

	SetSolid( SOLID_VPHYSICS );

	// Make it soild to other entities.
	VPhysicsInitStatic();

	//m_nSkin = m_intType;
	SetRenderColorToBalls();

	EnableBall();

	if (!m_bDisabled)
	{
		TurnOn();
	}

	BaseClass::Spawn();
}

void CVecBallDispenser::InputEnable( inputdata_t &inputdata )
{
	if ( m_bDisabled )
	{
		m_bDisabled = false;

		// Do a check. Make sure that we don't have the ball already.
		if (m_bAutoRegen)
		{
			// Check the player to see if they have the ball all ready.
			// This allows us to put multiple ball dispensers of the same type in the same map.
			SetNextThink( gpGlobals->curtime + 1 );
		}
		else
		{
			TurnOn();
		}
	}
}

void CVecBallDispenser::InputDisable( inputdata_t &inputdata )
{
	if ( !m_bDisabled )
	{
		m_bDisabled = true;
	}
}

// Only used by our think. 
void CVecBallDispenser::Restore( bool bSound )
{
	TurnOn();

	if (bSound )
	{
		EmitSound("Vectronic.Dispenser_Start");
	}

	CreateSounds();

	if(m_bDispatched)
	{
		m_bDisabled = false;
	}

	// Save on mem.
	SetNextThink( NULL );
}

//-----------------------------------------------------------------------------
// Purpose: We are called from Touch() to check if the player 
// has died or the Vecgun got cleansed to make sure we respawn the ball.
//-----------------------------------------------------------------------------
void CVecBallDispenser::Think( void )
{
	// if we are not disabled, why are we here?
	if(!m_bDisabled)
		return;

	// Get our player no matter where they are.
	CBaseEntity *pGetPlayer = UTIL_GetLocalPlayer();
	CVectronicPlayer *pPlayer = To_VectronicPlayer( pGetPlayer);

	// Test to see if they have not died. If so, respawn.
	if (rHGameRules()->IsRespawningEnabled())
	{	
		if (pPlayer->IsAlive() == false )
		{
			Restore( false );
		}
	}

	// This is sorta more of an expensive way of doing this, but it ensures that if the gun is at all empty.
	// The dispenser will regenerate.

	// A keyvalue bool here to ensure mappers still have control over if they want it done automatically or not.
	if (m_bAutoRegen) 
	{
		// Test the gun to check if the gun is empty by a cleanser, but we are disabled.
		CWeaponVecgun *pVecgun = dynamic_cast<CWeaponVecgun*>( pPlayer->Weapon_OwnsThisType( "weapon_vecgun" ) );

		if ( pVecgun )
		{
			if (pVecgun->IsUsingCustomBalls() == false )
			{
				if (GetOurType() == BALL0 && pVecgun->CanFireBlue() == false )
				{
					Restore();
				}
				else if (GetOurType() == BALL1 && pVecgun->CanFireGreen() == false )
				{
					Restore();
				}
				else if (GetOurType() == BALL2 && pVecgun->CanFireYellow() == false )
				{
					Restore();
				}
			}
			else
			{
				// Customs
				if (GetOurType() == BALL3 && pVecgun->CanFireBlue() == false )
				{
					Restore();
				}
				else if (GetOurType() == BALL4 && pVecgun->CanFireGreen() == false )
				{
					Restore();
				}
				else if (GetOurType() == BALL5 && pVecgun->CanFireYellow() == false )
				{
					Restore();
				}
			}

		}
	}

	// Ok, our player is still alive and the gun still has our ball.
	SetNextThink( gpGlobals->curtime );
}

//-----------------------------------------------------------------------------
// Purpose: Sets up the entity's initial state
//-----------------------------------------------------------------------------
void CVecBallDispenser::TurnOn( void )
{
	// If we are starting enabled.
	if(!m_bDispatched)
	{
		DispatchSpawn( m_hParticleEffect );
		DispatchSpawn( m_hBaseParticleEffect );
		DispatchSpawn( m_hBaseParticleEffect2 );

		m_hBaseParticleEffect2->Activate();
		m_hBaseParticleEffect->Activate();
		m_hParticleEffect->Activate();
		m_bDispatched = true;
	}

	m_hParticleEffect->StartParticleSystem();
	m_hBaseParticleEffect->StartParticleSystem();
	m_hBaseParticleEffect2->StartParticleSystem();
	CreateSounds();
}
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVecBallDispenser::EnableBall()
{
	m_hParticleEffect = (CParticleSystem *) CreateEntityByName( "info_particle_system" );
	if ( m_hParticleEffect != NULL )
	{
		Vector vecAbsOrigin = GetAbsOrigin();
		QAngle angles = GetAbsAngles();
			
		int		attachment;
		attachment = LookupAttachment( "attach_particle" );

		GetAttachment( attachment, vecAbsOrigin );
		m_hParticleEffect->SetAbsOrigin( vecAbsOrigin );
		m_hParticleEffect->SetAbsAngles( angles );
		m_hParticleEffect->SetParent(this);
		m_hParticleEffect->KeyValue( "start_active", "0" );
		m_hParticleEffect->KeyValue( "effect_name", BALL_THIRDPERSON );
	}

	m_hBaseParticleEffect = (CParticleSystem *) CreateEntityByName( "info_particle_system" );
	if ( m_hBaseParticleEffect != NULL )
	{
		Vector vecAbsOrigin = GetAbsOrigin();
		QAngle angles = GetAbsAngles();
			
		int		attachment;
		attachment = LookupAttachment( "attach_spotlight" );

		GetAttachment( attachment, vecAbsOrigin );
		m_hBaseParticleEffect->SetAbsOrigin( vecAbsOrigin );
		m_hBaseParticleEffect->SetAbsAngles( angles );
		m_hBaseParticleEffect->SetParent(this);
		m_hBaseParticleEffect->KeyValue( "start_active", "0" );
		m_hBaseParticleEffect->KeyValue( "effect_name", DISPENSER_CORE );
	}

	m_hBaseParticleEffect2 = (CParticleSystem *) CreateEntityByName( "info_particle_system" );
	if ( m_hBaseParticleEffect2 != NULL )
	{
		Vector vecAbsOrigin = GetAbsOrigin();
		QAngle angles = GetAbsAngles();
			
		int		attachment;
		attachment = LookupAttachment( "attach_spotlight" );

		GetAttachment( attachment, vecAbsOrigin );
		m_hBaseParticleEffect2->SetAbsOrigin( vecAbsOrigin );
		m_hBaseParticleEffect2->SetAbsAngles( angles );
		m_hBaseParticleEffect2->SetParent(this);
		m_hBaseParticleEffect2->KeyValue( "start_active", "0" );
		m_hBaseParticleEffect2->KeyValue( "effect_name", DISPENSER_BASE );

	}

	RefreshParticleColors();

	if(m_bDispatched)
	{
		m_bDisabled = false;
	}
}

void CVecBallDispenser::RefreshParticleColors()
{
	if (m_hParticleEffect != NULL && m_hBaseParticleEffect != NULL && m_hBaseParticleEffect2 != NULL)
	{
		Vector ParticleColor (GetOurBallColor_R(), GetOurBallColor_G(), GetOurBallColor_B());
		m_hParticleEffect->SetControlPointValue( 7, ParticleColor );
		m_hBaseParticleEffect->SetControlPointValue( 7, ParticleColor );
		m_hBaseParticleEffect2->SetControlPointValue( 7, ParticleColor );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVecBallDispenser::Touch( CBaseEntity *pOther )
{
	if (m_bDisabled)
		return;

	if ( pOther->IsPlayer())
	{
		CVectronicPlayer *pPlayer = To_VectronicPlayer (pOther );
		if ( pPlayer )
		{
			CWeaponVecgun *pVecgun = dynamic_cast<CWeaponVecgun*>( pPlayer->Weapon_OwnsThisType( "weapon_vecgun" ) );

			if ( pVecgun )
			{
				if(!m_bDispatched)
					return;

				// If the gun is set to fire custom balls, but we come across our balls, ingnore touching.
				if (pVecgun->IsUsingCustomBalls() && GetOurType() <= BALL2 )
					return;

				// If the gun is set to fire our balls, but we come across custom balls, ingnore touching.
				if (pVecgun->IsUsingCustomBalls() == false && GetOurType() >= BALL3 )
					return;

				if (GetOurType() == BALL0 || GetOurType() == BALL3 )
				{
					if (pVecgun->CanFireBlue())
						return;

					pVecgun->SetCanFireBlue();
				}

				if (GetOurType() == BALL1 || GetOurType() == BALL4 )
				{
					if (pVecgun->CanFireGreen())
						return;

					pVecgun->SetCanFireGreen();
				}

				if (GetOurType() == BALL2 || GetOurType() == BALL5 )
				{
					if (pVecgun->CanFireYellow())
						return;

					pVecgun->SetCanFireYellow();
				}

				// Disable when we touch.
				m_bDisabled = true;
				m_OnBallEquipped.FireOutput(this,this);
				m_hParticleEffect->StopParticleSystem();
				m_hBaseParticleEffect->StopParticleSystem();
				StopLoopingSounds();

				SetNextThink( gpGlobals->curtime + 1 );
			}
		}
		else
		{
			// We don't want to anything if the player touches the trigger without the gun.
			return;
		}

	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVecBallDispenser::InputRespawn ( inputdata_t &inputData )
{
	TurnOn();
	EmitSound("Vectronic.Dispenser_Start");
	CreateSounds();

	if(m_bDispatched)
	{
		m_bDisabled = false;
	}
}
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVecBallDispenser::CreateSounds()
{
	if ( !m_pLoopSound )
	{
		CSoundEnvelopeController &controller = CSoundEnvelopeController::GetController();

		CReliableBroadcastRecipientFilter filter;

		m_pLoopSound = controller.SoundCreate( filter, entindex(), "Vectronic.Dispenser_loop" );
		controller.Play( m_pLoopSound, 0.78, 100 );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVecBallDispenser::StopLoopingSounds()
{
	if ( m_pLoopSound )
	{
		CSoundEnvelopeController &controller = CSoundEnvelopeController::GetController();

		controller.SoundDestroy( m_pLoopSound );
		m_pLoopSound = NULL;
	}

	BaseClass::StopLoopingSounds();
}
