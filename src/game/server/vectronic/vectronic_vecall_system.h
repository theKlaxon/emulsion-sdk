//====== Copyright © 1996-2003, Valve Corporation, All rights reserved. =======
//
// Purpose: Game rules for Scratch
//
//=============================================================================

#ifndef VECTRONIC_VECBALL_SYSTEM_H
#define VECTRONIC_VECBALL_SYSTEM_H

#ifdef _WIN32
#pragma once
#endif

#include "cbase.h"
#include "baseglowanimating.h"

#include "particle_parse.h"
#include "particle_system.h"

//----------------------------------------------------------
// BaseClass for our VecProps.
//----------------------------------------------------------
class CVectronicProp : public CBaseGlowAnimating
{
public:
	DECLARE_CLASS( CVectronicProp, CBaseGlowAnimating );
	DECLARE_DATADESC();

	CVectronicProp()
	{
	}

	void Precache( void );
	void Spawn ( void );
	void ThinkCheckBallColors();

	virtual void RefreshSpriteColors(){};
	virtual void RefreshParticleColors(){};
	void		MakeGlowBasedOnBallColor();

	enum Types_t
	{
		BALL0 = 0,
		BALL1 = 1,
		BALL2 = 2,
		BALL3 = 3,
		BALL4 = 4,
		BALL5 = 5,

		BALL_ANY = -1,
	};

	int GetOurBallColor_R();
	int GetOurBallColor_G();
	int GetOurBallColor_B();

	void SetRenderColorToBalls();
	int	 m_intType; 

	int GetOurType()
	{
		return m_intType;
	};

	void SetOurType( int type )
	{
		m_intType = type;
	};

private:

	int n_intBall3_r;
	int n_intBall3_g;
	int n_intBall3_b;

	int n_intBall4_r;
	int n_intBall4_g;
	int n_intBall4_b;

	int n_intBall5_r;
	int n_intBall5_g;
	int n_intBall5_b;
	
};

//----------------------------------------------------------
// Cleanser particle.
//----------------------------------------------------------
class CPointCleanser : public CPointEntity
{
public:
	DECLARE_CLASS( CPointCleanser, CPointEntity );
	DECLARE_DATADESC();
 
	CPointCleanser()
	{
	}

	//void Precache( void );
	void Spawn( void );
	void TurnOn( void );
	void ChangeColor();
	void ThinkCheckBallColors();

	int GetOurType()
	{
		return m_nFilterType;
	};

private:

	enum Types_t
	{
		BALL0 = 0,
		BALL1 = 1,
		BALL2 = 2,
		BALL3 = 3,
		BALL4 = 4,
		BALL5 = 5,

		BALL_ANY = -1,
	};

	bool m_bDispatched;
	bool m_bWide;
	bool m_bDisabled;
	int	 m_nFilterType;

	void InputTurnOn ( inputdata_t &inputData );
	void InputTurnOff ( inputdata_t &inputData );

	CHandle<CParticleSystem> m_hParticleEffect;

	int n_intBall3_r;
	int n_intBall3_g;
	int n_intBall3_b;

	int n_intBall4_r;
	int n_intBall4_g;
	int n_intBall4_b;

	int n_intBall5_r;
	int n_intBall5_g;
	int n_intBall5_b;

	int GetOurBallColor_R();
	int GetOurBallColor_G();
	int GetOurBallColor_B();

	// Get our prop.
	string_t m_iszPropName; //Get the name of our prop_vecplate!
	CHandle<CBaseAnimating> m_hProp;
};

#endif // VECTRONIC_VECBALL_SYSTEM