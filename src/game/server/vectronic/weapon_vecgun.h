//=========== Copyright © 2013, rHetorical, All rights reserved. =============
//
// Purpose: 
//		
//=============================================================================

#ifndef WEAPON_VECGUN_H
#define WEAPON_VECGUN_H

#ifdef _WIN32
#pragma once
#endif

#include "prop_vectronic_projectile.h"

#include "basevectroniccombatweapon_shared.h"
#include "basecombatcharacter.h"
#include "sprite.h"

#define MAX_SLOT 2
#define	REFIRE_TIME		0.1f
#define DELAY_TIME		50

//-----------------------------------------------------------------------------
// CWeaponVecgun
//-----------------------------------------------------------------------------
class CWeaponVecgun : public CBaseVectronicCombatWeapon
{
	DECLARE_DATADESC();

public:
	DECLARE_CLASS( CWeaponVecgun, CBaseVectronicCombatWeapon );

	CWeaponVecgun(void);

	DECLARE_SERVERCLASS();

	void	Precache( void );
	void	Think( void );
	void	ItemPostFrame( void );
	void	ItemPreFrame( void );
	void	ItemBusyFrame( void );
	void	AddViewKick( void );
	void	DryFire( void );
	void	Shake();
	void	UpdatePenaltyTime( void );

	void	PrimaryAttack( void ); // Fire the selected ball.
	void	SecondaryAttack( void ); // Change the selection
	void	DelayAttack(float fDelay)
	{
		m_nDelay = fDelay;
	};

	void	FireSelectedBall( void ); // Fire the selected ball.
	void	ChangeBall( void );
	void	TestSlot( void );

	//Granting Punts!
	void SetCanFireBlue();
	void SetCanFireGreen();
	void SetCanFireYellow();

	//Clear!
	void ClearGun();
	void ClearBlue();
	void ClearGreen();
	void ClearYellow();

	virtual int	GetMinBurst() 
	{ 
		return 1; 
	}

	virtual int	GetMaxBurst() 
	{ 
		return 3; 
	}

	virtual float GetFireRate( void ) 
	{
		return 0.5f; 
	}

#ifndef SWARM_DLL
	DECLARE_ACTTABLE();
#endif

	CNetworkVar( bool,	m_bCanFireBlue );
	CNetworkVar( bool,	m_bCanFireGreen );
	CNetworkVar( bool,	m_bCanFireYellow );
	CNetworkVar( bool,	m_bUseCustoms );

	bool	CanFireBlue(){return m_bCanFireBlue;};
	bool	CanFireGreen(){return m_bCanFireGreen;};
	bool	CanFireYellow(){return m_bCanFireYellow;};

	void	SetCustomBalls( bool set )
	{
			m_bUseCustoms = set;
	};

	bool	IsUsingCustomBalls(){return m_bUseCustoms;};

	CNetworkVar( int,	m_nDelay );
	CNetworkVar( int,	m_nCurrentSelection );
	CNetworkVar( int,	m_nNumShotsFired );

	int	GetCurrentSelection()
	{
		return m_nCurrentSelection;
	};

	float GetLastAttackTime()
	{
		return m_flLastAttackTime;
	};

	float GetSoonestAttack()
	{
		return m_flSoonestAttack;
	};

	void SetJumpToSelection( int intselect)
	{
		m_nCurrentSelection = intselect;
	};

	void SetBallLife(int balllife)
	{
		m_flBallLife = balllife;
	};

	float m_flBallLife;

	void SetBallSpeed(int ballspeed)
	{
		m_flBallSpeed = ballspeed;
	};

	float m_flBallSpeed;

private:

	float	m_flSoonestAttack;
	float	m_flLastAttackTime;
	float	m_flAccuracyPenalty;
	//int		m_nNumShotsFired;

	int		m_intOnlyBall;

	bool	m_bResetting;

	bool	m_bWasFired;

	//int		m_nCurrentSelection;

	// Here so the modder can make the gun fire custom balls.
	int m_iBall0;
	int m_iBall1;
	int m_iBall2;

	

/*
enum BallSelection
{
	VECTRONIC_BALL_BLUE			= 0,	// We were hit by the blue ball last time.
	VECTRONIC_BALL_GREEN		= 1,	// We were hit by the green ball last time.
	VECTRONIC_BALL_YELLOW		= 2,	// We were hit by the yellow ball last time.
};
*/

};

#endif // WEAPON_VECGUN_H