//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Vectronic Projectile.
//
//=============================================================================//

#include "cbase.h"
#include "prop_vectronic_projectile.h"
#include "props.h"
#include "explode.h"
#include "saverestore_utlvector.h"
#include "vectronic_shareddefs.h"
#include "materialsystem/imaterial.h"
#include "beam_flags.h"
#include "physics_prop_ragdoll.h"
#include "soundent.h"
#include "soundenvelope.h"
#include "te_effect_dispatch.h"
#include "ai_basenpc.h"
#include "filters.h"
#include "SpriteTrail.h"
#include "decals.h"
#include "vectronic_player.h"
#include "player.h"
#include "eventqueue.h"
#include "physics_collisionevent.h"
#include "GameStats.h"
#include "particle_parse.h"
#include "model_types.h"
#include "vectronic_gamerules.h"
#include "prop_vecbox.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern ConVar vectronic_vecball3_color_r;
extern ConVar vectronic_vecball3_color_g;
extern ConVar vectronic_vecball3_color_b;

extern ConVar vectronic_vecball4_color_r;
extern ConVar vectronic_vecball4_color_g;
extern ConVar vectronic_vecball4_color_b;

extern ConVar vectronic_vecball5_color_r;
extern ConVar vectronic_vecball5_color_g;
extern ConVar vectronic_vecball5_color_b;

ConVar	sk_npc_dmg_particleball("sk_npc_dmg_particleball", "0", FCVAR_REPLICATED);
ConVar	sk_particleball_guidefactor("sk_particleball_guidefactor", "0.5", FCVAR_REPLICATED);
ConVar	sk_particle_ball_search_radius("sk_particle_ball_search_radius", "512", FCVAR_REPLICATED);
ConVar	sk_particleball_seek_angle("sk_particleball_seek_angle", "15.0", FCVAR_REPLICATED);
ConVar	sk_particleball_seek_hit("sk_particleball_seek_hit", "1", FCVAR_REPLICATED);
ConVar	sk_particleball_size("sk_particleball_size", "8", FCVAR_REPLICATED);
ConVar	sk_particleball_draw_particle("sk_particleball_draw_particle", "1", FCVAR_REPLICATED);

// Ok, Here is a quick tag telling our ball NOT to use particles
// We are going to disable particles for now to simply get them 
// Out of sight while we focus on other things. This will also
// lower issues if we want to downgrade if nessarry. 
//#define NO_PARTICLE_EFFECTS

//Model. Not Drawn until Pickup. (Which we will never do!)
#define PROP_PARTICLE_BALL_MODEL	"models/effects/combineball.mdl"


//Tails
#define PROP_PARTICLE_SPRITE0_TRAIL BALL0_SPRITE_TAPE
#define PROP_PARTICLE_SPRITE1_TRAIL BALL1_SPRITE_TAPE
#define PROP_PARTICLE_SPRITE2_TRAIL BALL2_SPRITE_TAPE

//Particles Used To Explode
#define PROP_PARTICLE_BALL_HIT			"vecgun_shared_charge_glow"

//We hit a non-bounceable surface!
#define PROP_PARTICLE_BALL_HIT_WARP		"projectile_ball_warp"

#define PROP_PARTICLE_BALL_LIFETIME	0.1f	// Seconds
#define PROP_PARTICLE_BALL_TYPE	0	// Seconds

#define PROP_PARTICLE_BALL_HOLD_DISSOLVE_TIME	8.0f
#define	MAX_PARTICLEBALL_RADIUS	12


//-----------------------------------------------------------------------------
// Context think
//-----------------------------------------------------------------------------
static const char *s_pWhizThinkContext = "WhizThinkContext";
static const char *s_pHoldDissolveContext = "HoldDissolveContext";
static const char *s_pExplodeTimerContext = "ExplodeTimerContext";
static const char *s_pAnimThinkContext = "AnimThinkContext";
static const char *s_pCaptureContext = "CaptureContext";
static const char *s_pRemoveContext = "RemoveContext";

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : radius - 
// Output : CBaseEntity
//-----------------------------------------------------------------------------
CBaseEntity *CreateParticleBall0( const Vector &origin, const Vector &velocity, float mass, float type, float lifetime, CBaseEntity *pOwner )
{
	CPropParticleBall *pBall = static_cast<CPropParticleBall*>( CreateEntityByName( "prop_particle_ball" ) );
//	pBall->SetRadius( radius );
	pBall->SetRadius( sk_particleball_size.GetFloat() );

	pBall->SetAbsOrigin( origin );
	pBall->SetOwnerEntity( pOwner );
	pBall->SetOriginalOwner( pOwner );

	pBall->SetAbsVelocity( velocity );
	pBall->Spawn();

	pBall->SetState( CPropParticleBall::STATE_THROWN );
	pBall->SetSpeed( velocity.Length() );

//	pBall->EmitSound( "NPC_CombineBall.Launch" );

	PhysSetGameFlags( pBall->VPhysicsGetObject(), FVPHYSICS_WAS_THROWN );

//	pBall->StartWhizSoundThink();

	pBall->SetMass( mass );

	pBall->StartLifetime( lifetime );
	pBall->SetType ( type );
	pBall->SetWeaponLaunched( true );

	return pBall;
}

//-----------------------------------------------------------------------------
// Purpose: Allows game to know if the physics object should kill allies or not
//-----------------------------------------------------------------------------
CBasePlayer *CPropParticleBall::HasPhysicsAttacker( float dt )
{
	// Must have an owner
	if ( GetOwnerEntity() == NULL )
		return false;

	// Must be a player
	if ( GetOwnerEntity()->IsPlayer() == false )
		return false;

	// We don't care about the time passed in
	return static_cast<CBasePlayer *>(GetOwnerEntity());
}

//-----------------------------------------------------------------------------
// Purpose: Determines whether a physics object is a combine ball or not
// Input  : *pObj - Object to test
// Output : Returns true on success, false on failure.
// Notes  : This function cannot identify a combine ball that is held by
//			the physcannon because any object held by the physcannon is
//			COLLISIONGROUP_DEBRIS.
//-----------------------------------------------------------------------------
bool UTIL_IsParticleBall( CBaseEntity *pEntity )
{
	// Must be the correct collision group
	if ( pEntity->GetCollisionGroup() != COLLISION_GROUP_PROJECTILE )
		return false;

	//NOTENOTE: This allows ANY combine ball to pass the test

	/*
	CPropParticleBall *pBall = dynamic_cast<CPropParticleBall *>(pEntity);

	if ( pBall && pBall->WasWeaponLaunched() )
		return false;
	*/

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: Determines whether a physics object is an AR2 combine ball or not
// Input  : *pEntity - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool UTIL_IsAR2ParticleBall( CBaseEntity *pEntity )
{
	// Must be the correct collision group
	if ( pEntity->GetCollisionGroup() != COLLISION_GROUP_PROJECTILE )
		return false;

	CPropParticleBall *pBall = dynamic_cast<CPropParticleBall *>(pEntity);

	if ( pBall && pBall->WasWeaponLaunched() )
		return true;

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: Uses a deeper casting check to determine if pEntity is a combine
//			ball. This function exists because the normal (much faster) check
//			in UTIL_IsCombineBall() can never identify a combine ball held by
//			the physcannon because the physcannon changes the held entity's
//			collision group.
// Input  : *pEntity - Entity to check 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool UTIL_IsParticleBallDefinite( CBaseEntity *pEntity )
{
	CPropParticleBall *pBall = dynamic_cast<CPropParticleBall *>(pEntity);

	return pBall != NULL;
}

//-----------------------------------------------------------------------------
//
// Spawns combine balls
//
//-----------------------------------------------------------------------------
#define SF_SPAWNER_START_DISABLED 0x1000
#define SF_SPAWNER_POWER_SUPPLY 0x2000



//-----------------------------------------------------------------------------
// Implementation of CPropParticleBall
//-----------------------------------------------------------------------------

// We need 3 seporate entities because the boxes can't get the info from the entity.
LINK_ENTITY_TO_CLASS( prop_particle_ball, CPropParticleBall );

//-----------------------------------------------------------------------------
// Save/load: 
//-----------------------------------------------------------------------------
BEGIN_DATADESC( CPropParticleBall )

	DEFINE_FIELD( m_flLastBounceTime, FIELD_TIME ),
	DEFINE_FIELD( m_flRadius, FIELD_FLOAT ),
	DEFINE_FIELD( m_nState, FIELD_CHARACTER ),
	DEFINE_FIELD( m_pGlowTrail, FIELD_CLASSPTR ),
	DEFINE_SOUNDPATCH( m_pHoldingSound ),
	DEFINE_FIELD( m_bFiredGrabbedOutput, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_bEmit, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_bHeld, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_bLaunched, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_bStruckEntity, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_bWeaponLaunched, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_bForward, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_flSpeed, FIELD_FLOAT ),

	DEFINE_FIELD( m_flNextDamageTime, FIELD_TIME ),
	DEFINE_FIELD( m_flLastCaptureTime, FIELD_TIME ),
	DEFINE_FIELD( m_bCaptureInProgress, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_nBounceCount,	FIELD_INTEGER ),
	DEFINE_FIELD( m_nMaxBounces,	FIELD_INTEGER ),
	DEFINE_FIELD( m_intType,	FIELD_INTEGER ),
	DEFINE_FIELD( m_bBounceDie,	FIELD_BOOLEAN ),
	
	
//	DEFINE_FIELD( m_hSpawner, FIELD_EHANDLE ),

	DEFINE_THINKFUNC( ExplodeThink ),
	DEFINE_THINKFUNC( WhizSoundThink ),
	DEFINE_THINKFUNC( DieThink ),
	DEFINE_THINKFUNC( DissolveThink ),
	DEFINE_THINKFUNC( DissolveRampSoundThink ),
	DEFINE_THINKFUNC( AnimThink ),
	DEFINE_THINKFUNC( CaptureBySpawner ),

	DEFINE_FIELD( m_hParticleEffect, FIELD_EHANDLE ),
	DEFINE_FIELD( m_hParticleEffect2, FIELD_EHANDLE ),

	DEFINE_INPUTFUNC( FIELD_VOID, "Explode", InputExplode ),
	DEFINE_INPUTFUNC( FIELD_VOID, "FadeAndRespawn", InputFadeAndRespawn ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Kill", InputKill ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Socketed", InputSocketed ),

END_DATADESC()

IMPLEMENT_SERVERCLASS_ST( CPropParticleBall, DT_PropParticleBall )
	SendPropBool( SENDINFO( m_bEmit ) ),
	SendPropFloat( SENDINFO( m_flRadius ), 0, SPROP_NOSCALE ),
	SendPropBool( SENDINFO( m_bHeld ) ),
	SendPropInt( SENDINFO( m_intType ) ),
	SendPropBool( SENDINFO( m_bLaunched ) ),
END_SEND_TABLE()

//-----------------------------------------------------------------------------
// Precache 
//-----------------------------------------------------------------------------
void CPropParticleBall::Precache( void )
{
	//NOTENOTE: We don't call into the base class because it chains multiple 
	//			precaches we don't need to incur

	PrecacheModel( PROP_PARTICLE_BALL_MODEL );
	PrecacheModel( BALL_SPRITE_TRAIL );

//	PrecacheModel( PROP_PARTICLE_SPRITE0_TRAIL );
//	PrecacheModel( PROP_PARTICLE_SPRITE1_TRAIL );
//	PrecacheModel( PROP_PARTICLE_SPRITE2_TRAIL );

	PrecacheParticleSystem( BALL_GLOW );
	PrecacheParticleSystem( PROP_PARTICLE_BALL_HIT_WARP );

	PrecacheScriptSound( "ParticleBall.Launch" );
	PrecacheScriptSound( "ParticleBall.KillImpact" );
	PrecacheScriptSound( "ParticleBall.AmbientLoop" );

	PrecacheScriptSound( "ParticleBall.Explosion" );
	PrecacheScriptSound( "ParticleBall.WhizFlyby" );
	PrecacheScriptSound( "ParticleBall.Impact" );

	//PrecacheScriptSound( "NPC_CombineBall.HoldingInPhysCannon" );

#ifdef SWARM_DLL
	// Define our game's balls.
	PrecacheEffect( "cball_blue_bounce");
	PrecacheEffect( "cball_green_bounce");
	PrecacheEffect( "cball_purple_bounce");

	// Define custom stuff.
	PrecacheEffect( "cball_3_bounce");
	PrecacheEffect( "cball_4_bounce");
	PrecacheEffect( "cball_5_bounce");

	PrecacheEffect( "cball_explode");
#endif
}


//-----------------------------------------------------------------------------
// Spherical vphysics
//-----------------------------------------------------------------------------
bool CPropParticleBall::OverridePropdata() 
{ 
	return true; 
}


//-----------------------------------------------------------------------------
// Spherical vphysics
//-----------------------------------------------------------------------------
void CPropParticleBall::SetState( int state ) 
{ 
	if ( m_nState != state )
	{
		if ( m_nState == STATE_NOT_THROWN )
		{
			m_flLastCaptureTime = gpGlobals->curtime;
		}

		m_nState = state;
	}
}

bool CPropParticleBall::IsInField() const 
{ 
	return (m_nState == STATE_NOT_THROWN); 
}

	
//-----------------------------------------------------------------------------
// Sets the radius
//-----------------------------------------------------------------------------
void CPropParticleBall::SetRadius( float flRadius )
{
	m_flRadius = clamp( flRadius, 1, MAX_PARTICLEBALL_RADIUS );
}

//-----------------------------------------------------------------------------
// Create vphysics
//-----------------------------------------------------------------------------
bool CPropParticleBall::CreateVPhysics()
{
	SetSolid( SOLID_BBOX );

	float flSize = m_flRadius;

	SetCollisionBounds( Vector(-flSize, -flSize, -flSize), Vector(flSize, flSize, flSize) );
	objectparams_t params = g_PhysDefaultObjectParams;
	params.pGameData = static_cast<void *>(this);
	int nMaterialIndex = physprops->GetSurfaceIndex("particleball");
	IPhysicsObject *pPhysicsObject = physenv->CreateSphereObject( flSize, nMaterialIndex, GetAbsOrigin(), GetAbsAngles(), &params, false );
	if ( !pPhysicsObject )
		return false;

	VPhysicsSetObject( pPhysicsObject );
	SetMoveType( MOVETYPE_VPHYSICS );
	pPhysicsObject->Wake();

//	pPhysicsObject->SetMass( 750.0f );
	pPhysicsObject->SetMass( 0.1f );
	pPhysicsObject->EnableGravity( false );
	pPhysicsObject->EnableDrag( false );

	float flDamping = 0.0f;
	float flAngDamping = 0.5f;
	pPhysicsObject->SetDamping( &flDamping, &flAngDamping );
	pPhysicsObject->SetInertia( Vector( 1e30, 1e30, 1e30 ) );

	if( WasFiredByNPC() )
	{
		// Don't do impact damage. Just touch them and do your dissolve damage and move on.
		PhysSetGameFlags( pPhysicsObject, FVPHYSICS_NO_NPC_IMPACT_DMG );
	}
	else
	{
	//	PhysSetGameFlags( pPhysicsObject, FVPHYSICS_DMG_DISSOLVE | FVPHYSICS_HEAVY_OBJECT );
		PhysSetGameFlags( pPhysicsObject, FVPHYSICS_NO_NPC_IMPACT_DMG );
	}

	return true;
}


//-----------------------------------------------------------------------------
// Spawn: 
//-----------------------------------------------------------------------------
void CPropParticleBall::Spawn( void )
{
	Precache();
	BaseClass::Spawn();

	// We spawn the models and particles at SetType().

	EmitSound ("ParticleBall.AmbientLoop");
	/*
	if( ShouldHitPlayer() )
	{
		// This allows the combine ball to hit the player.
		SetCollisionGroup( COLLISION_GROUP_PROJECTILE_NPC );
	}
	else
	{
	*/
		SetCollisionGroup( COLLISION_GROUP_PROJECTILE );
	//}

	CreateVPhysics();

	Vector vecAbsVelocity = GetAbsVelocity();
	VPhysicsGetObject()->SetVelocity( &vecAbsVelocity, NULL );

	m_nState = STATE_NOT_THROWN;
	m_flLastBounceTime = -1.0f;
	m_bFiredGrabbedOutput = false;
	m_bForward = true;
	m_bCaptureInProgress = false;

	// No shadow!
	AddEffects( EF_NOSHADOW );
	
	if ( m_pGlowTrail != NULL )
	{
		m_pGlowTrail->FollowEntity( this );
		m_pGlowTrail->SetTransparency( kRenderTransAdd, 0, 0, 0, 255, kRenderFxNone );
		m_pGlowTrail->SetStartWidth( m_flRadius );
		m_pGlowTrail->SetEndWidth( 0 );
		m_pGlowTrail->SetLifeTime( 1.0f );
		m_pGlowTrail->TurnOff();
	}

	m_bEmit = true;
	m_bHeld = false;
	m_bLaunched = false;
	m_bStruckEntity = false;
	m_bWeaponLaunched = false;
	MakeParticleSystem();
	m_flNextDamageTime = gpGlobals->curtime;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPropParticleBall::MakeParticleSystem()
{
	m_hParticleEffect = (CParticleSystem *) CreateEntityByName( "info_particle_system" );
	if ( m_hParticleEffect != NULL )
	{
		QAngle angles = GetAbsAngles();
		// Setup our basic parameters
		//m_hParticleEffect->KeyValue( "start_active", "1" );
		m_hParticleEffect->KeyValue( "effect_name", BALL_PARTICLE_TRIAL	);
		m_hParticleEffect->SetParent( this );
		m_hParticleEffect->SetAbsOrigin( WorldSpaceCenter());
		m_hParticleEffect->SetAbsAngles( angles );
		DispatchSpawn( m_hParticleEffect );

		if (sk_particleball_draw_particle.GetBool())
		{
			m_hParticleEffect->Activate();
			m_hParticleEffect->StartParticleSystem();
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPropParticleBall::MakeHitParticleSystem()
{
	m_hParticleEffect2 = (CParticleSystem *) CreateEntityByName( "info_particle_system" );
	if ( m_hParticleEffect2 != NULL )
	{
		QAngle angles = GetAbsAngles();
		// Setup our basic parameters
		//m_hParticleEffect2->KeyValue( "start_active", "1" );
		m_hParticleEffect2->KeyValue( "effect_name", PROP_PARTICLE_BALL_HIT );
		m_hParticleEffect2->SetParent( this );
		m_hParticleEffect2->SetAbsOrigin( WorldSpaceCenter());
		m_hParticleEffect2->SetAbsAngles( angles );
		DispatchSpawn( m_hParticleEffect2 );

		if (sk_particleball_draw_particle.GetBool())
		{
			m_hParticleEffect2->Activate();
			m_hParticleEffect2->StartParticleSystem();
		}
	}
}
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPropParticleBall::StartAnimating( void )
{
	// Start our animation cycle. Use the random to avoid everything thinking the same frame
	SetContextThink( &CPropParticleBall::AnimThink, gpGlobals->curtime + random->RandomFloat( 0.0f, 0.1f), s_pAnimThinkContext );

	int nSequence = LookupSequence( "idle" );

	SetCycle( 0 );
	m_flAnimTime = gpGlobals->curtime;
	ResetSequence( nSequence );
	ResetClientsideFrame();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPropParticleBall::StopAnimating( void )
{
	SetContextThink( NULL, gpGlobals->curtime, s_pAnimThinkContext );
}

//-----------------------------------------------------------------------------
// Put it into the spawner
//-----------------------------------------------------------------------------
void CPropParticleBall::CaptureBySpawner( )
{
	m_bCaptureInProgress = true;
	m_bFiredGrabbedOutput = false;

	// Slow down the ball
	Vector vecVelocity;
	VPhysicsGetObject()->GetVelocity( &vecVelocity, NULL );
	float flSpeed = VectorNormalize( vecVelocity );
	if ( flSpeed > 25.0f )
	{
		vecVelocity *= flSpeed * 0.4f;
		VPhysicsGetObject()->SetVelocity( &vecVelocity, NULL );

		// Slow it down until we can set its velocity ok
		SetContextThink( &CPropParticleBall::CaptureBySpawner, gpGlobals->curtime + 0.01f,	s_pCaptureContext );
		return;
	}

	// Ok, we're captured
	SetContextThink( NULL, gpGlobals->curtime,	s_pCaptureContext );
	m_bCaptureInProgress = false;
}

//-----------------------------------------------------------------------------
// Put it into the spawner
//-----------------------------------------------------------------------------
void CPropParticleBall::ReplaceInSpawner( float flSpeed )
{
	m_bForward = true;
	m_nState = STATE_NOT_THROWN;

	// Prevent it from exploding
	ClearLifetime( );

	// Stop whiz noises
	SetContextThink( NULL, gpGlobals->curtime, s_pWhizThinkContext );

	// Slam velocity to what the field wants
	Vector vecTarget, vecVelocity;
	VectorSubtract( vecTarget, GetAbsOrigin(), vecVelocity );
	VectorNormalize( vecVelocity );
	vecVelocity *= flSpeed; 
	VPhysicsGetObject()->SetVelocity( &vecVelocity, NULL );
	
	// Set our desired speed to the spawner's speed. This will be
	// our speed on our first bounce in the field.
	SetSpeed( flSpeed );
}


float CPropParticleBall::LastCaptureTime() const
{
	if ( IsInField() || IsBeingCaptured() )
		return gpGlobals->curtime;

	return m_flLastCaptureTime;
}

//-----------------------------------------------------------------------------
// Purpose: Starts the lifetime countdown on the ball
// Input  : flDuration - number of seconds to live before exploding
//-----------------------------------------------------------------------------
void CPropParticleBall::StartLifetime( float flDuration )
{
	SetContextThink( &CPropParticleBall::ExplodeThink, gpGlobals->curtime + flDuration, s_pExplodeTimerContext );
}

void CPropParticleBall::GetTail()
{
	// Start up the eye trail
	m_pGlowTrail = CSpriteTrail::SpriteTrailCreate( BALL_SPRITE_TRAIL, GetAbsOrigin(), false );

	if ( m_intType == 0 )
	{
		m_pGlowTrail->SetRenderColor( BALL0_COLOR_R, BALL0_COLOR_B, BALL0_COLOR_B );
	}

	if ( m_intType == 1 )
	{
		m_pGlowTrail->SetRenderColor( BALL1_COLOR_R, BALL1_COLOR_B, BALL1_COLOR_B );
	}

	if ( m_intType == 2 )
	{
		m_pGlowTrail->SetRenderColor( BALL2_COLOR_R, BALL2_COLOR_B, BALL2_COLOR_B );
	}

	if ( m_intType == 3 )
	{
		m_pGlowTrail->SetRenderColor( vectronic_vecball3_color_r.GetInt(), vectronic_vecball3_color_g.GetInt(), vectronic_vecball3_color_b.GetInt() );
	}

	if ( m_intType == 4 )
	{
		m_pGlowTrail->SetRenderColor( vectronic_vecball4_color_r.GetInt(), vectronic_vecball4_color_g.GetInt(), vectronic_vecball4_color_b.GetInt() );
	}

	if ( m_intType == 5 )
	{
		m_pGlowTrail->SetRenderColor( vectronic_vecball5_color_r.GetInt(), vectronic_vecball5_color_g.GetInt(), vectronic_vecball5_color_b.GetInt() );
	}

	m_pGlowTrail->FollowEntity( this );
	m_pGlowTrail->SetTransparency( kRenderTransAdd, 0, 0, 0, 255, kRenderFxNone );
	m_pGlowTrail->SetStartWidth( m_flRadius );
	m_pGlowTrail->SetEndWidth( 0 );
	m_pGlowTrail->SetLifeTime( 0.1f );
	m_pGlowTrail->TurnOn();
}

//-----------------------------------------------------------------------------
// Purpose: Here we set the standards of spawning
//-----------------------------------------------------------------------------
void CPropParticleBall::SetType( int inttype )
{
	//Note: We only spawn the model if it's being picked up by the physcannon. 
	SetModel( PROP_PARTICLE_BALL_MODEL ); 

	m_intType = inttype;

	MakeParticleSystem();

	// Call Tail
	GetTail();

	if (m_intType == 0 )
	{
		Vector ParticleColor (BALL0_COLOR_R, BALL0_COLOR_G, BALL0_COLOR_B);
		m_hParticleEffect->SetControlPointValue( 7, ParticleColor );
	}
	if (m_intType == 1 )
	{
		Vector ParticleColor (BALL1_COLOR_R, BALL1_COLOR_G, BALL1_COLOR_B);
		m_hParticleEffect->SetControlPointValue( 7, ParticleColor );
	}

	if (m_intType == 2 )
	{
		Vector ParticleColor (BALL2_COLOR_R, BALL2_COLOR_G, BALL2_COLOR_B);
		m_hParticleEffect->SetControlPointValue( 7, ParticleColor );
	}

	if (m_intType == 3 )
	{
		Vector ParticleColor (vectronic_vecball3_color_r.GetInt(), vectronic_vecball3_color_g.GetInt(), vectronic_vecball3_color_b.GetInt());
		m_hParticleEffect->SetControlPointValue( 7, ParticleColor );
	}

	if (m_intType == 4 )
	{
		Vector ParticleColor (vectronic_vecball4_color_r.GetInt(), vectronic_vecball4_color_g.GetInt(), vectronic_vecball4_color_b.GetInt());
		m_hParticleEffect->SetControlPointValue( 7, ParticleColor );
	}

	if (m_intType == 5 )
	{
		Vector ParticleColor (vectronic_vecball5_color_r.GetInt(), vectronic_vecball5_color_g.GetInt(), vectronic_vecball5_color_b.GetInt());
		m_hParticleEffect->SetControlPointValue( 7, ParticleColor );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Stops the lifetime on the ball from expiring
//-----------------------------------------------------------------------------
void CPropParticleBall::ClearLifetime( void )
{
	// Prevent it from exploding
	SetContextThink( NULL, gpGlobals->curtime, s_pExplodeTimerContext );
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : mass - 
//-----------------------------------------------------------------------------
void CPropParticleBall::SetMass( float mass )
{
	IPhysicsObject *pObj = VPhysicsGetObject();

	if ( pObj != NULL )
	{
		pObj->SetMass( mass );
		pObj->SetInertia( Vector( 500, 500, 500 ) );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CPropParticleBall::ShouldHitPlayer() const 
{ 
	if ( GetOwnerEntity() ) 
	{
		CAI_BaseNPC *pNPC = GetOwnerEntity()->MyNPCPointer();
		if ( pNPC && !pNPC->IsPlayerAlly() )
		{
			return true;
		}
	}
	return false;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPropParticleBall::InputKill( inputdata_t &inputdata )
{
	// tell owner ( if any ) that we're dead.This is mostly for NPCMaker functionality.
	CBaseEntity *pOwner = GetOwnerEntity();
	if ( pOwner )
	{
		pOwner->DeathNotice( this );
		SetOwnerEntity( NULL );
	}

	if ( m_hParticleEffect != NULL )
	{
		UTIL_Remove( m_hParticleEffect );
	}

	UTIL_Remove( this );

	NotifySpawnerOfRemoval();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPropParticleBall::InputSocketed( inputdata_t &inputdata )
{
	// tell owner ( if any ) that we're dead.This is mostly for NPCMaker functionality.
	CBaseEntity *pOwner = GetOwnerEntity();
	if ( pOwner )
	{
		pOwner->DeathNotice( this );
		SetOwnerEntity( NULL );
	}

	if ( m_hParticleEffect != NULL )
	{
		UTIL_Remove( m_hParticleEffect );
	}
/*
	// if our owner is a player, tell them we were socketed
	CHL2_Player *pPlayer = dynamic_cast<CHL2_Player *>( pOwner );
	if ( pPlayer )
	{
		pPlayer->CombineBallSocketed( this );
	}
*/
	UTIL_Remove( this );

	NotifySpawnerOfRemoval();
}
//-----------------------------------------------------------------------------
// Cleanup. 
//-----------------------------------------------------------------------------
void CPropParticleBall::UpdateOnRemove()
{
	if ( m_pGlowTrail != NULL )
	{
		UTIL_Remove( m_pGlowTrail );
		m_pGlowTrail = NULL;
	}

	BaseClass::UpdateOnRemove();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPropParticleBall::ExplodeThink( void )
{
	DoExplosion();	
}

//-----------------------------------------------------------------------------
// Purpose: Tell the respawner to make a new one
//-----------------------------------------------------------------------------
void CPropParticleBall::NotifySpawnerOfRemoval( void )
{

}

//-----------------------------------------------------------------------------
// Fade out. 
//-----------------------------------------------------------------------------
void CPropParticleBall::DieThink()
{
	if ( m_hParticleEffect != NULL )
	{
		UTIL_Remove( m_hParticleEffect );
	}

	UTIL_Remove( this );
}


//-----------------------------------------------------------------------------
// Fade out. 
//-----------------------------------------------------------------------------
void CPropParticleBall::FadeOut( float flDuration )
{
	AddSolidFlags( FSOLID_NOT_SOLID );

	// Start up the eye trail
	if ( m_pGlowTrail != NULL )
	{
		m_pGlowTrail->SetBrightness( 0, flDuration );
	}

	SetThink( &CPropParticleBall::DieThink );
	SetNextThink( gpGlobals->curtime + flDuration );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPropParticleBall::StartWhizSoundThink( void )
{
	SetContextThink( &CPropParticleBall::WhizSoundThink, gpGlobals->curtime + 2.0f * TICK_INTERVAL, s_pWhizThinkContext );
}

//-----------------------------------------------------------------------------
// Danger sounds. 
//-----------------------------------------------------------------------------
void CPropParticleBall::WhizSoundThink()
{
	Vector vecPosition, vecVelocity;
	IPhysicsObject *pPhysicsObject = VPhysicsGetObject();
	
	if ( pPhysicsObject == NULL )
	{
		//NOTENOTE: We should always have been created at this point
		Assert( 0 );
		SetContextThink( &CPropParticleBall::WhizSoundThink, gpGlobals->curtime + 2.0f * TICK_INTERVAL, s_pWhizThinkContext );
		return;
	}

	pPhysicsObject->GetPosition( &vecPosition, NULL );
	pPhysicsObject->GetVelocity( &vecVelocity, NULL );
	
	// Multiplayer equivelent, loops through players and decides if it should go or not, like SP.
	if ( gpGlobals->maxClients > 1 )
	{
		CBasePlayer *pPlayer = NULL;

		for (int i = 1;i <= gpGlobals->maxClients; i++)
		{
			pPlayer = UTIL_PlayerByIndex( i );
			if ( pPlayer )
			{
				Vector vecDelta;
				VectorSubtract( pPlayer->GetAbsOrigin(), vecPosition, vecDelta );
				VectorNormalize( vecDelta );
				if ( DotProduct( vecDelta, vecVelocity ) > 0.5f )
				{
					Vector vecEndPoint;
					VectorMA( vecPosition, 2.0f * TICK_INTERVAL, vecVelocity, vecEndPoint );
					float flDist = CalcDistanceToLineSegment( pPlayer->GetAbsOrigin(), vecPosition, vecEndPoint );
					if ( flDist < 200.0f )
					{
						// We're basically doing what CPASAttenuationFilter does, on a per-user basis, if it passes we create the filter and send off the sound
						// if it doesn't, we skip the player.
						float distance, maxAudible;
						Vector vecRelative;

						VectorSubtract( pPlayer->EarPosition(), vecPosition, vecRelative );
						distance = VectorLength( vecRelative );
						maxAudible = ( 2 * SOUND_NORMAL_CLIP_DIST ) / ATTN_NORM;
						if ( distance <= maxAudible )
							continue;

						// Set the recipient to the player it checked against so multiple sounds don't play.
						CSingleUserRecipientFilter filter( pPlayer );

						EmitSound_t ep;
						ep.m_nChannel = CHAN_STATIC;
						if ( hl2_episodic.GetBool() )
						{
					//		ep.m_pSoundName = "NPC_CombineBall_Episodic.WhizFlyby";
						}
						else
						{
					//		ep.m_pSoundName = "NPC_CombineBall.WhizFlyby";
						}
						ep.m_flVolume = 1.0f;
						ep.m_SoundLevel = SNDLVL_NORM;

						EmitSound( filter, entindex(), ep );
					}
				}
			}
		}
	}
	else
	{
		CBasePlayer *pPlayer = UTIL_GetLocalPlayer();

		if ( pPlayer )
		{
			Vector vecDelta;
			VectorSubtract( pPlayer->GetAbsOrigin(), vecPosition, vecDelta );
			VectorNormalize( vecDelta );
			if ( DotProduct( vecDelta, vecVelocity ) > 0.5f )
			{
				Vector vecEndPoint;
				VectorMA( vecPosition, 2.0f * TICK_INTERVAL, vecVelocity, vecEndPoint );
				float flDist = CalcDistanceToLineSegment( pPlayer->GetAbsOrigin(), vecPosition, vecEndPoint );
				if ( flDist < 200.0f )
				{
					CPASAttenuationFilter filter( vecPosition, ATTN_NORM );

					EmitSound_t ep;
					ep.m_nChannel = CHAN_STATIC;
					if ( hl2_episodic.GetBool() )
					{
			//			ep.m_pSoundName = "NPC_CombineBall_Episodic.WhizFlyby";
					}
					else
					{
			//			ep.m_pSoundName = "NPC_CombineBall.WhizFlyby";
					}
					ep.m_flVolume = 1.0f;
					ep.m_SoundLevel = SNDLVL_NORM;

					EmitSound( filter, entindex(), ep );

					SetContextThink( &CPropParticleBall::WhizSoundThink, gpGlobals->curtime + 0.5f, s_pWhizThinkContext );
					return;
				}
			}
		}

	}

	SetContextThink( &CPropParticleBall::WhizSoundThink, gpGlobals->curtime + 2.0f * TICK_INTERVAL, s_pWhizThinkContext );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPropParticleBall::SetBallAsLaunched( void )
{
	// Give the ball a duration
	StartLifetime( PROP_PARTICLE_BALL_LIFETIME );
	SetType ( PROP_PARTICLE_BALL_TYPE );

	m_bHeld = false;
	m_bLaunched = true;
	SetState( STATE_THROWN );

	VPhysicsGetObject()->SetMass( 750.0f );
	VPhysicsGetObject()->SetInertia( Vector( 1e30, 1e30, 1e30 ) );

	StopLoopingSounds();
	
	WhizSoundThink();
}

//-----------------------------------------------------------------------------
// Lighten the mass so it's zippy toget to the gun
//-----------------------------------------------------------------------------
void CPropParticleBall::OnPhysGunPickup( CBasePlayer *pPhysGunUser, PhysGunPickup_t reason )
{
	CDefaultPlayerPickupVPhysics::OnPhysGunPickup( pPhysGunUser, reason );

	if ( m_nMaxBounces == -1 )
	{
		m_nMaxBounces = 0;
	}

	if ( !m_bFiredGrabbedOutput )
	{

	}

	if ( m_pGlowTrail )
	{
		m_pGlowTrail->TurnOff();
#ifndef SWARM_DLL
		m_pGlowTrail->SetRenderColor( 0, 0, 0, 0 );
#else
		m_pGlowTrail->SetRenderColor( 0, 0, 0 );
#endif
	}

	if ( reason != PUNTED_BY_CANNON )
	{
		SetState( STATE_HOLDING );
		CPASAttenuationFilter filter( GetAbsOrigin(), ATTN_NORM );
		filter.MakeReliable();
		
		EmitSound_t ep;
		ep.m_nChannel = CHAN_STATIC;

		ep.m_pSoundName = "Error";

		ep.m_flVolume = 1.0f;
		ep.m_SoundLevel = SNDLVL_NORM;

		// Now we own this ball
		SetPlayerLaunched( pPhysGunUser );

		CSoundEnvelopeController &controller = CSoundEnvelopeController::GetController();
		m_pHoldingSound = controller.SoundCreate( filter, entindex(), ep );
		controller.Play( m_pHoldingSound, 1.0f, 100 ); 

		// Don't collide with anything we may have to pull the ball through
		SetCollisionGroup( COLLISION_GROUP_DEBRIS );

		VPhysicsGetObject()->SetMass( 20.0f );
		VPhysicsGetObject()->SetInertia( Vector( 100, 100, 100 ) );

		// Make it not explode
		ClearLifetime( );

		m_bHeld = true;
		m_bLaunched = false;

		//Let the ball know is not being captured by one of those ball fields anymore.
		//
		m_bCaptureInProgress = false;


		SetContextThink( &CPropParticleBall::DissolveRampSoundThink, gpGlobals->curtime + GetBallHoldSoundRampTime(), s_pHoldDissolveContext );

		StartAnimating();
	}
	else
	{
		Vector vecVelocity;
		VPhysicsGetObject()->GetVelocity( &vecVelocity, NULL );

		SetSpeed( vecVelocity.Length() );

		// Set us as being launched by the player
		SetPlayerLaunched( pPhysGunUser );

		SetBallAsLaunched();

		StopAnimating();
	}
}

//-----------------------------------------------------------------------------
// Purpose: Reset the ball to be deadly to NPCs after we've picked it up
//-----------------------------------------------------------------------------
void CPropParticleBall::SetPlayerLaunched( CBasePlayer *pOwner )
{
	// Now we own this ball
	SetOwnerEntity( pOwner );
	SetWeaponLaunched( false );
	
	if( VPhysicsGetObject() )
	{
		PhysClearGameFlags( VPhysicsGetObject(), FVPHYSICS_NO_NPC_IMPACT_DMG );
		PhysSetGameFlags( VPhysicsGetObject(), FVPHYSICS_DMG_DISSOLVE | FVPHYSICS_HEAVY_OBJECT );
	}
}

//-----------------------------------------------------------------------------
// Activate death-spin!
//-----------------------------------------------------------------------------
void CPropParticleBall::OnPhysGunDrop( CBasePlayer *pPhysGunUser, PhysGunDrop_t Reason )
{
	CDefaultPlayerPickupVPhysics::OnPhysGunDrop( pPhysGunUser, Reason );

	SetState( STATE_THROWN );
	WhizSoundThink();

	m_bHeld = false;
	m_bLaunched = true;

	// Stop with the dissolving
	SetContextThink( NULL, gpGlobals->curtime, s_pHoldDissolveContext );

	// We're ready to start colliding again.
	SetCollisionGroup( COLLISION_GROUP_PROJECTILE );

	if ( m_pGlowTrail )
	{
		m_pGlowTrail->TurnOn();

#ifndef SWARM_DLL
		m_pGlowTrail->SetRenderColor( 255, 255, 255, 255 );
#else
		m_pGlowTrail->SetRenderColor( 255, 255, 255 );
#endif
		
	}

	// Set our desired speed to be launched at
	SetSpeed( 1500.0f );
	SetPlayerLaunched( pPhysGunUser );

	if ( Reason != LAUNCHED_BY_CANNON )
	{
		// Choose a random direction (forward facing)
		Vector vecForward;
		pPhysGunUser->GetVectors( &vecForward, NULL, NULL );

		QAngle shotAng;
		VectorAngles( vecForward, shotAng );

		// Offset by some small cone
		shotAng[PITCH] += random->RandomInt( -55, 55 );
		shotAng[YAW] += random->RandomInt( -55, 55 );

		AngleVectors( shotAng, &vecForward, NULL, NULL );

		vecForward *= GetSpeed();

		VPhysicsGetObject()->SetVelocity( &vecForward, &vec3_origin );
	}
	else
	{
		// This will have the consequence of making it so that the
		// ball is launched directly down the crosshair even if the player is moving.
		VPhysicsGetObject()->SetVelocity( &vec3_origin, &vec3_origin );
	}

	SetBallAsLaunched();
	StopAnimating();
}

//------------------------------------------------------------------------------
// Stop looping sounds
//------------------------------------------------------------------------------
void CPropParticleBall::StopLoopingSounds()
{
	if ( m_pHoldingSound )
	{
		CSoundEnvelopeController &controller = CSoundEnvelopeController::GetController();
		controller.Shutdown( m_pHoldingSound );
		controller.SoundDestroy( m_pHoldingSound );
		m_pHoldingSound = NULL;
	}
}


//------------------------------------------------------------------------------
// Pow!
//------------------------------------------------------------------------------
void CPropParticleBall::DissolveRampSoundThink( )
{
	float dt = GetBallHoldDissolveTime() - GetBallHoldSoundRampTime();
	if ( m_pHoldingSound )
	{
		CSoundEnvelopeController &controller = CSoundEnvelopeController::GetController();
		controller.SoundChangePitch( m_pHoldingSound, 150, dt );
	}
	SetContextThink( &CPropParticleBall::DissolveThink, gpGlobals->curtime + dt, s_pHoldDissolveContext );
}


//------------------------------------------------------------------------------
// Pow!
//------------------------------------------------------------------------------
void CPropParticleBall::DissolveThink( )
{
	DoExplosion();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
float CPropParticleBall::GetBallHoldDissolveTime()
{
	float flDissolveTime = PROP_PARTICLE_BALL_HOLD_DISSOLVE_TIME;

	if( g_pGameRules->IsSkillLevel( 1 ) && hl2_episodic.GetBool() )
	{
		// Give players more time to handle/aim combine balls on Easy.
		flDissolveTime *= 1.5f;
	}

	return flDissolveTime;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
float CPropParticleBall::GetBallHoldSoundRampTime()
{
	return GetBallHoldDissolveTime() - 1.0f;
}

//------------------------------------------------------------------------------
// Pow!
//------------------------------------------------------------------------------
void CPropParticleBall::DoExplosion( )
{
	// don't do this twice
	if ( GetMoveType() == MOVETYPE_NONE )
		return;

	if ( PhysIsInCallback() )
	{
		g_PostSimulationQueue.QueueCall( this, &CPropParticleBall::DoExplosion );
		return;
	}

	//Shockring
	CBroadcastRecipientFilter filter2;

	if ( OutOfBounces() == false )
	{
		StopSound("ParticleBall.AmbientLoop");

		// Don't buzz when we hit what we are suppose to hit.
		if(!m_bHitBox)
		{
			EmitSound( "ParticleBall.Explosion" );
		}

//		UTIL_ScreenShake( GetAbsOrigin(), 20.0f, 150.0, 1.0, 1250.0f, SHAKE_START );


//#ifndef SWARM_DLL
		DispatchParticleEffect (PROP_PARTICLE_BALL_HIT_WARP, PATTACH_ABSORIGIN, this );
//#endif
		CEffectData data;

//		data.m_vOrigin = GetAbsOrigin();

//		DispatchEffect( "cball_explode", data );
/*
		te->BeamRingPoint( filter2, 0, GetAbsOrigin(),	//origin
			m_flRadius,	//start radius
			1024,		//end radius
			s_nRingExplosionTexture, //texture
			0,			//halo index
			0,			//start frame
			2,			//framerate
			0.2f,		//life
			64,			//width
			0,			//spread
			0,			//amplitude
			255,	//r
			255,	//g
			225,	//b
			32,		//a
			0,		//speed
			FBEAM_FADEOUT
			);

		//Shockring
		te->BeamRingPoint( filter2, 0, GetAbsOrigin(),	//origin
			m_flRadius,	//start radius
			1024,		//end radius
			s_nRingExplosionTexture, //texture
			0,			//halo index
			0,			//start frame
			2,			//framerate
			0.5f,		//life
			64,			//width
			0,			//spread
			0,			//amplitude
			255,	//r
			255,	//g
			225,	//b
			64,		//a
			0,		//speed
			FBEAM_FADEOUT
			);
	}
	else
	{
		//Shockring
		te->BeamRingPoint( filter2, 0, GetAbsOrigin(),	//origin
			128,	//start radius
			384,		//end radius
			s_nRingExplosionTexture, //texture
			0,			//halo index
			0,			//start frame
			2,			//framerate
			0.25f,		//life
			48,			//width
			0,			//spread
			0,			//amplitude
			255,	//r
			255,	//g
			225,	//b
			64,		//a
			0,		//speed
			FBEAM_FADEOUT
			);
			*/
	}

	if( hl2_episodic.GetBool() )
	{
		CSoundEnt::InsertSound( SOUND_COMBAT | SOUND_CONTEXT_EXPLOSION, WorldSpaceCenter(), 180.0f, 0.25, this );
	}

	// Turn us off and wait because we need our trails to finish up properly
	SetAbsVelocity( vec3_origin );
	SetMoveType( MOVETYPE_NONE );
	AddSolidFlags( FSOLID_NOT_SOLID );

	m_bEmit = false;

	SetContextThink( &CPropParticleBall::SUB_Remove, gpGlobals->curtime + 0.1f, s_pRemoveContext );
	StopLoopingSounds();
}

//-----------------------------------------------------------------------------
// Enable/disable
//-----------------------------------------------------------------------------
void CPropParticleBall::InputExplode( inputdata_t &inputdata )
{
	DoExplosion();
}

//-----------------------------------------------------------------------------
// Enable/disable
//-----------------------------------------------------------------------------
void CPropParticleBall::InputFadeAndRespawn( inputdata_t &inputdata )
{
	FadeOut( 0.1f );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPropParticleBall::CollisionEventToTrace( int index, gamevcollisionevent_t *pEvent, trace_t &tr )
{
	UTIL_ClearTrace( tr );
	pEvent->pInternalData->GetSurfaceNormal( tr.plane.normal );
	pEvent->pInternalData->GetContactPoint( tr.endpos );
	tr.plane.dist = DotProduct( tr.plane.normal, tr.endpos );
	VectorMA( tr.endpos, -1.0f, pEvent->preVelocity[index], tr.startpos );
	tr.m_pEnt = pEvent->pEntities[!index];
	tr.fraction = 0.01f;	// spoof!
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CPropParticleBall::DissolveEntity( CBaseEntity *pEntity )
{
	if( pEntity->IsEFlagSet( EFL_NO_DISSOLVE ) )
		return false;

#ifdef HL2MP
	if ( pEntity->IsPlayer() )
	{
		m_bStruckEntity = true;
		return false;
	}
#endif

	if( !pEntity->IsNPC() && !(dynamic_cast<CRagdollProp*>(pEntity)) )
		return false;

	pEntity->GetBaseAnimating()->Dissolve( "", gpGlobals->curtime, false, ENTITY_DISSOLVE_NORMAL );
	
	// Note that we've struck an entity
	m_bStruckEntity = true;
	
	// Force an NPC to not drop their weapon if dissolved
	CBaseCombatCharacter *pBCC = ToBaseCombatCharacter( pEntity );
	if ( pBCC != NULL )
	{
		pEntity->AddSpawnFlags( SF_NPC_NO_WEAPON_DROP );
	}

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPropParticleBall::OnHitEntity( CBaseEntity *pHitEntity, float flSpeed, int index, gamevcollisionevent_t *pEvent )
{
	// Detonate on the strider + the bone followers in the strider
	if ( FClassnameIs( pHitEntity, "npc_strider" ) || 
		(pHitEntity->GetOwnerEntity() && FClassnameIs( pHitEntity->GetOwnerEntity(), "npc_strider" )) )
	{
		DoExplosion();
		return;
	}

	/*
	// We don't want the balls bouncing off of props
	int nModelType = modelinfo->GetModelType( pHitEntity->GetModel() );
	if ( nModelType == mod_studio )
	{
		DoExplosion();
		return;
	}
	*/

	// To Do: Find a way to make entites tell which ball they got hit with! Use m_intType!
//	CTakeDamageInfo info( this, GetOwnerEntity(), GetAbsVelocity(), GetAbsOrigin(), sk_npc_dmg_particleball.GetFloat(), DMG_GENERIC );

//	bool bIsDissolving = (pHitEntity->GetFlags() & FL_DISSOLVING) != 0;
//	bool bShouldHit = pHitEntity->PassesDamageFilter( info );

	//One more check
	//Combine soldiers are not allowed to hurt their friends with combine balls (they can still shoot and hurt each other with grenades).
	CBaseCombatCharacter *pBCC = pHitEntity->MyCombatCharacterPointer();

	if ( pBCC )
	{
//		bShouldHit = pBCC->IRelationType( GetOwnerEntity() ) != D_LI;
	}
/*
	if ( !bIsDissolving && bShouldHit == true )
	{ 

		if ( pHitEntity->PassesDamageFilter( info ) )
		{
			if( WasFiredByNPC() || m_nMaxBounces == -1 )
			{
				// Since Combine balls fired by NPCs do a metered dose of damage per impact, we have to ignore touches
				// for a little while after we hit someone, or the ball will immediately touch them again and do more
				// damage. 
				if( gpGlobals->curtime >= m_flNextDamageTime )
				{
					EmitSound( "NPC_CombineBall.KillImpact" );

					if ( pHitEntity->IsNPC() && pHitEntity->Classify() != CLASS_PLAYER_ALLY_VITAL && hl2_episodic.GetBool() == true )
					{
						if ( pHitEntity->Classify() != CLASS_PLAYER_ALLY || pHitEntity->Classify() == CLASS_PLAYER_ALLY && m_bStruckEntity == false )
						{
							info.SetDamage( pHitEntity->GetMaxHealth() );
							m_bStruckEntity = true;
						}
					}
					else
					{
						// Ignore touches briefly.
						m_flNextDamageTime = gpGlobals->curtime + 0.1f;
					}

					pHitEntity->TakeDamage( info );
				}
			
			}
			else
			{
				if ( (m_nState == STATE_THROWN) && (pHitEntity->IsNPC() || dynamic_cast<CRagdollProp*>(pHitEntity) ))
				{
					EmitSound( "NPC_CombineBall.KillImpact" );
				}
				if ( (m_nState != STATE_HOLDING) )
				{

					CBasePlayer *pPlayer = ToBasePlayer( GetOwnerEntity() );
					if ( pPlayer && UTIL_IsAR2ParticleBall( this ) && ToBaseCombatCharacter( pHitEntity ) )
					{
						gamestats->Event_WeaponHit( pPlayer, false, "weapon_ar2", info );
					}

					DissolveEntity( pHitEntity );
					if ( pHitEntity->ClassMatches( "npc_hunter" ) )
					{
						DoExplosion();
						return;
					}
				}
				
			}	
		
		}
	}
	*/
	Vector vecFinalVelocity;
	if ( IsInField() )
	{
		// Don't deflect when in a spawner field
		vecFinalVelocity = pEvent->preVelocity[index];
	}
	else
	{
		// Don't slow down when hitting other entities.
		vecFinalVelocity = pEvent->postVelocity[index];
		VectorNormalize( vecFinalVelocity );
		vecFinalVelocity *= GetSpeed();
	}
	//PhysCallbackSetVelocity( pEvent->pObjects[index], vecFinalVelocity ); 
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPropParticleBall::DoImpactEffect( const Vector &preVelocity, int index, gamevcollisionevent_t *pEvent )
{
	// Do that crazy impact effect!
	trace_t tr;
	CollisionEventToTrace( !index, pEvent, tr );

	CBaseEntity *pTraceEntity = pEvent->pEntities[index];
	UTIL_TraceLine(tr.startpos - preVelocity * 2.0f, tr.startpos + preVelocity * 2.0f, MASK_SHOT_PORTAL, pTraceEntity, COLLISION_GROUP_NONE, &tr);

	if ( tr.fraction < 1.0f )
	{
		if ( tr.surface.flags & SURF_NOPORTAL )
		{

#ifndef NO_PARTICLE_EFFECTS
{
#ifndef SWARM_DLL
			DispatchParticleEffect (PROP_PARTICLE_BALL_HIT_WARP, PATTACH_ABSORIGIN, this );
#endif
}
#endif
			EmitSound( "ParticleBall.Explosion" );
			DoExplosion();
		}

		// See if we hit the sky
		if ( tr.surface.flags & SURF_SKY )
		{
			DoExplosion();
			return;
		}

			// Send the effect over
			CEffectData	data;

			data.m_flRadius = 16;
			data.m_vNormal	= tr.plane.normal;
			data.m_vOrigin	= tr.endpos + tr.plane.normal * 1.0f;

			if (m_intType == 0) 
			{
				DispatchEffect( "cball_blue_bounce", data );
			}

			if (m_intType == 1) 
			{
				DispatchEffect( "cball_green_bounce", data );
			}

			if (m_intType == 2) 
			{
				DispatchEffect( "cball_purple_bounce", data );
			}

			if (m_intType == 3) 
			{
				DispatchEffect( "cball_3_bounce", data );
			}

			if (m_intType == 4) 
			{
				DispatchEffect( "cball_4_bounce", data );
			}

			if (m_intType == 5) 
			{
				DispatchEffect( "cball_5_bounce", data );
			}
			EmitSound( "ParticleBall.Impact" );
			return;
	}
}

//-----------------------------------------------------------------------------
// Tells whether this combine ball should consider deflecting towards this entity.
//-----------------------------------------------------------------------------
bool CPropParticleBall::IsAttractiveTarget( CBaseEntity *pEntity )
{

	if ( FClassnameIs( pEntity, "prop_vecbox" ) )
		return true;
/*
	if ( !pEntity->IsAlive() )
		return false;

	if ( pEntity->GetFlags() & EF_NODRAW )
		return false;

	// Don't guide toward striders
	if ( FClassnameIs( pEntity, "npc_strider" ) )
		return false;

	if( WasFiredByNPC() )
	{
		// Fired by an NPC
		if( !pEntity->IsNPC() && !pEntity->IsPlayer() )
			return false;

		// Don't seek entities of the same class.
		if ( pEntity->m_iClassname == GetOwnerEntity()->m_iClassname )
			return false;
	}
	else
	{
		
#ifndef HL2MP
		
		if ( GetOwnerEntity() ) 
		{
			// Things we check if this ball has an owner that's not an NPC.
			if( GetOwnerEntity()->IsPlayer() ) 
			{
				if( pEntity->Classify() == CLASS_PLAYER)
				{
					// Not attracted to other players or allies.
					return false;
				}
			}
		}

		// The default case.
		if ( !pEntity->IsNPC() )
			return false;
		

		//if( pEntity->Classify() == CLASS_BULLSEYE )
			//return false;

#else
		if ( pEntity->IsPlayer() == false )
			 return false;

		if ( pEntity == GetOwnerEntity() )
			 return false;
		
		//No tracking teammates in teammode!
		if ( g_pGameRules->IsTeamplay() )
		{
			if ( g_pGameRules->PlayerRelationship( GetOwnerEntity(), pEntity ) == GR_TEAMMATE )
				 return false;
		}
#endif

		// We must be able to hit them
		trace_t	tr;
		UTIL_TraceLine( WorldSpaceCenter(), pEntity->BodyTarget( WorldSpaceCenter() ), MASK_SOLID, this, COLLISION_GROUP_NONE, &tr );

		if ( tr.fraction < 1.0f && tr.m_pEnt != pEntity )
			return false;
	}
*/
	return false;
}

//-----------------------------------------------------------------------------
// Deflects the ball toward enemies in case of a collision 
//-----------------------------------------------------------------------------
void CPropParticleBall::DeflectTowardEnemy( float flSpeed, int index, gamevcollisionevent_t *pEvent )
{
	// Bounce toward a particular enemy; choose one that's closest to my new velocity.
	Vector vecVelDir = pEvent->postVelocity[index];
	VectorNormalize( vecVelDir );

	CBaseEntity *pBestTarget = NULL;

	Vector vecStartPoint;
	pEvent->pInternalData->GetContactPoint( vecStartPoint );

	float flBestDist = MAX_COORD_FLOAT;

	CBaseEntity *list[1024];

	Vector	vecDelta;
	float	distance, flDot;

	// If we've already hit something, get accurate
	bool bSeekKill = m_bStruckEntity && (WasWeaponLaunched() || sk_particleball_seek_hit.GetInt() );

	if ( bSeekKill )
	{
		int nCount = UTIL_EntitiesInSphere( list, 1024, GetAbsOrigin(), sk_particle_ball_search_radius.GetFloat(), FL_NPC | FL_CLIENT );
		
		for ( int i = 0; i < nCount; i++ )
		{
			if ( !IsAttractiveTarget( list[i] ) )
				continue;

			VectorSubtract( list[i]->WorldSpaceCenter(), vecStartPoint, vecDelta );
			distance = VectorNormalize( vecDelta );

			if ( distance < flBestDist )
			{
				// Check our direction
				if ( DotProduct( vecDelta, vecVelDir ) > 0.0f )
				{
					pBestTarget = list[i];
					flBestDist = distance;
				}
			}
		}
	}
	else
	{
		float flMaxDot = 0.966f;
		if ( !WasWeaponLaunched() )
		{
			float flMaxDot = sk_particleball_seek_angle.GetFloat();
			float flGuideFactor = sk_particleball_guidefactor.GetFloat();
			for ( int i = m_nBounceCount; --i >= 0; )
			{
				flMaxDot *= flGuideFactor;
			}
			flMaxDot = cos( flMaxDot * M_PI / 180.0f );

			if ( flMaxDot > 1.0f )
			{
				flMaxDot = 1.0f;
			}
		}

		// Otherwise only help out a little
		Vector extents = Vector(256, 256, 256);
		Ray_t ray;
		ray.Init( vecStartPoint, vecStartPoint + 2048 * vecVelDir, -extents, extents );
		int nCount = UTIL_EntitiesAlongRay( list, 1024, ray, FL_NPC | FL_CLIENT );
		for ( int i = 0; i < nCount; i++ )
		{
			if ( !IsAttractiveTarget( list[i] ) )
				continue;

			VectorSubtract( list[i]->WorldSpaceCenter(), vecStartPoint, vecDelta );
			distance = VectorNormalize( vecDelta );
			flDot = DotProduct( vecDelta, vecVelDir );
			
			if ( flDot > flMaxDot )
			{
				if ( distance < flBestDist )
				{
					pBestTarget = list[i];
					flBestDist = distance;
				}
			}
		}
	}

	if ( pBestTarget )
	{
		Vector vecDelta;
		VectorSubtract( pBestTarget->WorldSpaceCenter(), vecStartPoint, vecDelta );
		VectorNormalize( vecDelta );
		vecDelta *= GetSpeed();
		PhysCallbackSetVelocity( pEvent->pObjects[index], vecDelta ); 
	}
}


//-----------------------------------------------------------------------------
// Bounce inside the spawner: 
//-----------------------------------------------------------------------------
void CPropParticleBall::BounceInSpawner( float flSpeed, int index, gamevcollisionevent_t *pEvent )
{
	m_bForward = !m_bForward;

	Vector vecTarget;

	Vector vecVelocity;
	VectorSubtract( vecTarget, GetAbsOrigin(), vecVelocity );
	VectorNormalize( vecVelocity );
	vecVelocity *= flSpeed;

	PhysCallbackSetVelocity( pEvent->pObjects[index], vecVelocity ); 
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CPropParticleBall::IsHittableEntity( CBaseEntity *pHitEntity )
{
	if ( pHitEntity->IsWorld() )
		return false;

	//if ( pHitEntity->Classify() == CLASS_PLAYER )
		//return false; //?

	if ( pHitEntity->GetMoveType() == MOVETYPE_PUSH )
	{
		if( pHitEntity->GetOwnerEntity() && FClassnameIs(pHitEntity->GetOwnerEntity(), "npc_strider") )
		{
			// The Strider's Bone Followers are MOVETYPE_PUSH, and we want the combine ball to hit these.
			return true;
		}

		// If the entity we hit can take damage, we're good
		if ( pHitEntity->m_takedamage == DAMAGE_YES )
			return true;

		return false;
	}

	if ( FClassnameIs( pHitEntity, "prop_vecbox" ))
	{
		/*
		CVecBox* m_pBox = static_cast<CVecBox*>(pHitEntity);
		if (m_pBox->IsGhost())
			return false;
		*/

		m_bHitBox = true;
		DoExplosion();

		MakeHitParticleSystem();

		if ( m_intType == 0 )
		{
			//DispatchParticleEffect (PROP_PARTICLE_BALL_HIT0, PATTACH_ABSORIGIN, this );
			Vector ParticleColor (BALL0_COLOR_R, BALL0_COLOR_G, BALL0_COLOR_B);
			m_hParticleEffect->SetControlPointValue( 7, ParticleColor );
		}
		if ( m_intType == 1 )
		{
			//DispatchParticleEffect (PROP_PARTICLE_BALL_HIT1, PATTACH_ABSORIGIN, this );
			Vector ParticleColor (BALL1_COLOR_R, BALL1_COLOR_G, BALL1_COLOR_B);
			m_hParticleEffect->SetControlPointValue( 7, ParticleColor );
		}
		if ( m_intType == 2 )
		{
			//DispatchParticleEffect (PROP_PARTICLE_BALL_HIT2, PATTACH_ABSORIGIN, this );
			Vector ParticleColor (BALL2_COLOR_R, BALL2_COLOR_G, BALL2_COLOR_B);
			m_hParticleEffect->SetControlPointValue( 7, ParticleColor );
		}

		if ( m_intType == 3 )
		{
			//DispatchParticleEffect (PROP_PARTICLE_BALL_HIT3, PATTACH_ABSORIGIN, this );
			Vector ParticleColor (vectronic_vecball3_color_r.GetInt(), vectronic_vecball3_color_g.GetInt(), vectronic_vecball3_color_b.GetInt());
			m_hParticleEffect->SetControlPointValue( 7, ParticleColor );
		}

		if ( m_intType == 4 )
		{
			//DispatchParticleEffect (PROP_PARTICLE_BALL_HIT4, PATTACH_ABSORIGIN, this );
			Vector ParticleColor (vectronic_vecball4_color_r.GetInt(), vectronic_vecball4_color_g.GetInt(), vectronic_vecball4_color_b.GetInt());
			m_hParticleEffect->SetControlPointValue( 7, ParticleColor );
		}

		if ( m_intType == 5 )
		{
			//DispatchParticleEffect (PROP_PARTICLE_BALL_HIT5, PATTACH_ABSORIGIN, this );
			Vector ParticleColor (vectronic_vecball5_color_r.GetInt(), vectronic_vecball5_color_g.GetInt(), vectronic_vecball5_color_b.GetInt());
			m_hParticleEffect->SetControlPointValue( 7, ParticleColor );
		}

		return true;
	}

	return true;
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPropParticleBall::VPhysicsCollision( int index, gamevcollisionevent_t *pEvent )
{
	Vector preVelocity = pEvent->preVelocity[index];
	float flSpeed = VectorNormalize( preVelocity );

	if ( m_nMaxBounces == -1 )
	{
		const surfacedata_t *pHit = physprops->GetSurfaceData( pEvent->surfaceProps[!index] );

		if( pHit->game.material != CHAR_TEX_FLESH || !hl2_episodic.GetBool() )
		{
			CBaseEntity *pHitEntity = pEvent->pEntities[!index];
			if ( pHitEntity && IsHittableEntity( pHitEntity ) )
			{
				OnHitEntity( pHitEntity, flSpeed, index, pEvent );
			}

			// Remove self without affecting the object that was hit. (Unless it was flesh)
			NotifySpawnerOfRemoval();
			PhysCallbackRemove( this->NetworkProp() );

			// disable dissolve damage so we don't kill off the player when he's the one we hit
			PhysClearGameFlags( VPhysicsGetObject(), FVPHYSICS_DMG_DISSOLVE );
			return;
		}
	}

	// Prevents impact sounds, effects, etc. when it's in the field
	if ( !IsInField() )
	{
		BaseClass::VPhysicsCollision( index, pEvent );
	}

	if ( m_nState == STATE_HOLDING )
		return;

	// If we've collided going faster than our desired, then up our desired
	if ( flSpeed > GetSpeed() )
	{
		SetSpeed( flSpeed );
	}

	// Make sure we don't slow down
	Vector vecFinalVelocity = pEvent->postVelocity[index];
	VectorNormalize( vecFinalVelocity );
	vecFinalVelocity *= GetSpeed();
	PhysCallbackSetVelocity( pEvent->pObjects[index], vecFinalVelocity ); 

	CBaseEntity *pHitEntity = pEvent->pEntities[!index];
	if ( pHitEntity && IsHittableEntity( pHitEntity ) )
	{
		OnHitEntity( pHitEntity, flSpeed, index, pEvent );
		return;
	}

	if ( IsInField() )
	{


		PhysCallbackSetVelocity( pEvent->pObjects[index], vec3_origin ); 

		// Delay the fade out so that we don't change our 
		// collision rules inside a vphysics callback.
		variant_t emptyVariant;
		g_EventQueue.AddEvent( this, "FadeAndRespawn", 0.01, NULL, NULL );
		return;
	}

	if ( IsBeingCaptured() )
		return;

	// Do that crazy impact effect!
	DoImpactEffect( preVelocity, index, pEvent );

	// Only do the bounce so often
	if ( gpGlobals->curtime - m_flLastBounceTime < 0.25f )
		return;

	// Save off our last bounce time
	m_flLastBounceTime = gpGlobals->curtime;

	// Reset the sound timer
	SetContextThink( &CPropParticleBall::WhizSoundThink, gpGlobals->curtime + 0.01, s_pWhizThinkContext );

	// Deflect towards nearby enemies
	DeflectTowardEnemy( flSpeed, index, pEvent );

	// Once more bounce
	++m_nBounceCount;

	if ( OutOfBounces() && m_bBounceDie == false )
	{
		StartLifetime( 0.1 );
		//Hack: Stop this from being called by doing this.
		m_bBounceDie = true;
	}
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPropParticleBall::AnimThink( void )
{
	StudioFrameAdvance();
	SetContextThink( &CPropParticleBall::AnimThink, gpGlobals->curtime + 0.1f, s_pAnimThinkContext );
}

// Point Spawner
class CPointParticleBallSpawn : public CLogicalEntity
{
public: 	
	
	DECLARE_CLASS( CPointParticleBallSpawn, CLogicalEntity);

	void Spawn( );
	void Precache( void );
//	void Think( void );

	void FireBall( void );

	DECLARE_DATADESC();

	// Input functions.
	void InputLaunchBall( inputdata_t &inputData);
	void InputEnable( inputdata_t &inputdata );
	void InputDisable( inputdata_t &inputdata );
	void InputToggle( inputdata_t &inputdata );

	// Output functions.

private:

	bool m_bDisabled;
	int	 m_bType;
	float m_flLife;

	float		m_flConeDegrees;

	COutputEvent m_OnPostSpawnBall;

};

LINK_ENTITY_TO_CLASS( point_vecball_launcher, CPointParticleBallSpawn);

BEGIN_DATADESC( CPointParticleBallSpawn )

	//Save/load
	DEFINE_KEYFIELD(m_bDisabled, FIELD_BOOLEAN, "StartDisabled"),
	DEFINE_KEYFIELD(m_bType, FIELD_INTEGER, "BallType"),
	DEFINE_KEYFIELD(m_flLife, FIELD_FLOAT, "Life"),
	
	DEFINE_INPUTFUNC(FIELD_VOID, "Enable", InputEnable),
	DEFINE_INPUTFUNC(FIELD_VOID, "Disable", InputDisable),
	DEFINE_INPUTFUNC(FIELD_VOID, "Toggle", InputToggle),
	DEFINE_INPUTFUNC( FIELD_VOID, "LaunchBall", InputLaunchBall ),

	DEFINE_OUTPUT( m_OnPostSpawnBall, "OnPostSpawnProjectile" ),

END_DATADESC()

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPointParticleBallSpawn::Spawn( void )
{
	Precache();
	Think();
	UTIL_PrecacheOther( "prop_particle_ball" );
	BaseClass::Spawn();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPointParticleBallSpawn::Precache( void )
{
	BaseClass::Precache();
}

//------------------------------------------------------------------------------
// Purpose: Turns on the relay, allowing it to fire outputs.
//------------------------------------------------------------------------------
void CPointParticleBallSpawn::InputEnable( inputdata_t &inputdata )
{
	m_bDisabled = false;
}

//------------------------------------------------------------------------------
// Purpose: Turns on the relay, allowing it to fire outputs.
//------------------------------------------------------------------------------
void CPointParticleBallSpawn::InputDisable( inputdata_t &inputdata )
{
	m_bDisabled = true;
}

//------------------------------------------------------------------------------
// Purpose: Turns on the relay, allowing it to fire outputs.
//------------------------------------------------------------------------------
void CPointParticleBallSpawn::InputToggle( inputdata_t &inputdata )
{
	m_bDisabled = !m_bDisabled;
}

//-----------------------------------------------------------------------------
// Purpose: "Then I flip a switch..."
//-----------------------------------------------------------------------------
void CPointParticleBallSpawn::InputLaunchBall( inputdata_t &inputData)
{
	m_OnPostSpawnBall.FireOutput( inputData.pActivator, this );
	FireBall();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPointParticleBallSpawn::FireBall()
{
	CPropParticleBall *pBall = static_cast<CPropParticleBall*>( CreateEntityByName( "prop_particle_ball" ) );

	if ( pBall == NULL )
		 return;

//	Vector vecVelocity;
	Vector vecAbsOrigin = GetAbsOrigin();
	Vector zaxis;
	
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

	pBall->SetType ( m_bType );

}