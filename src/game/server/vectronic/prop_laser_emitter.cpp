//=========== Copyright © 2014, rHetorical, All rights reserved. =============
//
// Purpose: 
//		
//=============================================================================

#include "cbase.h"
#include "EnvLaser.h"
#include "beam_shared.h"
#include "particle_parse.h"
#include "particle_system.h"
#include "sprite.h"
#include "soundenvelope.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define ATTACHMENT0 "center_attach"
#define ATTACHMENT1 "left_attach"
#define ATTACHMENT2 "right_attach"
#define ATTACHMENT3 "up_attach"
#define ATTACHMENT4 "down_attach"

#define ENTITY_MODEL "models/props/laser_emiter.mdl"
#define ENITIY_SOUND "Vectronic.LaserLoop"
#define ENTITY_PARTICLE "laser_emiter"
#define ENITIY_SPRITE "laser_emiter"
#define ENITIY_BEAM "effects/laser1.vmt"

class CPropLaser : public CBaseAnimating
{
public:
	DECLARE_CLASS( CPropLaser, CBaseAnimating );
	DECLARE_DATADESC();

	void Spawn( void );
	void Precache( void );
	void SetupLaser();

	void TurnOn();
	void TurnOff();

	void CreateSounds();
	void StopLoopingSounds();

	void InputTurnOn ( inputdata_t &inputData );
	void InputTurnOff ( inputdata_t &inputData );
	void InputToggle ( inputdata_t &inputData );

private:

	bool m_bLaserOn;
	bool m_bStartLaserOff;
//	bool m_bDeadly;

	CHandle<CEnvLaser>	m_hLaser;
	CHandle<CParticleSystem> m_hParticleEffect;
	CSoundPatch		*m_pLoopSound;
	CHandle<CSprite>	m_hSprite;

	CHandle<CBeam>		m_hBeam1;
	CHandle<CBeam>		m_hBeam2;
};

LINK_ENTITY_TO_CLASS( prop_laser_emitter, CPropLaser )

BEGIN_DATADESC( CPropLaser )

	// Save/load
	DEFINE_SOUNDPATCH( m_pLoopSound ),
	DEFINE_FIELD( m_hLaser, FIELD_EHANDLE ),
	DEFINE_FIELD( m_hParticleEffect, FIELD_EHANDLE ),
	DEFINE_FIELD( m_hSprite, FIELD_EHANDLE ),
	DEFINE_FIELD( m_bLaserOn, FIELD_BOOLEAN ),

	DEFINE_AUTO_ARRAY( m_hBeam1, FIELD_EHANDLE ),
	DEFINE_AUTO_ARRAY( m_hBeam2, FIELD_EHANDLE ),

	DEFINE_KEYFIELD( m_bStartLaserOff, FIELD_BOOLEAN, "StartState" ),
//	DEFINE_KEYFIELD( m_bDeadly, FIELD_BOOLEAN, "LethalDamage" ),

	DEFINE_INPUTFUNC( FIELD_VOID, "TurnOn", InputTurnOn ),
	DEFINE_INPUTFUNC( FIELD_VOID, "TurnOff", InputTurnOff ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Toggle", InputToggle ),

END_DATADESC()

void CPropLaser::Precache( void )
{
	PrecacheModel( ENTITY_MODEL );
	PrecacheModel( ENITIY_BEAM );
	PrecacheModel( ENITIY_SPRITE );

	PrecacheScriptSound( ENITIY_SOUND );

	BaseClass::Precache();
	
}

void CPropLaser::Spawn( void )
{
	Precache();

	SetModel( ENTITY_MODEL );

	VPhysicsInitStatic();
	SetupLaser();
	SetSolid( SOLID_VPHYSICS );

	// If we are told to start off, don't turn on.
	if (!m_bStartLaserOff)
	{
		TurnOn();
	}
}

void CPropLaser::CreateSounds()
{
	if ( !m_pLoopSound )
	{
		CSoundEnvelopeController &controller = CSoundEnvelopeController::GetController();

		CReliableBroadcastRecipientFilter filter;

		m_pLoopSound = controller.SoundCreate( filter, entindex(), ENITIY_SOUND );
		controller.Play( m_pLoopSound, 0.5, 100 );
	}
}

void CPropLaser::StopLoopingSounds()
{
	if ( m_pLoopSound )
	{
		CSoundEnvelopeController &controller = CSoundEnvelopeController::GetController();

		controller.SoundDestroy( m_pLoopSound );
		m_pLoopSound = NULL;
	}

	BaseClass::StopLoopingSounds();
}

void CPropLaser::SetupLaser( void )
{
	int		attachment;
	attachment = LookupAttachment( ATTACHMENT0 );

	m_hLaser = (CEnvLaser *) CreateEntityByName( "env_laser" );

	if ( m_hLaser != NULL )
	{
		Vector vecAbsOrigin = GetAbsOrigin();
		QAngle angles = GetAbsAngles();
		
		if( attachment > -1 )
		{
			GetAttachment( attachment, vecAbsOrigin );
			m_hLaser->SetAbsOrigin( vecAbsOrigin );
			m_hLaser->SetAbsAngles( angles );
			m_hLaser->SetOwnerEntity(this);

			m_hLaser->KeyValue( "texture", ENITIY_BEAM );
			m_hLaser->KeyValue( "endsprite", ENITIY_SPRITE );

			/*
			if (m_bDeadly)
			{
				m_hLaser->KeyValue( "damage", "1" );
			}
			else
			{
				m_hLaser->KeyValue( "damage", "100" );
			}
			*/

			m_hLaser->KeyValue( "damage", "0" );
			m_hLaser->KeyValue( "dissolvetype", "-1" );
			m_hLaser->KeyValue( "framestart", "0" );
			m_hLaser->KeyValue( "infinite", "1" );
			m_hLaser->KeyValue( "NoiseAmplitude", "0" );
			m_hLaser->KeyValue( "renderamt", "0" );
			m_hLaser->KeyValue( "rendercolor", "0 148 255" );
			m_hLaser->KeyValue( "renderfx", "0" );
			m_hLaser->KeyValue( "spawnflags", "0" );
			m_hLaser->KeyValue( "TextureScroll", "35" );
			m_hLaser->KeyValue( "width", "24" );

			DispatchSpawn( m_hLaser );

			m_hLaser->TurnOff();
		}
	}

	m_hParticleEffect = (CParticleSystem *) CreateEntityByName( "info_particle_system" );
	if ( m_hParticleEffect != NULL )
	{
		Vector vecAbsOrigin = GetAbsOrigin();
		QAngle angles = GetAbsAngles();

		GetAttachment( attachment, vecAbsOrigin );
		m_hParticleEffect->SetAbsOrigin( vecAbsOrigin );
		m_hParticleEffect->SetAbsAngles( angles );
		m_hParticleEffect->SetParent(this);
		m_hParticleEffect->KeyValue( "start_active", "0" );
		m_hParticleEffect->KeyValue( "effect_name", ENTITY_PARTICLE );
		DispatchSpawn( m_hParticleEffect );
		m_hParticleEffect->Activate();

	}

	CBaseEntity *pBeamEnt = this;

	if (m_hBeam1 != NULL )
	{
		m_hBeam1 = CBeam::BeamCreate( ENITIY_BEAM, 16.0f );
		m_hBeam1->EntsInit( pBeamEnt, pBeamEnt );

		int	startAttachment1 = LookupAttachment( ATTACHMENT1 );
		int endAttachment1	= LookupAttachment( ATTACHMENT2 );

		m_hBeam1->FollowEntity( pBeamEnt );
		m_hBeam1->SetStartAttachment( startAttachment1 );
		m_hBeam1->SetEndAttachment( endAttachment1 );
		m_hBeam1->SetColor( 0, 148, 255 );
		m_hBeam1->LiveForTime(0);
		m_hBeam1->TurnOff();
	}

	if (m_hBeam2 != NULL )
	{
		m_hBeam2 = CBeam::BeamCreate( ENITIY_BEAM, 16.0f );
		m_hBeam2->EntsInit( pBeamEnt, pBeamEnt );

		int	startAttachment2 = LookupAttachment( ATTACHMENT1 );
		int endAttachment2	= LookupAttachment( ATTACHMENT2 );

		m_hBeam2->FollowEntity( pBeamEnt );
		m_hBeam2->SetStartAttachment( startAttachment2 );
		m_hBeam2->SetEndAttachment( endAttachment2 );
		m_hBeam2->SetColor( 0, 148, 255 );
		m_hBeam2->LiveForTime(0);
		m_hBeam2->TurnOff();
	}


	bool bSpawnedSprite = false; // Make sure we only spawn once.

	if (!bSpawnedSprite )
	{
		m_hSprite = CSprite::SpriteCreate( ENITIY_SPRITE, GetAbsOrigin(), false );
		m_hSprite->SetAttachment( this, LookupAttachment( ATTACHMENT0 ) );
		m_hSprite->SetTransparency( kRenderWorldGlow, 0, 148, 255, 255, kRenderFxNone );
		m_hSprite->SetScale( 1.0f );

		bSpawnedSprite = true;

		m_hSprite->TurnOff();
	}

}

void CPropLaser::TurnOn()
{
	if (m_hLaser == NULL || m_hParticleEffect == NULL)
	{
		DevMsg("CPortalLaser: Could not find our handles, retrying.\n");
		SetupLaser();
	}

	if (!m_bLaserOn)
	{
		m_hLaser->TurnOn();
		m_hSprite->TurnOn();
		m_hParticleEffect->StartParticleSystem();

		CreateSounds();
		//SetNextThink(gpGlobals->curtime);

		m_hBeam1->TurnOff();
		m_hBeam2->TurnOn();

		m_bLaserOn = true;
	}
}

void CPropLaser::TurnOff()
{
	if (m_bLaserOn)
	{
		m_hLaser->TurnOff();
		m_hSprite->TurnOff();
		m_hParticleEffect->StopParticleSystem();

		StopLoopingSounds();

		m_hBeam1->TurnOff();
		m_hBeam2->TurnOn();

		m_bLaserOn = false;
	}

}

void CPropLaser::InputTurnOn ( inputdata_t &inputData )
{
	TurnOn();
}

void CPropLaser::InputTurnOff ( inputdata_t &inputData )
{
	TurnOff();
	StopLoopingSounds();
}

void CPropLaser::InputToggle ( inputdata_t &inputData )
{
	if (!m_bLaserOn)
	{
		TurnOn();
	}
	else
	{
		TurnOff();
		StopLoopingSounds();
	}
}
