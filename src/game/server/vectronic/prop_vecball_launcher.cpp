//=========== Copyright © 2014, rHetorical, All rights reserved. =============
//
// Purpose: 
//		
//=============================================================================

#include "cbase.h"
#include "vectronic_vecall_system.h"
#include "prop_vectronic_projectile.h"
#include "baseanimating.h"
#include "particle_parse.h"
#include "particle_system.h"
#include "soundent.h"
#include "engine/ienginesound.h"
#include "soundenvelope.h"
#include "sprite.h"
#include "game.h"

// Our sharedefs.
#include "vectronic_shareddefs.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define ENTITY_MODEL	"models/props/ballshoot.mdl"
#define ENTITY_SOUND	"VecLauncher.Fire"

#define SPRITE			 "sprites/light_glow03.vmt"

//-----------------------------------------------------------------------------
// Purpose: Base
//-----------------------------------------------------------------------------
class CPropVecBallLauncher : public CVectronicProp
{
	public:
	DECLARE_CLASS( CPropVecBallLauncher, CVectronicProp );
	DECLARE_DATADESC();

	//Constructor
	CPropVecBallLauncher()
	{
		m_flLife = 1;
	}

	void Spawn( void );
	void Precache( void );
	void Think( void );

	virtual void RefreshParticleColors();
	virtual void RefreshSpriteColors();

	void SetupParticle();
	void SetupSprite();
	void MakeFlash();
	void PreFireBall();
	void FireBall();

	// Input functions.
	void InputLaunchBall( inputdata_t &inputData);

private:

	//int m_intType;
	float m_flLife;
	float m_flConeDegrees;
	bool m_bSpawnedSprite;
	bool m_bMadeFlash;

	CHandle<CParticleSystem> m_hParticleEffect;
	CHandle<CParticleSystem> m_hParticleEffect2;
	CHandle<CSprite>		m_hSprite;
	COutputEvent	m_OnPostSpawnBall;

};

LINK_ENTITY_TO_CLASS( prop_vecball_launcher, CPropVecBallLauncher );

// Start of our data description for the class
BEGIN_DATADESC( CPropVecBallLauncher )
 
	//Save/load
	DEFINE_FIELD( m_hParticleEffect, FIELD_EHANDLE ),
	DEFINE_FIELD( m_hParticleEffect2, FIELD_EHANDLE ),
	DEFINE_FIELD( m_bMadeFlash, FIELD_BOOLEAN ),

	//DEFINE_KEYFIELD( m_intType, FIELD_INTEGER, "skin"),
	DEFINE_KEYFIELD(m_flLife, FIELD_FLOAT, "Life"),

	DEFINE_INPUTFUNC( FIELD_VOID, "LaunchBall", InputLaunchBall ),

	DEFINE_OUTPUT( m_OnPostSpawnBall, "OnPostSpawnProjectile" ),

END_DATADESC()

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPropVecBallLauncher::Precache( void )
{
	PrecacheModel( ENTITY_MODEL );
	PrecacheScriptSound( ENTITY_SOUND );
	PrecacheModel( SPRITE );

	PrecacheParticleSystem( BALL_PARTICLE_NODES );
	PrecacheParticleSystem( BALL_GLOW);

	UTIL_PrecacheOther( "prop_particle_ball" );

	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose: Sets up the entity's initial state
//-----------------------------------------------------------------------------
void CPropVecBallLauncher::Spawn( void )
{
	Precache();
	SetModel( ENTITY_MODEL );
	SetSolid( SOLID_VPHYSICS );

//	SetCollisionGroup(COLLISION_GROUP_VPHYSICS);

	// Make it soild to other entities.
	VPhysicsInitStatic();

	SetRenderColorToBalls();

	SetSequence( LookupSequence("idle") );
	SetupParticle();

	// 09/16/14 - New animation frame makes the shadows.... ECH!
	AddEffects(EF_NOSHADOW);

	if(!m_bSpawnedSprite)
	{
		SetupSprite();
	}

	BaseClass::Spawn();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPropVecBallLauncher::SetupSprite()
{
	if (m_bSpawnedSprite)
		return;

	m_hSprite = CSprite::SpriteCreate( SPRITE, GetAbsOrigin(), false );
	
	m_hSprite->SetAttachment( this, LookupAttachment( "muzzle2" ) );

	m_hSprite->SetTransparency( kRenderWorldGlow, 255, 255, 255, 255, kRenderFxNone );
	m_hSprite->SetBrightness( 255, 0.2f );
	m_hSprite->SetScale( 0.30f ); //10
	RefreshSpriteColors();
	m_hSprite->TurnOn();

	m_bSpawnedSprite = true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPropVecBallLauncher::RefreshSpriteColors()
{
	if (m_hSprite != NULL)
	{
		m_hSprite->SetRenderColor( GetOurBallColor_R(), GetOurBallColor_G(), GetOurBallColor_B() );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPropVecBallLauncher::MakeFlash()
{
	m_hParticleEffect2 = (CParticleSystem *) CreateEntityByName( "info_particle_system" );
	if ( m_hParticleEffect2 != NULL )
	{
		Vector vecAbsOrigin = GetAbsOrigin();
		QAngle angles = GetAbsAngles();

		//DevMsg("CPropVecBallLauncher: Making Flash Particle\n");
			
		int		attachment;
		attachment = LookupAttachment( "muzzle2" );

		GetAttachment( attachment, vecAbsOrigin );
		m_hParticleEffect2->SetAbsOrigin( vecAbsOrigin );
		m_hParticleEffect2->SetAbsAngles( angles );
		m_hParticleEffect2->SetParent(this);
		m_hParticleEffect2->KeyValue( "start_active", "0" );
		m_hParticleEffect2->KeyValue( "effect_name", BALL_GLOW );
		DispatchSpawn( m_hParticleEffect2 );

		RefreshParticleColors();

		m_hParticleEffect2->Activate();
		m_hParticleEffect2->StartParticleSystem();

	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPropVecBallLauncher::SetupParticle()
{
	m_hParticleEffect = (CParticleSystem *) CreateEntityByName( "info_particle_system" );
	if ( m_hParticleEffect != NULL )
	{
		Vector vecAbsOrigin = GetAbsOrigin();
		QAngle angles = GetAbsAngles();
			
		int		attachment;
		attachment = LookupAttachment( "muzzle2" );

		GetAttachment( attachment, vecAbsOrigin );
		m_hParticleEffect->SetAbsOrigin( vecAbsOrigin );
		m_hParticleEffect->SetAbsAngles( angles );
		m_hParticleEffect->SetParent(this);
		m_hParticleEffect->KeyValue( "start_active", "1" );
		m_hParticleEffect->KeyValue( "effect_name", BALL_PARTICLE_NODES );

		RefreshParticleColors();

		DispatchSpawn( m_hParticleEffect );
		m_hParticleEffect->Activate();

	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPropVecBallLauncher::RefreshParticleColors()
{
	if (m_hParticleEffect != NULL && m_hParticleEffect2 != NULL)
	{
		Vector ParticleColor (GetOurBallColor_R(), GetOurBallColor_G(), GetOurBallColor_B());
		m_hParticleEffect->SetControlPointValue( 7, ParticleColor );
		m_hParticleEffect2->SetControlPointValue( 7, ParticleColor );
	}
	else if (m_hParticleEffect != NULL)
	{
		Vector ParticleColor (GetOurBallColor_R(), GetOurBallColor_G(), GetOurBallColor_B());
		m_hParticleEffect->SetControlPointValue( 7, ParticleColor );
	}
}

//-----------------------------------------------------------------------------
// Purpose: "Then I flip a switch..."
//-----------------------------------------------------------------------------
void CPropVecBallLauncher::InputLaunchBall( inputdata_t &inputData)
{
	PreFireBall();
	m_OnPostSpawnBall.FireOutput( inputData.pActivator, this );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPropVecBallLauncher::PreFireBall()
{
	// 8-17-14: We have an issue here when we port this to ASW.
	// Prop does not animate correctly. 

#define NEW_ANIMATION

#ifdef NEW_ANIMATION
	int nSequence = LookupSequence( "in" );
	ResetSequence( nSequence );
	SetPlaybackRate( 1.0f );
	UseClientSideAnimation();
#else
	int nSequence = LookupSequence( "fire" );
	ResetSequence( nSequence );
	SetPlaybackRate( 1.0f );
	UseClientSideAnimation();
	ResetClientsideFrame();
#endif

	// If this exists, delete it.
	UTIL_Remove(m_hParticleEffect2);

	// Wait a tad bit for actual firing.
	SetNextThink( gpGlobals->curtime + 0.18 );
}

//-----------------------------------------------------------------------------
// Purpose: Precache assets used by the entity
//-----------------------------------------------------------------------------
void CPropVecBallLauncher::Think( void )
{
	// Play Fire sound.
	EmitSound( ENTITY_SOUND );
	//m_hParticleEffect->StopParticleSystem();

#ifdef NEW_ANIMATION
	int nSequence = LookupSequence( "idle" );
	ResetSequence( nSequence );
	SetPlaybackRate( 1.0f );
	UseClientSideAnimation();
#endif

	FireBall();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPropVecBallLauncher::FireBall()
{
	CPropParticleBall *pBall = static_cast<CPropParticleBall*>( CreateEntityByName( "prop_particle_ball" ) );

	if ( pBall == NULL )
		 return;

	Vector vecAbsOrigin = GetAbsOrigin();
	QAngle angles = GetAbsAngles();
			
	int		attachment;
	attachment = LookupAttachment( "muzzle" );

	GetAttachment( attachment, vecAbsOrigin );
	Vector zaxis;

	//GetAttachment( LookupAttachment( "muzzle" ), vecAbsOrigin, GetAbsAngles() );
	//DispatchParticleEffect( "projectile_ball_warp", PATTACH_POINT_FOLLOW, this, attachment );

	MakeFlash();

	pBall->SetAbsOrigin( vecAbsOrigin );

	//	pBall->SetRadius( radius );
	pBall->SetRadius( 8 );

	pBall->SetAbsOrigin( vecAbsOrigin );
	pBall->SetOwnerEntity( this );

	Vector vDirection;
	QAngle qAngle = GetAbsAngles();

	qAngle = qAngle + QAngle ( random->RandomFloat( -m_flConeDegrees, m_flConeDegrees ), random->RandomFloat( -m_flConeDegrees, m_flConeDegrees ), 0 );

	AngleVectors( qAngle, &vDirection, NULL, NULL );

	vDirection *= 1000.0f;
	pBall->SetAbsVelocity( vDirection );

//	Vector vecVelocity = 500.0f;
//	pBall->SetAbsVelocity( vecVelocity );
	pBall->Spawn();

	pBall->SetState( CPropParticleBall::STATE_THROWN );
	pBall->SetSpeed( vDirection.Length() );

	pBall->EmitSound( "ParticleBall.Launch" );

	PhysSetGameFlags( pBall->VPhysicsGetObject(), FVPHYSICS_WAS_THROWN );

//	pBall->StartWhizSoundThink();

	pBall->SetMass( 0.1f );

	pBall->StartLifetime( m_flLife );

	pBall->SetWeaponLaunched( true );

	pBall->SetType ( GetOurType() );

//	return pBall;
}