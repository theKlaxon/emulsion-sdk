//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef PROP_PARTILCE_PROJECTILE_H
#define PROP_PARTILCE_PROJECTILE_H
#ifdef _WIN32
#pragma once
#endif

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include "player_pickup.h"	// for Particle ball inheritance
#include "particle_parse.h"
#include "particle_system.h"
#include "prop_vecbox.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class CSpriteTrail;
//-----------------------------------------------------------------------------
// Looks for enemies, bounces a max # of times before it breaks
//-----------------------------------------------------------------------------
class CPropParticleBall : public CBaseAnimating, public CDefaultPlayerPickupVPhysics
{
	DECLARE_CLASS( CPropParticleBall, CBaseAnimating );
	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();

public:
	virtual void Precache();
	virtual void Spawn();
	void	MakeParticleSystem();
	void	MakeHitParticleSystem();
	virtual void UpdateOnRemove();
	void StopLoopingSounds();
	void GetTail();

	virtual void OnPhysGunPickup( CBasePlayer *pPhysGunUser, PhysGunPickup_t reason );
	virtual void OnPhysGunDrop( CBasePlayer *pPhysGunUser, PhysGunDrop_t Reason );
	virtual void VPhysicsCollision( int index, gamevcollisionevent_t *pEvent );

	virtual bool OverridePropdata();
	virtual bool CreateVPhysics();

	virtual void ExplodeThink( void );

	// Override of IPlayerPickupVPhysics;
	virtual bool ShouldPuntUseLaunchForces( PhysGunForce_t reason ) { return ( reason == PHYSGUN_FORCE_PUNTED ); }

	void SetRadius( float flRadius );
	void SetSpeed( float flSpeed ) { m_flSpeed = flSpeed; }
	float GetSpeed( void ) const { return m_flSpeed; }

	void CaptureBySpawner( );
	bool IsBeingCaptured() const { return m_bCaptureInProgress; }

	void ReplaceInSpawner( float flSpeed );

	// Input
	void InputExplode( inputdata_t &inputdata );
	void InputFadeAndRespawn( inputdata_t &inputdata );
	void InputKill( inputdata_t &inputdata );
	void InputSocketed( inputdata_t &inputdata );

	enum
	{
		STATE_NOT_THROWN = 0,
		STATE_HOLDING,
		STATE_THROWN,
		STATE_LAUNCHED, //by a Particle_ball launcher
	};

	void SetState( int state );
	bool IsInField() const;

	void StartWhizSoundThink( void );

	void StartLifetime( float flDuration );
	void ClearLifetime( );
	void SetMass( float mass );
	void SetType( int inttype );

	void SetWeaponLaunched( bool state = true ) { m_bWeaponLaunched = state; m_bLaunched = state; }
	bool WasWeaponLaunched( void ) const { return m_bWeaponLaunched; }

	bool WasFiredByNPC() const { return (GetOwnerEntity() && GetOwnerEntity()->IsNPC()); }

	bool ShouldHitPlayer() const;

	virtual CBasePlayer *HasPhysicsAttacker( float dt );

	void	NotifySpawnerOfRemoval( void );

	//int m_intType;
	int GetType(){return m_intType;};

	float	LastCaptureTime() const;

//	unsigned char GetState() const { return m_nState;	}
	unsigned char GetState() const { return m_intType;	}

	int  NumBounces( void ) const { return m_nBounceCount; }

	void SetMaxBounces( int iBounces )
	{
		m_nMaxBounces = iBounces;
	}

	void SetEmitState( bool bEmit )
	{
		m_bEmit = bEmit;
	}

	void SetOriginalOwner( CBaseEntity *pEntity ) { m_hOriginalOwner = pEntity; }
	CBaseEntity *GetOriginalOwner() { return m_hOriginalOwner; }

	// Pow!
	bool m_bHitBox;
	void DoExplosion();

private:

	void SetPlayerLaunched( CBasePlayer *pOwner );

	float GetBallHoldDissolveTime();
	float GetBallHoldSoundRampTime();

	void StartAnimating( void );
	void StopAnimating( void );

	void SetBallAsLaunched( void );

	void CollisionEventToTrace( int index, gamevcollisionevent_t *pEvent, trace_t &tr );
	bool DissolveEntity( CBaseEntity *pEntity );
	void OnHitEntity( CBaseEntity *pHitEntity, float flSpeed, int index, gamevcollisionevent_t *pEvent );
	void DoImpactEffect( const Vector &preVelocity, int index, gamevcollisionevent_t *pEvent );

	// Bounce inside the spawner: 
	void BounceInSpawner( float flSpeed, int index, gamevcollisionevent_t *pEvent );

	bool IsAttractiveTarget( CBaseEntity *pEntity );

	// Deflects the ball toward enemies in case of a collision 
	void DeflectTowardEnemy( float flSpeed, int index, gamevcollisionevent_t *pEvent );

	// Is this something we can potentially dissolve? 
	bool IsHittableEntity( CBaseEntity *pHitEntity );

	// Sucky. 
	void WhizSoundThink();
	void DieThink();
	void DissolveThink();
	void DissolveRampSoundThink();
	void AnimThink( void );

	void FadeOut( float flDuration );


	bool OutOfBounces( void ) const
	{
		return ( m_nState == STATE_LAUNCHED && m_nMaxBounces != 0 && m_nBounceCount >= m_nMaxBounces );
	}

private:

	int		m_nBounceCount;
	int		m_nMaxBounces;
	bool	m_bBounceDie;

	float	m_flLastBounceTime;

	bool	m_bFiredGrabbedOutput;
	bool	m_bStruckEntity;		// Has hit an entity already (control accuracy)
	bool	m_bWeaponLaunched;		// Means this was fired from the AR2
	bool	m_bForward;				// Movement direction in ball spawner

	unsigned char m_nState;
	bool	m_bCaptureInProgress;

	float	m_flSpeed;

	CSpriteTrail *m_pGlowTrail;
	CSoundPatch *m_pHoldingSound;

	float	m_flNextDamageTime;
	float	m_flLastCaptureTime;

	EHANDLE m_hOriginalOwner;
	CHandle<CParticleSystem> m_hParticleEffect;
	CHandle<CParticleSystem> m_hParticleEffect2;
	//CHandle<CVecBox> m_hBox;

	CNetworkVar( bool, m_bEmit );
	CNetworkVar( bool, m_bHeld );
	CNetworkVar( int, m_intType );
	CNetworkVar( bool, m_bLaunched );
	CNetworkVar( float, m_flRadius );
};

// Creates a Particle ball
//CBaseEntity *CreateParticleBall( const Vector &origin, const Vector &velocity, float mass, int type, float lifetime, CBaseEntity *pOwner );
CBaseEntity *CreateParticleBall0( const Vector &origin, const Vector &velocity, float mass, float type, float lifetime, CBaseEntity *pOwner );
//CBaseEntity *CreateParticleBall1( const Vector &origin, const Vector &velocity, float mass, float lifetime, CBaseEntity *pOwner );
//CBaseEntity *CreateParticleBall2( const Vector &origin, const Vector &velocity, float mass, float lifetime, CBaseEntity *pOwner );

// Query function to find out if a physics object is a Particle ball (used for collision checks)
bool UTIL_IsParticleBall( CBaseEntity *pEntity );
bool UTIL_IsParticleBallDefinite( CBaseEntity *pEntity );
bool UTIL_IsAR2ParticleBall( CBaseEntity *pEntity );

#endif // PROP_Particle_BALL_H
