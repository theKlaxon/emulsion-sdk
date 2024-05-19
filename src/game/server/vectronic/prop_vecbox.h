//=========== Copyright © 2013, rHetorical, All rights reserved. =============
//
// Purpose: 
//		
//=============================================================================

#ifndef PROP_VECBOX_H
#define PROP_VECBOX_H

#ifdef _WIN32
#pragma once
#endif

#include "player_pickup.h"
#include "vectronic_vecall_system.h"
#include "baseglowanimating.h"
#include "sprite.h"
#include "point_vectronic_suck.h"
#ifdef SWARM_DLL
#include "particle_parse.h"
#include "particle_system.h"
#endif

#define VECBOX_MODEL "models/props/puzzlebox.mdl"
#define VECBOX_MODEL_GHOST "models/props/puzzlebox_ghost.mdl"

// In SWARM, we replace this with a particle!
#ifdef SWARM_DLL
#define VECBOX_LIGHT "test_light"
#else
#define VECBOX_SPRITE "sprites/light_glow03.vmt"
#endif

#define GHOSTID 9

//-----------------------------------------------------------------------------
// Purpose: Base
//-----------------------------------------------------------------------------
class CVecBox : public CVectronicProp, public CDefaultPlayerPickupVPhysics
{
	DECLARE_CLASS( CVecBox, CVectronicProp );
public:
	
	DECLARE_DATADESC();
	DECLARE_PREDICTABLE();

	bool CreateVPhysics()
	{
		VPhysicsInitNormal( SOLID_VPHYSICS, 0, false );
		return true;
	}

	//Constructor
	CVecBox();

	void Spawn( void );
	void Precache( void );
	void Think( void );
	void Touch( CBaseEntity *pOther );
	void UpdateColor();
	void SetupSprites();
	virtual void RefreshParticleColors();

	void MakeFlash();

	void EffectBlue();
	void EffectGreen();
	void EffectPurple();
	void EffectBall3();
	void EffectBall4();
	void EffectBall5();

	bool IsProtected(){return m_bIsProtected;};

	virtual int OnTakeDamage(const CTakeDamageInfo &inputInfo);

	int m_intProtecting;

	// Report back what we are protecting.
	int IsProtecting()
	{
		return m_intProtecting;
	};

	void SetCurrentBall ( int current )
	{
		 m_intType = current;
	};

	int LastBallHit() { return m_intType; };

	bool IsGhost()
	{
		return m_bIsGhost;
	};

	bool HasGhost()
	{
		return m_bHasGhost;
	};

	void ResetBox();

	void OnDissolve();

	void MakeGhost();
	void MakeGhost2();
	void KillGhost();

	// Use
	void OnPhysGunPickup( CBasePlayer *pPhysGunUser, PhysGunPickup_t reason );
	void OnPhysGunDrop( CBasePlayer *pPhysGunUser, PhysGunDrop_t reason );
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	int ObjectCaps();

	//Clear effects
	void Clear();

	unsigned char GetState() const { return m_intLastBallHit;	}

	void InputEnableGravity(inputdata_t &inputData);
	void InputDissolve( inputdata_t &inputData );
	void InputExplode( inputdata_t &inputData );
	void InputMakeGhost(inputdata_t &inputData);
	void InputKillGhost( inputdata_t &inputData );

	// Target
	void InputEnableTarget(inputdata_t &inputData);
	void InputDisableTarget(inputdata_t &inputData);
	//void InputSetTargetMagitude(inputdata_t &inputData);
	void InputSetTargetRadius(inputdata_t &inputData);

	void InputDisableTouch(inputdata_t &inputData);
	void InputEnableTouch(inputdata_t &inputData);

private:

	CHandle<CVecBox> m_hBox;

	// Target
	void SuckThink(void);
	inline void SuckEntity(CBaseEntity *pTarget, const Vector &vecPushPoint);

	bool m_bTarget;
	float m_flRadius;

	bool	m_bActivated;
	bool	m_bIsGhost; //Boo! I'm a Ghost!
	bool	m_bHasGhost; // We have a Ghost! AHH!
	bool	m_bIsProtected; // Do we have a sheild?

	bool	m_bGib; //Our Gibs  
	bool	m_DisableTouch;

	bool m_bIsTouching;
	int m_intLastBallHit;

	bool m_bSpawnedSprites;

	//int m_intStartActivation;

	float		m_flConeDegrees;
	CHandle<CBasePlayer>	m_hPhysicsAttacker;


// In SWARM, we replace this with a particle!
#ifndef SWARM_DLL
	CHandle<CSprite>		m_hSprite[6];
#else
	CHandle<CParticleSystem>		m_hParticleEffect[6];
	CHandle<CParticleSystem>		m_hParticleEffect2;
#endif

	// 0 = none
	// 1 = 0
	// 2 = 1 ... etc
	Vector MdlTop;

	// Pickup
	COutputEvent m_OnPlayerUse;
	COutputEvent m_OnPlayerPickup;
	COutputEvent m_OnPhysGunPickup;
	COutputEvent m_OnPhysGunDrop;

	// Punt Outputs
	COutputEvent m_OnBall;
	COutputEvent m_OnBall0;
	COutputEvent m_OnBall1;
	COutputEvent m_OnBall2;

	COutputEvent m_OnBall3;
	COutputEvent m_OnBall4;
	COutputEvent m_OnBall5;

	COutputEvent m_OnReset;

	// Death!
	COutputEvent m_OnFizzled;
	COutputEvent m_OnExplode;

};
#endif // PROP_VECBOX_H
