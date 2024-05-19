//=========== Copyright © 2014, rHetorical, All rights reserved. =============
//
// Purpose: 
//
//=============================================================================

#ifndef PROP_VECBALL_DISPENSER_H
#define PROP_VECBALL_DISPENSER_H
#ifdef _WIN32
#pragma once
#endif

#include "cbase.h"
#include "vectronic_vecall_system.h"
#include "prop_vectronic_projectile.h"
#include "baseanimating.h"
#include "sprite.h"
#include "particle_parse.h"
#include "particle_system.h"

//----------------------------------------------------------
// Particle attachment spawning. Read InputTestEntity for info. 
//----------------------------------------------------------
class CVecBallDispenser : public CVectronicProp
{
public:
	DECLARE_CLASS( CVecBallDispenser, CVectronicProp );
	DECLARE_DATADESC();
 
	CVecBallDispenser()
	{
		//m_intType = 0;
		m_bDisabled = false;
		m_bAutoRegen = true;
	}

	void Spawn( void );
	virtual void RefreshParticleColors();
	void Precache( void );
	void Think ( void );
	void Touch( CBaseEntity *pOther );

	// Sound
	void CreateSounds();
	void StopLoopingSounds();

	// Start enabled?
//	bool m_bEnabled;
	bool m_bDispatched;

	void TurnOn();
	void Restore( bool bSound = true ); // used by our think.

//	void SetupSprite();
//	bool m_bSpawnedSprite;

	// Functions to call for the right ball dispensers.
	void EnableBall();

	void InputEnable( inputdata_t &inputdata );
	void InputDisable( inputdata_t &inputdata );
	void InputRespawn ( inputdata_t &inputData );

private:
 
	//int	 m_intType; // 0 = Blue, 1 = Green
	bool m_bDisabled;

	bool m_bAutoRegen; // Put here incase we don't wish to auto regenerate.

	CSoundPatch		*m_pLoopSound;

	CHandle<CParticleSystem> m_hParticleEffect;
	CHandle<CParticleSystem> m_hBaseParticleEffect;
	CHandle<CParticleSystem> m_hBaseParticleEffect2;

	COutputEvent m_OnBallEquipped;
};

#endif // PROP_VECBALL_DISPENSER_H