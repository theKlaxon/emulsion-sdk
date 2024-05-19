//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "basecombatweapon_shared.h"
#include "vectronic_weapon_parse.h"

#ifndef BASEVECTRONICCOMBATWEAPON_SHARED_H
#define BASEVECTRONICCOMBATWEAPON_SHARED_H
#ifdef _WIN32
#pragma once
#endif

#define CROSSHAIR_CONTRACT_PIXELS_PER_SECOND	7.0f

#if defined( CLIENT_DLL )
#define CBaseVectronicCombatWeapon C_BaseVectronicCombatWeapon
#endif

class CBasePlayer;
class CVectronicPlayer;
class CBaseCombatCharacter;

class CBaseVectronicCombatWeapon : public CBaseCombatWeapon
{
#if !defined( CLIENT_DLL )
#ifndef _XBOX
	DECLARE_DATADESC();
#else
protected:
	DECLARE_DATADESC();
private:
#endif
#endif

	DECLARE_CLASS( CBaseVectronicCombatWeapon, CBaseCombatWeapon );
public:
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	CBaseVectronicCombatWeapon();
	~CBaseVectronicCombatWeapon();

	CBasePlayer* GetPlayerOwner() const;
	//CVectronicPlayer*	 GetVectronicPlayerOwner() const;

	virtual bool	WeaponShouldBeLowered( void );

			bool	CanLower();
	virtual bool	Ready( void );
	virtual bool	Lower( void );
	virtual bool	Deploy( void );
	virtual void	DryFire( void );
	virtual bool	Holster( CBaseCombatWeapon *pSwitchingTo );
	virtual void	WeaponIdle( void );
	virtual void	ItemPostFrame();

	virtual void	PrimaryAttack( void );						// do "+ATTACK"
	//virtual void	SecondaryAttack( void ) { return; }			// do "+ATTACK2"

	virtual void	AddViewmodelBob( CBaseViewModel *viewmodel, Vector &origin, QAngle &angles );
	virtual	float	CalcViewmodelBob( void );

	virtual Vector	GetBulletSpread( WeaponProficiency_t proficiency );
	virtual float	GetSpreadBias( WeaponProficiency_t proficiency );

	virtual const	WeaponProficiencyInfo_t *GetProficiencyValues();
	static const	WeaponProficiencyInfo_t *GetDefaultProficiencyValues();

	virtual void	SetPickupTouch( void );
	virtual void	ItemHolsterFrame( void );
	void			SetTouchPickup( bool bForcePickup ) { m_bForcePickup = bForcePickup; }
	//void			WeaponAccuracyPenalty( float flPenAccuracy ){ flPenAccuracy = m_flPenAccuracy; }
	int				m_iPrimaryAttacks;		// # of primary attacks performed with this weapon
	int				m_iSecondaryAttacks;	// # of secondary attacks performed with this weapon

	CVectronicWeaponInfo const	&GetSDKWpnData() const;
	virtual void FireBullets( const FireBulletsInfo_t &info );

	virtual void			Equip( CBaseCombatCharacter *pOwner );

#ifndef SWARM_DLL
	virtual void			OnPickedUp( CBaseCombatCharacter *pNewOwner );
#endif
#if defined( CLIENT_DLL )
	virtual void	DrawCrosshair();
#endif
	float	m_flPrevAnimTime;
	float  m_flNextResetCheckTime;

	bool	m_bDelayFire;			// This variable is used to delay the time between subsequent button pressing.
	float	m_flAccuracy;
	float	m_flDecreaseShotsFired;


	IPhysicsConstraint		*GetConstraint() { return m_pConstraint; }

private:
	WEAPON_FILE_INFO_HANDLE	m_hWeaponFileInfo;
	IPhysicsConstraint		*m_pConstraint;

protected:
	CVectronicWeaponInfo *m_pWeaponInfo;

	bool			m_bLowered;			// Whether the viewmodel is raised or lowered
	float			m_flRaiseTime;		// If lowered, the time we should raise the viewmodel
	float			m_flHolsterTime;	// When the weapon was holstered


	bool			m_bForcePickup;
};

#endif // BASEHLCOMBATWEAPON_SHARED_H
