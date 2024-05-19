//======== Copyright © 2013 - 2014, rHetorical, All rights reserved. ==========
//
// Purpose: 
//		
//=============================================================================

#include "cbase.h"
#include "engine/IEngineSound.h"
#include "vectronic_player.h"
#include "prop_vectronic_projectile.h"
#include "prop_vecbox.h"
#include "filters.h"
#include "physics.h"
#include "vphysics_interface.h"
#include "entityoutput.h"
#include "studio.h"
#include "explode.h"
#include <convar.h>
#include "particle_parse.h"
#include "props.h"
#include "physics_collisionevent.h"
#include "player_pickup.h"
#include "player_pickup_controller.h"
#include "vectronic_vecall_system.h"

// Our sharedefs.
#include "vectronic_shareddefs.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define SUCK_MAGNITUDE 9999.0f

extern ConVar player_throwforce;

extern ConVar vectronic_vecball3_color_r;
extern ConVar vectronic_vecball3_color_g;
extern ConVar vectronic_vecball3_color_b;

extern ConVar vectronic_vecball4_color_r;
extern ConVar vectronic_vecball4_color_g;
extern ConVar vectronic_vecball4_color_b;

extern ConVar vectronic_vecball5_color_r;
extern ConVar vectronic_vecball5_color_g;
extern ConVar vectronic_vecball5_color_b;

void CC_GiveVecBox( void )
{
	engine->ServerCommand("ent_create prop_vecbox\n");
}

static ConCommand makebox("ent_create_vecbox", CC_GiveVecBox, "Give the player a Vectronic Box\n", FCVAR_CHEAT );

LINK_ENTITY_TO_CLASS( prop_vecbox, CVecBox );
PRECACHE_REGISTER(prop_vecbox);

BEGIN_DATADESC( CVecBox )

	//Save/load
	DEFINE_USEFUNC( Use ),
	DEFINE_THINKFUNC(SuckThink),

	//DEFINE_KEYFIELD( m_intStartActivation, FIELD_INTEGER, "startstate" ),

	// Target
	DEFINE_KEYFIELD(m_bTarget, FIELD_BOOLEAN, "target"),
	DEFINE_KEYFIELD(m_flRadius, FIELD_FLOAT, "radius"),
	
	DEFINE_FIELD( m_intLastBallHit, FIELD_INTEGER ),
	DEFINE_FIELD( m_intProtecting, FIELD_INTEGER ),
	DEFINE_FIELD( m_DisableTouch, FIELD_BOOLEAN),
	DEFINE_FIELD( m_bHasGhost, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_bIsGhost, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_bActivated, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_bIsProtected, FIELD_BOOLEAN ),

	DEFINE_FIELD( m_hBox, FIELD_EHANDLE ),
	//DEFINE_FIELD(m_hTarget, FIELD_EHANDLE),

#ifndef SWARM_DLL
	DEFINE_AUTO_ARRAY( m_hSprite, FIELD_EHANDLE ),
#else
	DEFINE_FIELD( m_hParticleEffect, FIELD_EHANDLE ),
#endif

	// I/O
	DEFINE_INPUTFUNC( FIELD_VOID, "Dissolve", InputDissolve ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Explode", InputExplode ),

	// Ball Touch
	DEFINE_INPUTFUNC(FIELD_VOID, "DisableBallTouch", InputDisableTouch),
	DEFINE_INPUTFUNC(FIELD_VOID, "EnableBallTouch", InputEnableTouch),

	DEFINE_INPUTFUNC(FIELD_BOOLEAN, "EnableGravity", InputEnableGravity), //02/13/18 - Added upon request

	// Ghost
	DEFINE_INPUTFUNC(FIELD_VOID, "MakeGhost", InputMakeGhost),
	DEFINE_INPUTFUNC( FIELD_VOID, "KillGhost", InputKillGhost ),

	// Target
	DEFINE_INPUTFUNC(FIELD_VOID, "EnableTarget", InputEnableTarget),
	DEFINE_INPUTFUNC(FIELD_VOID, "DisableTarget", InputDisableTarget),
//	DEFINE_INPUTFUNC(FIELD_FLOAT, "SetTargetMagitude", InputSetTargetMagitude),
	DEFINE_INPUTFUNC(FIELD_FLOAT, "SetTargetRadius", InputSetTargetRadius),

	DEFINE_OUTPUT( m_OnPlayerUse, "OnPlayerUse" ),
	DEFINE_OUTPUT( m_OnPlayerPickup, "OnPlayerPickup" ),
	DEFINE_OUTPUT( m_OnPhysGunPickup, "OnPhysGunPickup" ),
	DEFINE_OUTPUT( m_OnPhysGunDrop, "OnPhysGunDrop" ),

	DEFINE_OUTPUT( m_OnBall, "OnBallHit"),
	DEFINE_OUTPUT( m_OnBall0, "OnBall0" ),
	DEFINE_OUTPUT( m_OnBall1, "OnBall1" ),
	DEFINE_OUTPUT( m_OnBall2, "OnBall2" ),

	DEFINE_OUTPUT( m_OnBall3, "OnBall3" ),
	DEFINE_OUTPUT( m_OnBall4, "OnBall4" ),
	DEFINE_OUTPUT( m_OnBall5, "OnBall5" ),

	DEFINE_OUTPUT( m_OnReset, "OnReset" ),

	DEFINE_OUTPUT( m_OnExplode, "OnExplode" ),

END_DATADESC()

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CVecBox::CVecBox( void )
{
	SetCurrentBall(BALL_ANY);

	m_bIsGhost = false;
	m_bActivated = false;
	m_bIsProtected = false;
	m_DisableTouch = false;
	m_bTarget = false;
	m_intProtecting = BALL_ANY;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVecBox::Precache( void )
{
	// Our Box Model
	PrecacheModel( VECBOX_MODEL );
	PrecacheModel( VECBOX_MODEL_GHOST );

#ifndef SWARM_DLL
	PrecacheModel( VECBOX_SPRITE );
#endif

	// Particles
	PrecacheParticleSystem( VECBOX_CLEAR_PARTICLE );

	// Sounds
	PrecacheScriptSound( "VecBox.Activate" );
	PrecacheScriptSound( "VecBox.Deactivate" );
	PrecacheScriptSound( "VecBox.ClearShield" );

	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVecBox::Spawn( void )
{
	SetModel( VECBOX_MODEL );
	SetSolid( SOLID_VPHYSICS );
	CreateVPhysics();
	SetUse( &CVecBox::Use );

	SetRenderColorToBalls();
	SetupSprites();

	// Quick Think!
	SetNextThink( gpGlobals->curtime + .1 );

	if (m_bTarget )
	{
		RegisterThinkContext("SuckThink");
		SetContextThink(&CVecBox::SuckThink, gpGlobals->curtime, "SuckThink");

		SetNextThink(gpGlobals->curtime + 0.05f, "SuckThink");
	}

	BaseClass::Spawn();

}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pTarget - 
//-----------------------------------------------------------------------------
void CVecBox::SuckEntity(CBaseEntity *pTarget, const Vector &vecPushPoint)
{
	Vector vecPushDir;
	vecPushDir = (pTarget->BodyTarget(vecPushPoint, false) - vecPushPoint);

	float dist = VectorNormalize(vecPushDir);

	float flFalloff = RemapValClamped(dist, m_flRadius, m_flRadius*0.25f, 0.0f, 1.0f);

	switch (pTarget->GetMoveType())
	{
	case MOVETYPE_NONE:
	case MOVETYPE_PUSH:
	case MOVETYPE_STEP:
	case MOVETYPE_NOCLIP:
		break;

	case MOVETYPE_VPHYSICS:
	{
		IPhysicsObject *pPhys = pTarget->VPhysicsGetObject();
		if (pPhys)
		{
			// UNDONE: Assume the velocity is for a 100kg object, scale with mass
			pPhys->ApplyForceCenter((SUCK_MAGNITUDE * -1) * flFalloff * 100.0f * vecPushDir * pPhys->GetMass() * gpGlobals->frametime);
			return;
		}

		CPropParticleBall *pBall = dynamic_cast<CPropParticleBall*>(pTarget);
		if (pBall)
		{
			pBall->SetSpeed(0.0f);
			return;
		}

	}
	break;

	default:
	{
		Vector vecPush = ((SUCK_MAGNITUDE * -1) * vecPushDir * flFalloff);
		if (pTarget->GetFlags() & FL_BASEVELOCITY)
		{
			vecPush = vecPush + pTarget->GetBaseVelocity();
		}
		if (vecPush.z > 0 && (pTarget->GetFlags() & FL_ONGROUND))
		{
			pTarget->SetGroundEntity(NULL);
			Vector origin = pTarget->GetAbsOrigin();
			origin.z += 1.0f;
			pTarget->SetAbsOrigin(origin);
		}

		pTarget->SetBaseVelocity(vecPush);
		pTarget->AddFlag(FL_BASEVELOCITY);
	}
	break;
	}

}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVecBox::SuckThink(void)
{
	 // DevMsg("CVecBox: Suck Thinking\n");
	// Get a collection of entities in a radius around us
	CBaseEntity *pEnts[256];
	int numEnts = UTIL_EntitiesInSphere(pEnts, 256, GetAbsOrigin(), m_flRadius, 0);

	for (int i = 0; i < numEnts; i++)
	{
		// Must be solid
		if (pEnts[i]->IsSolid() == false)
			continue;

		// Cannot be parented (only push parents)
		if (pEnts[i]->GetMoveParent() != NULL)
			continue;

		// Must be moveable
		if (pEnts[i]->GetMoveType() != MOVETYPE_VPHYSICS &&
			pEnts[i]->GetMoveType() != MOVETYPE_WALK &&
			pEnts[i]->GetMoveType() != MOVETYPE_STEP)
			continue;

		CBaseAnimating *pAnim = pEnts[i]->GetBaseAnimating();
		if (!pAnim)
			return;

		CPropParticleBall *pBall = dynamic_cast<CPropParticleBall*>(pEnts[i]);

		// Look only for VecBalls. If it's not a vecball, look through the other entities untill we find one.
		if (pBall == NULL)
		{
			continue;
		}

		// Push it along
		SuckEntity(pEnts[i], GetAbsOrigin());
	}

	// If we get disabled, stop thinking.
	if (m_bTarget)
	{ 
		SetNextThink(gpGlobals->curtime + 0.05f, "SuckThink");
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVecBox::MakeFlash()
{
	m_hParticleEffect2 = (CParticleSystem *) CreateEntityByName( "info_particle_system" );
	if ( m_hParticleEffect2 != NULL )
	{
		Vector vecAbsOrigin = GetAbsOrigin();
		QAngle angles = GetAbsAngles();
			
		int		attachment;
		attachment = LookupAttachment( "muzzle2" );

		GetAttachment( attachment, vecAbsOrigin );
		m_hParticleEffect2->SetAbsOrigin( vecAbsOrigin );
		m_hParticleEffect2->SetAbsAngles( angles );
		m_hParticleEffect2->SetParent(this);
		m_hParticleEffect2->KeyValue( "effect_name", VECBOX_HIT_PARTICLE );
		DispatchSpawn( m_hParticleEffect2 );

		RefreshParticleColors();

		m_hParticleEffect2->Activate();
		m_hParticleEffect2->StartParticleSystem();

	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVecBox::SetupSprites( void )
{
	if (m_bIsGhost)
		return;

	if(!m_bSpawnedSprites)
	{
		int i;
		for ( i = 0; i < 6; i++ )
		{
			if (m_hParticleEffect[i] == NULL )
			{
				const char *attachNames[] = 
				{
					"sprite_attach1",
					"sprite_attach2",
					"sprite_attach3",
					"sprite_attach4",
					"sprite_attach5",
					"sprite_attach6"
				};

				m_hParticleEffect[i] = (CParticleSystem *) CreateEntityByName( "info_particle_system" );

				Vector vecAbsOrigin = GetAbsOrigin();
				QAngle angles = GetAbsAngles();

				int		attachment;
				attachment = LookupAttachment( attachNames[i] );

				GetAttachment( attachment, vecAbsOrigin );
				m_hParticleEffect[i]->SetAbsOrigin( vecAbsOrigin );
				m_hParticleEffect[i]->SetAbsAngles( angles );
				m_hParticleEffect[i]->SetParent(this);
				m_hParticleEffect[i]->KeyValue( "start_active", "1" );
				m_hParticleEffect[i]->KeyValue( "effect_name", VECBOX_LIGHT );

				RefreshParticleColors();

				DispatchSpawn( m_hParticleEffect[i] );
				m_hParticleEffect[i]->Activate();
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVecBox::Think( void )
{
	if (m_bIsGhost)
		return;

	BaseClass::Think();
	m_bIsTouching = false;
	m_intLastBallHit = LastBallHit();
	DevMsg("Current Type: %i\n", m_intLastBallHit );

	switch ( LastBallHit() )
	{
		case BALL1:
			if (!m_bHasGhost)
			{
				if (m_bIsProtected)
				{
					if (m_intProtecting >= BALL0)
					{
						MakeGhost();
					}
					m_bIsProtected = false;

				}
				else
				{
					MakeGhost();
				}
			}

			break;
	}

	// If this exists, delete it.
	UTIL_Remove(m_hParticleEffect2);

	DevMsg("CVecBox: Done Thinking\n");
}

//Physcannon shit

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVecBox::OnPhysGunPickup( CBasePlayer *pPhysGunUser, PhysGunPickup_t reason )
{
	m_hPhysicsAttacker = pPhysGunUser;

	if ( reason == PICKED_UP_BY_CANNON || reason == PICKED_UP_BY_PLAYER )
	{
		m_OnPlayerPickup.FireOutput( pPhysGunUser, this );
	}

	if ( reason == PICKED_UP_BY_CANNON )
	{
		m_OnPhysGunPickup.FireOutput( pPhysGunUser, this );
	}
}
void CVecBox::OnPhysGunDrop( CBasePlayer *pPhysGunUser, PhysGunDrop_t reason )
{
	m_OnPhysGunDrop.FireOutput( pPhysGunUser, this );
}

//End of Physcannon shit

//-----------------------------------------------------------------------------
// Purpose: Pick me up!
//-----------------------------------------------------------------------------
int CVecBox::ObjectCaps()
{ 
	int caps = BaseClass::ObjectCaps();

	if ( !m_bIsGhost )
	{
		caps |= FCAP_IMPULSE_USE;
	}

	return caps;
}
void CVecBox::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	//DevMsg ("+USE WAS PRESSED ON ME\n");
	CBasePlayer *pPlayer = ToBasePlayer( pActivator );
	if ( pPlayer )
	{
		if ( HasSpawnFlags( SF_PHYSPROP_ENABLE_PICKUP_OUTPUT ) )
		{
			m_OnPlayerUse.FireOutput( this, this );
		}
		pPlayer->PickupObject( this );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVecBox::InputDissolve( inputdata_t &inputData )
{
	// We may have to think for this...
	m_OnFizzled.FireOutput( this, this );
	OnDissolve();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVecBox::InputEnableGravity(inputdata_t &inputData)
{
	VPhysicsGetObject()->EnableGravity(inputData.value.Bool());
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVecBox::InputMakeGhost(inputdata_t &inputData)
{
	MakeGhost();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVecBox::InputKillGhost( inputdata_t &inputData )
{
	// If we are not a ghost, kill OUR ghost if we have one.
	// If we are a ghost, kill ourselfs! **RIP EMO VecCube 2014-2015 we'll miss you. :(**
	if (!m_bIsGhost)
	{
		if (m_bHasGhost)
		{ 
			KillGhost();
		}
	}
	else
	{
		UTIL_Remove(this);
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVecBox::InputExplode( inputdata_t &inputData )
{
	Vector vecUp;
	GetVectors( NULL, NULL, &vecUp );
	Vector vecOrigin = WorldSpaceCenter() + ( vecUp * 12.0f );

	if (m_bHasGhost)
	{
		KillGhost();
	}

	if ( m_hParticleEffect2 != NULL)
	{
		UTIL_Remove(m_hParticleEffect2);
	}

	ExplosionCreate( GetAbsOrigin(), GetAbsAngles(), this, 100.0, 75.0, true);
	m_OnExplode.FireOutput( this, this );
	m_bGib = true;

	CPVSFilter filter( vecOrigin );
	for ( int i = 0; i < 4; i++ )
	{
		Vector gibVelocity = RandomVector(-100,100);
		int iModelIndex = modelinfo->GetModelIndex( g_PropDataSystem.GetRandomChunkModel( "MetalChunks" ) );	
		te->BreakModel( filter, 0.0, vecOrigin, GetAbsAngles(), Vector(40,40,40), gibVelocity, iModelIndex, 150, 4, 2.5, BREAK_METAL );
	}
	UTIL_Remove( this );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVecBox::InputDisableTouch(inputdata_t &inputData)
{
	m_DisableTouch = true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVecBox::InputEnableTouch(inputdata_t &inputData)
{
	m_DisableTouch = false;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVecBox::InputEnableTarget(inputdata_t &inputdata)
{
	m_bTarget = true;
	SetNextThink(gpGlobals->curtime + 0.05f, "SuckThink");
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVecBox::InputDisableTarget(inputdata_t &inputdata)
{
	m_bTarget = false;
}

/*
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVecBox::InputSetTargetMagitude(inputdata_t &inputdata)
{
	if (m_hTarget != NULL)
	{
		m_hTarget->SetMagitude(inputdata.value.Float());
	}
}
*/
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVecBox::InputSetTargetRadius(inputdata_t &inputdata)
{
	m_flRadius = inputdata.value.Float();
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CBaseEntity* ConvertToSimpleBox ( CBaseEntity* pEnt )
{
	CBaseEntity *pRetVal = NULL;
	pRetVal = CreateEntityByName( "simple_physics_prop" );

	pRetVal->KeyValue( "model", STRING(pEnt->GetModelName()) );
	pRetVal->SetAbsOrigin( pEnt->GetAbsOrigin() );
	pRetVal->SetAbsAngles( pEnt->GetAbsAngles() );
	pRetVal->Spawn();
	pRetVal->VPhysicsInitNormal( SOLID_VPHYSICS, 0, false );
	
	return pRetVal;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVecBox::OnDissolve()
{
	// First test if we have a child ghost. 
	// NOT PUTTING THIS HERRE WILL RESRORT TO CRASH!!
	if (m_bHasGhost && m_hBox != NULL )
	{
		KillGhost();
	}

	if ( m_hParticleEffect2 != NULL)
	{
		UTIL_Remove(m_hParticleEffect2);
	}

	// Set Skin and model
	SetRenderColor(255, 255, 255);

	// HACK! We have to switch a model because of our alpha masking!
	SetModel(VECBOX_MODEL_GHOST); 

	// Since the ghost box is smaller then the normal, we have to scale it back up.
	SetModelScale(1.15f);

	// Now, Fizzle like the shit in Portal!
	CBaseAnimating *pBaseAnimating = dynamic_cast<CBaseAnimating*>( this );

	if ( pBaseAnimating && !pBaseAnimating->IsDissolving() )
	{
		Vector vOldVel;
		AngularImpulse vOldAng;
		pBaseAnimating->GetVelocity( &vOldVel, &vOldAng );

		IPhysicsObject* pOldPhys = pBaseAnimating->VPhysicsGetObject();

		if ( pOldPhys && ( pOldPhys->GetGameFlags() & FVPHYSICS_PLAYER_HELD ) )
		{
			CVectronicPlayer  *pPlayer = (CVectronicPlayer *)GetPlayerHoldingEntity( pBaseAnimating );
			if( pPlayer )
			{
				// Modify the velocity for held objects so it gets away from the player
				pPlayer->ForceDropOfCarriedPhysObjects( pBaseAnimating );

				pPlayer->GetAbsVelocity();
				vOldVel = pPlayer->GetAbsVelocity() + Vector( pPlayer->EyeDirection2D().x * 4.0f, pPlayer->EyeDirection2D().y * 4.0f, -32.0f );
			}
		}

		// Swap object with an disolving physics model to avoid touch logic
		CBaseEntity *pDisolvingObj = ConvertToSimpleBox( pBaseAnimating );
		if ( pDisolvingObj )
		{
			// Remove old prop, transfer name and children to the new simple prop
			pDisolvingObj->SetName( pBaseAnimating->GetEntityName() );
			UTIL_TransferPoseParameters( pBaseAnimating, pDisolvingObj );
			TransferChildren( pBaseAnimating, pDisolvingObj );
			pDisolvingObj->SetCollisionGroup(COLLISION_GROUP_INTERACTIVE_DEBRIS);
			pBaseAnimating->AddSolidFlags( FSOLID_NOT_SOLID );
			pBaseAnimating->AddEffects( EF_NODRAW );

			IPhysicsObject* pPhys = pDisolvingObj->VPhysicsGetObject();
			if ( pPhys )
			{
				pPhys->EnableGravity( false );

				Vector vVel = vOldVel;
				AngularImpulse vAng = vOldAng;

				// Disolving hurts, damp and blur the motion a little
				vVel *= 0.5f;
				vAng.z += 20.0f;

				pPhys->SetVelocity( &vVel, &vAng );
			}

			pBaseAnimating->AddFlag( FL_DISSOLVING );
			pBaseAnimating->SetCollisionGroup(COLLISION_GROUP_DISSOLVING);
			UTIL_Remove( pBaseAnimating );
		}
		
		CBaseAnimating *pDisolvingAnimating = dynamic_cast<CBaseAnimating*>( pDisolvingObj );
		if ( pDisolvingAnimating ) 
		{
			pDisolvingAnimating->Dissolve( "", gpGlobals->curtime, false, ENTITY_DISSOLVE_NORMAL );
		}
	}

}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVecBox::Touch( CBaseEntity *pOther )
{
	CPropParticleBall *pBall = dynamic_cast<CPropParticleBall*>( pOther );

	if (!pBall)
		return;

	if (m_DisableTouch)
		return;

	if (!m_bIsTouching)
	{
		if (pBall->GetType() == BALL0 )
		{
			if (m_bIsGhost)
			{
				DevMsg ("CVecBox: Hit by a Ball But we are a ghost.\n");
				return;
			}
			else
			{
				EffectBlue();
				m_bActivated = true;
			}
		}

		if (pBall->GetType() == BALL1 )
		{
			if (m_bIsGhost)
			{
				DevMsg ("CVecBox: Hit by a Ball But we are a ghost.\n");
				return;
			}
			else
			{
				EffectGreen();
				m_bActivated = true;
			}
		}

		if (pBall->GetType() == BALL2 )
		{
			if (m_bIsGhost)
			{
				DevMsg ("CVecBox: Hit by a Ball But we are a ghost.\n");
				return;
			}
			else if( LastBallHit() != BALL_ANY )
			{
				EffectPurple();
				m_bActivated = true;
			}
		}

		if (pBall->GetType() == BALL3 )
		{
			if (m_bIsGhost)
			{
				DevMsg ("CVecBox: Hit by a Ball But we are a ghost.\n");
				return;
			}
			else
			{
				EffectBall3();
				m_bActivated = true;
			}
		}

		if (pBall->GetType() == BALL4 )
		{
			if (m_bIsGhost)
			{
				DevMsg ("CVecBox: Hit by a Ball But we are a ghost.\n");
				return;
			}
			else
			{
				EffectBall4();
				m_bActivated = true;
			}
		}

		if (pBall->GetType() == BALL5 )
		{
			if (m_bIsGhost)
			{
				DevMsg ("CVecBox: Hit by a Ball But we are a ghost.\n");
				return;
			}
			else
			{
				EffectBall5();
				m_bActivated = true;
			}
		}

		m_OnBall.FireOutput(this, this);
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVecBox::EffectBlue()
{
	if ( m_bIsGhost)
		return;

	m_bIsTouching = true;

	// If we get hit again by the same ball, reset!
	if ( LastBallHit() == BALL0 )
	{
		ResetBox();
		m_bActivated = false;
	}
	else //If we are hit with the blue ball, and we have not been before.
	{
		DevMsg ("CVecBox: Hit by Ball 0\n");
		SetCurrentBall (BALL0);
		VPhysicsGetObject()->EnableGravity( false );

		// Only do this when we got hit, and we were not protected.
		if (!m_bIsProtected)
		{
			m_OnBall0.FireOutput( this, this );
			EmitSound( "VecBox.Activate" );
		}
		else
		{
			EmitSound("VecBox.ClearShield");
		}

		// No matter what, kill our ghost if we have one!
		if (m_bHasGhost && m_hBox != NULL)
		{
			KillGhost();
		}

		// We are no longer protected
		m_bIsProtected = false;

		// Update our color!
		UpdateColor();
	}

	// Think sets skin and sprite.
	SetNextThink( gpGlobals->curtime + .1 );

	return;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVecBox::EffectGreen()
{
	if ( m_bIsGhost)
		return;

	m_bIsTouching = true;

	// If we get hit again by the same ball, reset!
	if ( LastBallHit() == BALL1 )
	{
		ResetBox();
		m_bActivated = false;
	}
	else //If we are hit with the green ball, and we have not been before.
	{
		DevMsg ("CVecBox: Hit by Ball 1\n");
		SetCurrentBall (BALL1);
		VPhysicsGetObject()->EnableGravity( true );

		// Only do this when we got hit, and we were not protected.
		if (!m_bIsProtected)
		{
			m_OnBall1.FireOutput( this, this );
			EmitSound( "VecBox.Activate" );
		}
		else
		{
			EmitSound("VecBox.ClearShield");
		}

		// Safety Check! If we don't have this handle created, then we don't have a ghost. Period!
		if (m_hBox == NULL)
		{
			DevMsg("CVecBox: Saftey Check! No Handle but we still have a ghost? Fixing!\n");
			m_bHasGhost = false;
		}

		// No matter what, kill our ghost if we have one!
		if (m_bHasGhost && !m_bIsProtected)
		{
			KillGhost();
		}

		// We are no longer protected
		//m_bIsProtected = false;

		// Update our color!
		UpdateColor();
	}

	// Think function will make a new ghost for us.
	SetNextThink( gpGlobals->curtime + .1 );

	return;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVecBox::EffectPurple()
{
	if ( m_bIsGhost)
		return;

	m_bIsTouching = true;

	// OK! ball 2 is diffrent. Instead of clearing out the
	// previous activation and replacing it with another one,
	// Ball 2 adds a sheild to the previous activaton. 

	// Don't protect null boxes!
	if ( LastBallHit() == BALL_ANY )
		return;

	// If we are yellow and we are hit again by yellow, Restore our previous!
	if ( LastBallHit() == BALL2 )
	{
		if ( m_intProtecting == BALL0 )
		{
			EffectBlue();
		}
		else if ( m_intProtecting == BALL1 )
		{
			EffectGreen();
		}
		else if ( m_intProtecting == BALL3 )
		{
			EffectBall3();
		}
		else if ( m_intProtecting == BALL4 )
		{
			EffectBall4();
		}
		else if ( m_intProtecting == BALL5 )
		{
			EffectBall5();
		}
		return;
	}

	// Save our previous activations in another int.
	MakeGlowBasedOnBallColor();
	m_intProtecting = LastBallHit();

	/*
	// Save our previous activations in another int.
	if ( LastBallHit() == BALL0 )
	{
		MakeGlowBasedOnBallColor();
		m_intProtecting = BALL0;
	}
	else if ( LastBallHit() == BALL1 )
	{
		MakeGlowBasedOnBallColor();
		m_intProtecting = BALL1;
	}
	else if ( LastBallHit() == BALL3 )
	{
		MakeGlowBasedOnBallColor();
		m_intProtecting = BALL3;
	}
	else if ( LastBallHit() == BALL4 )
	{
		MakeGlowBasedOnBallColor();
		m_intProtecting = BALL4;
	}
	else if ( LastBallHit() == BALL5 )
	{
		MakeGlowBasedOnBallColor();
		m_intProtecting = BALL5;
	}
	*/

	// This bool is used to tell the box, and other entities that we are protected!
	if (m_intProtecting > BALL_ANY)
	{
		DevMsg ("CVecBox: Saved previous activaton!\n");
		m_bIsProtected = true;
	}

	// Do the usual bullshit.
	DevMsg ("CVecBox: Hit by Ball 2\n");
	m_OnBall2.FireOutput( this, this );
	EmitSound( "VecBox.Activate" );

	SetCurrentBall( BALL2 );
	UpdateColor();

	SetNextThink( gpGlobals->curtime + .1 );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVecBox::EffectBall3()
{
	if ( m_bIsGhost)
		return;

	m_bIsTouching = true;
	VPhysicsGetObject()->EnableGravity( true );

	// If we get hit again by the same ball, reset!
	if ( LastBallHit() == BALL3 )
	{
		ResetBox();
		m_bActivated = false;
	}
	else //If we are hit with the red ball, and we have not been before.
	{
		DevMsg ("CVecBox: Hit by Ball 3\n");
		SetCurrentBall( BALL3 );

		// Only do this when we got hit, and we were not protected.
		if (!m_bIsProtected)
		{
			m_OnBall3.FireOutput( this, this );
			EmitSound( "VecBox.Activate" );
		}
		else
		{
			EmitSound("VecBox.ClearShield");
		}

		// No matter what, kill our ghost if we have one!
		if (m_bHasGhost && m_hBox != NULL)
		{
			KillGhost();
		}

		// We are no longer protected
		m_bIsProtected = false;

		// Update our color!
		UpdateColor();
	}

	// Think sets skin and sprite.
	SetNextThink( gpGlobals->curtime + .1 );

	return;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVecBox::EffectBall4()
{
	if ( m_bIsGhost)
		return;

	m_bIsTouching = true;
	VPhysicsGetObject()->EnableGravity( true );

	// If we get hit again by the same ball, reset!
	if ( LastBallHit() == BALL4 )
	{
		ResetBox();
		m_bActivated = false;
	}
	else //If we are hit with the red ball, and we have not been before.
	{
		DevMsg ("CVecBox: Hit by Ball 4\n");
		SetCurrentBall( BALL4 );

		// Only do this when we got hit, and we were not protected.
		if (!m_bIsProtected)
		{
			m_OnBall4.FireOutput( this, this );
			EmitSound( "VecBox.Activate" );
		}
		else
		{
			EmitSound("VecBox.ClearShield");
		}

		// No matter what, kill our ghost if we have one!
		if (m_bHasGhost && m_hBox != NULL)
		{
			KillGhost();
		}

		// We are no longer protected
		m_bIsProtected = false;

		// Update our color!
		UpdateColor();
	}

	// Think sets skin and sprite.
	SetNextThink( gpGlobals->curtime + .1 );

	return;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVecBox::EffectBall5()
{
	if ( m_bIsGhost)
		return;

	m_bIsTouching = true;
	VPhysicsGetObject()->EnableGravity( true );

	// If we get hit again by the same ball, reset!
	if ( LastBallHit() == BALL5 )
	{
		ResetBox();
		m_bActivated = false;
	}
	else //If we are hit with the red ball, and we have not been before.
	{
		DevMsg ("CVecBox: Hit by Ball 5\n");
		SetCurrentBall( BALL5 );

		// Only do this when we got hit, and we were not protected.
		if (!m_bIsProtected)
		{
			m_OnBall5.FireOutput( this, this );
			EmitSound( "VecBox.Activate" );
		}
		else
		{
			EmitSound("VecBox.ClearShield");
		}

		// No matter what, kill our ghost if we have one!
		if (m_bHasGhost && m_hBox != NULL)
		{
			KillGhost();
		}

		// We are no longer protected
		m_bIsProtected = false;

		// Update our color!
		UpdateColor();
	}

	// Think sets skin and sprite.
	SetNextThink( gpGlobals->curtime + .1 );

	return;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVecBox::Clear( void )
{
	if (!m_bActivated )
		return;

	// Hold up! If we are saved by a purple ball, clear only the purple ball and return to the previous activation!
	if (m_bIsProtected)
	{
		if ( m_intProtecting == BALL0 )
		{
			DevMsg ("CVecBox: Going back to previous activation!\n");
			EffectBlue();
		}
		else if ( m_intProtecting == BALL1 )
		{
			DevMsg ("CVecBox: Going back to previous activation!\n");

			// Instead of sending it back to EffectGreen, we do the same thing
			// only this time we don't kill and remake our ghost!
			SetCurrentBall( BALL1 );
			VPhysicsGetObject()->EnableGravity( true );
			EmitSound("VecBox.ClearShield");

			// We are no longer protected
			m_bIsProtected = false;

			// Update our color!
			UpdateColor();
		}
		else if ( m_intProtecting == BALL3 )
		{
			DevMsg ("CVecBox: Going back to previous activation!\n");
			EffectBall3();
		}
		else if ( m_intProtecting == BALL4 )
		{
			DevMsg ("CVecBox: Going back to previous activation!\n");
			EffectBall4();
		}
		else if ( m_intProtecting == BALL5 )
		{
			DevMsg ("CVecBox: Going back to previous activation!\n");
			EffectBall5();
		}

		m_intProtecting = BALL_ANY;
		m_bIsProtected = false;
		return;
	}

	//Debug msg
	DevMsg ("CVecBox: Ok, we are activated. CLEAN ME!!\n");

	//Reset Vphysics
	VPhysicsGetObject()->Wake();
	VPhysicsGetObject()->EnableGravity( true );

	ResetBox();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVecBox::ResetBox()
{
	DevMsg ("CVecBox: A Ball of this type already hit this. Resetting!\n");
	m_OnReset.FireOutput( this, this );

	// If we are not 0, play the sound and reset it to 0. Because 0 means no Punts.
	if (LastBallHit() != BALL_ANY )
	{
		//Play Sound Zzzzz
		EmitSound( "VecBox.Deactivate" );

		//Set int to 0
		SetCurrentBall( BALL_ANY );

		// Whoops, we might need this here!
		m_intProtecting = BALL_ANY;
	}
	
	// Ok We are off.
	m_bActivated = false;

	// We are no longer protected
	m_bIsProtected = false;

	// No matter what, kill our ghost if we have one!
	if (m_bHasGhost && m_hBox != NULL)
	{
		KillGhost();
	}

	//Reset Vphysics
	VPhysicsGetObject()->EnableGravity( true );

	RemoveGlowEffect();

	// Update our color back to white.
	UpdateColor();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVecBox::UpdateColor()
{
	SetRenderColorToBalls();

	if ( LastBallHit() == BALL_ANY )
	{
		//m_nSkin = 0;
		DispatchParticleEffect (VECBOX_CLEAR_PARTICLE, PATTACH_ABSORIGIN, this );
	}
	else
	{
		MakeFlash();
	}

	if ( LastBallHit() != BALL2)
	{
		RemoveGlowEffect();
	}

	/*
	if ( LastBallHit() == VECTRONIC_BALL_BLUE)
	{
		MakeFlash();
		//m_nSkin = 1;
		RemoveGlowEffect();

		//DispatchParticleEffect (VECBOX_HIT0_PARTICLE, PATTACH_ABSORIGIN, this );
		Vector ParticleColor (BALL0_COLOR_R, BALL0_COLOR_G, BALL0_COLOR_B);
		m_hParticleEffect2->SetControlPointValue( 7, ParticleColor );
		MakeFlash();
		
	}

	if ( LastBallHit() == VECTRONIC_BALL_GREEN)
	{
		MakeFlash();
		//m_nSkin = 2;
		RemoveGlowEffect();
		//DispatchParticleEffect (VECBOX_HIT1_PARTICLE, PATTACH_ABSORIGIN, this );
		Vector ParticleColor (BALL1_COLOR_R, BALL1_COLOR_G, BALL1_COLOR_B);
		m_hParticleEffect2->SetControlPointValue( 7, ParticleColor );
		
	}

	if ( LastBallHit() == VECTRONIC_BALL_PURPLE)
	{
		MakeFlash();
		//m_nSkin = 3;
		//DispatchParticleEffect (VECBOX_HIT2_PARTICLE, PATTACH_ABSORIGIN, this );
		Vector ParticleColor (BALL2_COLOR_R, BALL2_COLOR_G, BALL2_COLOR_B);
		m_hParticleEffect2->SetControlPointValue( 7, ParticleColor );
	}

	if ( LastBallHit() == VECTRONIC_BALL_3)
	{
		MakeFlash();
		//m_nSkin = 4;
		//DispatchParticleEffect (VECBOX_HIT3_PARTICLE, PATTACH_ABSORIGIN, this );
		Vector ParticleColor (vectronic_vecball3_color_r.GetInt(), vectronic_vecball3_color_g.GetInt(), vectronic_vecball3_color_b.GetInt());
		m_hParticleEffect2->SetControlPointValue( 7, ParticleColor );
		RemoveGlowEffect();
	}

	if ( LastBallHit() == VECTRONIC_BALL_4)
	{
		MakeFlash();
		//m_nSkin = 5;
		//DispatchParticleEffect (VECBOX_HIT3_PARTICLE, PATTACH_ABSORIGIN, this );
		Vector ParticleColor (vectronic_vecball4_color_r.GetInt(), vectronic_vecball4_color_g.GetInt(), vectronic_vecball4_color_b.GetInt());
		m_hParticleEffect2->SetControlPointValue( 7, ParticleColor );
		RemoveGlowEffect();
	}

	if ( LastBallHit() == VECTRONIC_BALL_5)
	{
		MakeFlash();
		//m_nSkin = 6;
		//DispatchParticleEffect (VECBOX_HIT3_PARTICLE, PATTACH_ABSORIGIN, this );
		Vector ParticleColor (vectronic_vecball5_color_r.GetInt(), vectronic_vecball5_color_g.GetInt(), vectronic_vecball5_color_b.GetInt());
		m_hParticleEffect2->SetControlPointValue( 7, ParticleColor );
		RemoveGlowEffect();
	}
	*/
}

void CVecBox::RefreshParticleColors()
{
	Vector Color (GetOurBallColor_R(), GetOurBallColor_G(), GetOurBallColor_B());

	int i;
	for ( i = 0; i < 6; i++ )
	{
		if (m_hParticleEffect[i] != NULL )
		{
			m_hParticleEffect[i]->SetControlPointValue( 7, Color );
			/*
			if ( LastBallHit() == VECTRONIC_BALL_NONE)
			{
				Vector Color (255,255,255);
				m_hParticleEffect[i]->SetControlPointValue( 7, Color );
			}
			if ( LastBallHit() == VECTRONIC_BALL_BLUE)
			{
				Vector Color (BALL0_COLOR_R, BALL0_COLOR_G, BALL0_COLOR_B);
				m_hParticleEffect[i]->SetControlPointValue( 7, Color );
			}
			if ( LastBallHit() == VECTRONIC_BALL_GREEN)
			{
				Vector Color (BALL1_COLOR_R, BALL1_COLOR_G, BALL1_COLOR_B);
				m_hParticleEffect[i]->SetControlPointValue( 7, Color );
			}
			if ( LastBallHit() == VECTRONIC_BALL_PURPLE)
			{
				Vector Color (BALL2_COLOR_R, BALL2_COLOR_G, BALL2_COLOR_B);
				m_hParticleEffect[i]->SetControlPointValue( 7, Color );
			}
			if ( LastBallHit() == VECTRONIC_BALL_3)
			{
				Vector Color (vectronic_vecball3_color_r.GetInt(),vectronic_vecball3_color_g.GetInt(),vectronic_vecball3_color_b.GetInt());
				m_hParticleEffect[i]->SetControlPointValue( 7, Color );
			}
			if ( LastBallHit() == VECTRONIC_BALL_4)
			{
				Vector Color (vectronic_vecball4_color_r.GetInt(),vectronic_vecball4_color_g.GetInt(),vectronic_vecball4_color_b.GetInt());
				m_hParticleEffect[i]->SetControlPointValue( 7, Color );
			}
			if ( LastBallHit() == VECTRONIC_BALL_5)
			{
				Vector Color (vectronic_vecball5_color_r.GetInt(),vectronic_vecball5_color_g.GetInt(),vectronic_vecball5_color_b.GetInt());
				m_hParticleEffect[i]->SetControlPointValue( 7, Color );
			}
			*/

		}
	}

	if (m_hParticleEffect2 != NULL )
	{
		m_hParticleEffect2->SetControlPointValue( 7, Color );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVecBox::MakeGhost()
{
	m_bHasGhost = true;
	//CVecBox *pBox = static_cast<CVecBox*>( CreateEntityByName( "prop_vecbox" ) );
	m_hBox = (CVecBox *) CreateEntityByName( "prop_vecbox" );

	if ( m_hBox == NULL )
		return;

	Vector vecAbsOrigin = GetAbsOrigin();
	Vector zaxis;

	m_hBox->SetAbsOrigin( vecAbsOrigin );

	Vector vDirection;

	vDirection *= 0.0f;

	// Woof Woof, The ghost should get the boxes angles and then spawn.
	QAngle qAngles = this->GetAbsAngles();
	SetOwnerEntity( m_hBox );
	GetOwnerEntity()->SetAbsAngles(qAngles);

	m_hBox->m_bIsGhost = true;
	m_hBox->SetCurrentBall(GHOSTID);
	//m_hBox->SetCollisionGroup(COLLISION_GROUP_DEBRIS);
	m_hBox->Spawn();
	m_hBox->MakeGhost2();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVecBox::MakeGhost2()
{
	// If we make it here, we are a ghost!
	DevMsg ("CVecBox: We have made a ghost!\n");
	SetModel(VECBOX_MODEL_GHOST);

	SetRenderColor(230,230,230);

	SetGlowVector( 0.76f, 0.76f, 0.76f);
	//SetGlowVector( 0.6078431372549019f, 0.6862745098039216f, 0.6039215686274509f);
	AddGlowEffect();

	AddEffects(EF_NOSHADOW);
	m_nSkin = 0;
	VPhysicsGetObject()->EnableMotion ( false );
	SetSolid( SOLID_VPHYSICS );

}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CVecBox::OnTakeDamage(const CTakeDamageInfo &inputInfo)
{
	//BaseClass::OnTakeDamage(inputInfo);

	CTakeDamageInfo inputInfoCopy(inputInfo);

	CBaseEntity *pAttacker = inputInfoCopy.GetAttacker();
	CBaseEntity *pInflictor = inputInfoCopy.GetInflictor();

	// Refuse damage from prop_weighted_cube.
	if ((pAttacker && FClassnameIs(pAttacker, "prop_particle_ball")) ||
		(pInflictor && FClassnameIs(pInflictor, "prop_particle_ball")))
	{
		inputInfoCopy.SetDamage(0.0f);
	}

	return BaseClass::OnTakeDamage(inputInfoCopy);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVecBox::KillGhost()
{
	variant_t emptyVariant;
	GetOwnerEntity()->AcceptInput( "KillGhost", this, this, emptyVariant, 0 );
	DevMsg ("CVecBox: Ghost is dead\n");

	// Remove the Ghost stuff from the host box.
	UTIL_Remove(m_hBox);
	m_bIsGhost = false;
	m_bHasGhost = false;
}

// ###################################################################
//	> FilterClass
// ###################################################################
class CFilterBoxType : public CBaseFilter
{
	DECLARE_CLASS( CFilterBoxType, CBaseFilter );
	DECLARE_DATADESC();

public:
	int m_iBoxType;

	bool PassesFilterImpl( CBaseEntity *pCaller, CBaseEntity *pEntity )
	{
		CVecBox *pBox = dynamic_cast<CVecBox*>(pEntity );

		if ( pBox )
		{
			return pBox->GetState() == m_iBoxType;
		}
		return false;
	}
};

LINK_ENTITY_TO_CLASS( filter_vecbox_activation, CFilterBoxType );

BEGIN_DATADESC( CFilterBoxType )
	// Keyfields
	DEFINE_KEYFIELD( m_iBoxType,	FIELD_INTEGER,	"boxtype" ),
END_DATADESC()