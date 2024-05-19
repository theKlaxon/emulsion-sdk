//======== Copyright © 2013 - 2014, rHetorical, All rights reserved. ==========
//
// Purpose:
//		
//=============================================================================

#include "cbase.h"
#include <convar.h>
#include "player.h"
#include "gamerules.h"
#include "in_buttons.h"
#include "soundent.h"
#include "game.h"
#include "sprite.h"
#include "particle_parse.h"
#include "vectronic_player.h"
#include "vectronic_gamerules.h"
#include "weapon_vecgun.h"
#include "rumble_shared.h"

//ConVar	vectronic_vecgun_ball_lifetime		( "vectronic_vecgun_ball_lifetime", "1")	;
ConVar	vectronic_vecgun_use_custom_balls	( "vectronic_vecgun_use_custom_balls", "0" );

LINK_ENTITY_TO_CLASS( weapon_vecgun, CWeaponVecgun );
PRECACHE_WEAPON_REGISTER( weapon_vecgun );

IMPLEMENT_SERVERCLASS_ST(CWeaponVecgun, DT_WeaponVecgun)
	SendPropBool( SENDINFO(m_bCanFireBlue) ),
	SendPropBool( SENDINFO(m_bCanFireGreen) ),
	SendPropBool( SENDINFO(m_bCanFireYellow) ),
	SendPropBool( SENDINFO(m_bUseCustoms) ),
	SendPropInt ( SENDINFO( m_nNumShotsFired ), 8, SPROP_UNSIGNED ),
	SendPropInt ( SENDINFO( m_nCurrentSelection ), 8, SPROP_UNSIGNED ),
	SendPropInt ( SENDINFO( m_nDelay ), 8, SPROP_UNSIGNED ),

END_SEND_TABLE()

BEGIN_DATADESC( CWeaponVecgun )

	DEFINE_FIELD( m_flSoonestAttack,		FIELD_TIME ),
	DEFINE_FIELD( m_flLastAttackTime,		FIELD_TIME ),
	DEFINE_FIELD( m_flAccuracyPenalty,		FIELD_FLOAT ), //NOTENOTE: This is NOT tracking game time
	DEFINE_FIELD( m_nNumShotsFired,			FIELD_INTEGER ),
	DEFINE_FIELD( m_nCurrentSelection,		FIELD_INTEGER ),
	DEFINE_FIELD( m_nDelay,					FIELD_INTEGER ),

	DEFINE_FIELD( m_iBall0,					FIELD_INTEGER ),
	DEFINE_FIELD( m_iBall1,					FIELD_INTEGER ),
	DEFINE_FIELD( m_iBall2,					FIELD_INTEGER ),

	DEFINE_FIELD( m_flBallLife,				FIELD_FLOAT),

	// What can we fire?
	DEFINE_FIELD( m_bCanFireBlue, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_bCanFireGreen, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_bCanFireYellow, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_bUseCustoms, FIELD_BOOLEAN ), //<- This is the switch to use 0, 1, 2, or 3, 4, 5.

	// Here so the modder can make the gun fire custom balls.
	DEFINE_KEYFIELD( m_bUseCustoms, FIELD_BOOLEAN, "UseCustoms" ),
	DEFINE_KEYFIELD(m_flBallLife,	FIELD_FLOAT,	"BallLife"	),
	DEFINE_FIELD( m_bWasFired, FIELD_BOOLEAN ),

	DEFINE_FIELD( m_bResetting, FIELD_BOOLEAN ),

	DEFINE_FIELD( m_intOnlyBall,		FIELD_INTEGER ),

END_DATADESC()

#ifndef SWARM_DLL
acttable_t	CWeaponVecgun::m_acttable[] = 
{

	{ ACT_HL2MP_IDLE,                   ACT_HL2MP_IDLE_SHOTGUN,                    false },
    { ACT_HL2MP_RUN,                    ACT_HL2MP_RUN_SHOTGUN,                    false },
    { ACT_HL2MP_IDLE_CROUCH,            ACT_HL2MP_IDLE_CROUCH_SHOTGUN,            false },
    { ACT_HL2MP_WALK_CROUCH,            ACT_HL2MP_WALK_CROUCH_SHOTGUN,            false },
    { ACT_HL2MP_GESTURE_RANGE_ATTACK,   ACT_HL2MP_GESTURE_RANGE_ATTACK_SHOTGUN,    false },
    { ACT_HL2MP_GESTURE_RELOAD,         ACT_HL2MP_GESTURE_RELOAD_SHOTGUN,        false },
    { ACT_HL2MP_JUMP,                   ACT_HL2MP_JUMP_SHOTGUN,                    false },
    { ACT_RANGE_ATTACK1,                ACT_RANGE_ATTACK_SHOTGUN,                false },

};
IMPLEMENT_ACTTABLE( CWeaponVecgun );
#endif

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CWeaponVecgun::CWeaponVecgun( void )
{
	m_flSoonestAttack = gpGlobals->curtime;
	m_flAccuracyPenalty = 0.0f;

	m_fMinRange1		= 24;
	m_fMaxRange1		= 1500;
	m_fMinRange2		= 24;
	m_fMaxRange2		= 200;

	m_bFiresUnderwater	= true;

	//Ok, atleast for now, the puntgun can not fire anything.
	m_bCanFireBlue = false;
	m_bCanFireGreen = false;
	m_bCanFireYellow = false;

	m_bWasFired = false;

	// TEMP: Switch to Customs. Find a better way please!
	if (vectronic_vecgun_use_custom_balls.GetBool())
	{
		m_bUseCustoms = true;
	}

	//SetNextThink( gpGlobals->curtime );
	m_nDelay = DELAY_TIME;
	AddEffects( EF_NODRAW );

	// Unless the user changed these, keep them the same.
	m_iBall0 = ( ( m_bUseCustoms ) ? ( 3 ) : ( 0 ) );
	m_iBall1 = ( ( m_bUseCustoms ) ? ( 4 ) : ( 1 ) );
	m_iBall2 = ( ( m_bUseCustoms ) ? ( 5 ) : ( 2 ) );

	//m_nCurrentSelection = VECTRONIC_BALL_BLUE;
	m_nCurrentSelection = m_iBall0;



	m_flBallLife = rHGameRules()->GetBallLife();
	m_flBallSpeed = rHGameRules()->GetBallSpeed();

}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponVecgun::Precache( void )
{
	BaseClass::Precache();

	UTIL_PrecacheOther( "prop_particle_ball" );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponVecgun::Think( void )
{

}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CWeaponVecgun::DryFire( void )
{
	WeaponSound( EMPTY );

	m_flSoonestAttack			= gpGlobals->curtime + 0.1f;
	m_flNextPrimaryAttack		= gpGlobals->curtime + SequenceDuration();
	m_flNextSecondaryAttack		= gpGlobals->curtime + SequenceDuration();

}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponVecgun::ChangeBall( void )
{
	if(m_intOnlyBall >= 0)
	{
		DevMsg("CWeaponVecgun: Only have one ball, not changing slots.\n");
		return;
	}

	DevMsg("CWeaponVecgun: Changing selected ball!\n");

	// If we are at the max slot, go to 0.
	//if (m_nCurrentSelection == MAX_SLOT + 1)
	if (m_nCurrentSelection == m_iBall2 + 1)
	{
		DevMsg("CWeaponVecgun: Restarted slot cycle.\n");

		if ( CanFireBlue() )
		{
			//m_nCurrentSelection = VECTRONIC_BALL_BLUE;
			m_nCurrentSelection = m_iBall0;
		}
		else if ( CanFireGreen() )
		{
			//m_nCurrentSelection = VECTRONIC_BALL_GREEN;
			m_nCurrentSelection = m_iBall1;
		}
		else if ( CanFireYellow() )
		{
			//m_nCurrentSelection = VECTRONIC_BALL_YELLOW;
			m_nCurrentSelection = m_iBall2;
		}
	}
	else
	{
		// Go to the next slot.
		m_nCurrentSelection++;
	}

	WeaponSound( WPN_DOUBLE );

	CBasePlayer *pOwner = ToBasePlayer( GetOwner() );
	if( pOwner )
	{
		// RUMBLE
		pOwner->RumbleEffect( RUMBLE_PISTOL, 0, RUMBLE_FLAG_RESTART );
	}

	return;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponVecgun::TestSlot( void )
{
	// Test what ball is the only ball.
	if (CanFireBlue() && !CanFireGreen() && !CanFireYellow() )
	{
		//m_intOnlyBall = VECTRONIC_BALL_BLUE;
		m_intOnlyBall = m_iBall0;
	}
	else if ( !CanFireBlue() && CanFireGreen() && !CanFireYellow() )
	{
		//m_intOnlyBall = VECTRONIC_BALL_GREEN;
		m_intOnlyBall = m_iBall1;
	}
	else if ( !CanFireBlue() && !CanFireGreen() && CanFireYellow() )
	{
		//m_intOnlyBall = VECTRONIC_BALL_YELLOW;
		m_intOnlyBall = m_iBall2;
	}
	else
	{
		m_intOnlyBall = -1;
	}

	// Slot test again.
	//if (m_nCurrentSelection == MAX_SLOT + 1)
	if (m_nCurrentSelection == m_iBall2 + 1)
	{
		DevMsg("CWeaponVecgun: Restarted slot cycle.\n");
		if ( CanFireBlue() )
		{
			//m_nCurrentSelection = 0;
			m_nCurrentSelection = m_iBall0;
		}
		else if ( CanFireGreen() )
		{
			//m_nCurrentSelection = 1;
			m_nCurrentSelection = m_iBall1;
		}
		else if ( CanFireYellow() )
		{
			//m_nCurrentSelection = 2;
			m_nCurrentSelection = m_iBall2;
		}
		return;
	}

	// Here if we are on a slot that we don't have, jump to the next slot.
	//if (m_nCurrentSelection == VECTRONIC_BALL_BLUE && CanFireBlue() == false )
	if (m_nCurrentSelection == m_iBall0 && CanFireBlue() == false )
	{
		// Go to the next slot.
		DevMsg("CWeaponVecgun: Skipping to next slot.\n");
		m_nCurrentSelection++;
	}

	if (m_nCurrentSelection == m_iBall1 && CanFireGreen() == false)
	{
		// Go to the next slot.
		DevMsg("CWeaponVecgun: Skipping to next slot.\n");
		m_nCurrentSelection++;
	}

	if (m_nCurrentSelection == m_iBall2 && CanFireYellow() == false )
	{
		// Go to the next slot.
		DevMsg("CWeaponVecgun: Skipping to next slot.\n");
		m_nCurrentSelection++;
	}

	return;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponVecgun::FireSelectedBall( void )
{
	CBasePlayer *pOwner = ToBasePlayer( GetOwner() );
	
	if ( pOwner == NULL )
		return;

	DevMsg("CWeaponVecgun: Firing selected ball!\n");

	// Fire the bullets
	Vector vecSrc	 = pOwner->Weapon_ShootPosition( );
	Vector vecAiming = pOwner->GetAutoaimVector( AUTOAIM_SCALE_DEFAULT );
	Vector impactPoint = vecSrc + ( vecAiming * MAX_TRACE_LENGTH );

	// Fire the bullets
	Vector vecVelocity = vecAiming * m_flBallSpeed;

	// Fire the combine ball
	CreateParticleBall0(vecSrc, vecVelocity, 0.1f, m_nCurrentSelection, m_flBallLife, pOwner);

	SendWeaponAnim( ACT_VM_PRIMARYATTACK );

	// RUMBLE
	pOwner->RumbleEffect( RUMBLE_SHOTGUN_SINGLE, 0, RUMBLE_FLAG_RESTART );

	WeaponSound( SINGLE );

	m_nNumShotsFired++;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CWeaponVecgun::PrimaryAttack( void )
{
	if ((gpGlobals->curtime - m_flLastAttackTime) > REFIRE_TIME && m_nDelay >= DELAY_TIME)
	{
		// Do we have balls at all?
		if (CanFireBlue() || CanFireGreen() || CanFireYellow() )
		{
			m_nNumShotsFired = 0;
			m_nDelay = 0;
			m_bWasFired = true;
			FireSelectedBall();

		}
		else
		{
			DevMsg("CWeaponVecgun: No ball to fire.\n");
			return;
		}
	}

	m_flLastAttackTime = gpGlobals->curtime;
	m_flSoonestAttack = gpGlobals->curtime + REFIRE_TIME;

	CBasePlayer *pOwner = ToBasePlayer( GetOwner() );
	if( pOwner )
	{
		pOwner->ViewPunchReset();
	}
}
//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CWeaponVecgun::SecondaryAttack( void )
{
	if ( ( gpGlobals->curtime - m_flLastAttackTime ) > 0.2f )
	{
		// Do we have balls at all?
		if (CanFireBlue() || CanFireGreen() || CanFireYellow() )
		{
			m_nNumShotsFired = 0;
			ChangeBall();

		}
	}
	else
	{
		m_nNumShotsFired++;
	}


	m_flLastAttackTime = gpGlobals->curtime;
	m_flSoonestAttack = gpGlobals->curtime + 0.1f;

	CBasePlayer *pOwner = ToBasePlayer( GetOwner() );
	if( pOwner )
	{
		pOwner->ViewPunchReset();
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponVecgun::UpdatePenaltyTime( void )
{
	CBasePlayer *pOwner = ToBasePlayer( GetOwner() );

	if ( pOwner == NULL )
		return;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponVecgun::ItemPreFrame( void )
{
	UpdatePenaltyTime();

	BaseClass::ItemPreFrame();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponVecgun::ItemBusyFrame( void )
{
	UpdatePenaltyTime();

	// HACK: Set the delay to 100. We need to wait anyway after the player picks up an object.
	m_nDelay = 100;
	BaseClass::ItemBusyFrame();
}

//-----------------------------------------------------------------------------
// Purpose: Allows firing as fast as button is pressed
//-----------------------------------------------------------------------------
void CWeaponVecgun::ItemPostFrame( void )
{
	BaseClass::ItemPostFrame();

	CVectronicPlayer *pOwner = To_VectronicPlayer( GetOwner() );

	if ( pOwner == NULL )
		return;

	m_bUseCustoms = vectronic_vecgun_use_custom_balls.GetBool();

	if (IsUsingCustomBalls())
	{
		if( m_iBall0 == 0 || m_iBall1 == 1 || m_iBall0 == 2)
		{
			ClearGun();
			m_iBall0 = 3;
			m_iBall1 = 4;
			m_iBall2 = 5;
		}
	}
	else
	{
		if( m_iBall0 == 3 || m_iBall1 == 4 || m_iBall0 == 5)
		{
			ClearGun();
			m_iBall0 = 0;
			m_iBall1 = 1;
			m_iBall2 = 2;
		}
	}

	// Do we have balls at all?
	if (CanFireBlue() || CanFireGreen() || CanFireYellow() )
	{
		TestSlot();
	}

	if ( m_bInReload )
		return;
	
	if(m_bResetting)
	{
		m_nDelay++;

		if (m_nDelay >= DELAY_TIME)
		{
			m_bResetting = false;
		}
	}

	if (m_bWasFired)
	{
		if (m_nDelay < DELAY_TIME)
		{
			m_bResetting = true;
		}
	}

	//Allow a refire as fast as the player can click
	if ( ( ( pOwner->m_nButtons & IN_ATTACK ) == false ) && ( m_flSoonestAttack < gpGlobals->curtime ) || ( ( pOwner->m_nButtons & IN_ATTACK ) == true ) && m_bWasFired )
	{
		m_flNextPrimaryAttack = gpGlobals->curtime - REFIRE_TIME;
		if (m_nDelay < DELAY_TIME)
		{
			//DevMsg("CWeaponVecgun: Registering ATTACK1\n");
			m_bResetting = true;
			m_bWasFired = false;
		}
	}

	//Allow a refire as fast as the player can click
	if ( ( ( pOwner->m_nButtons & IN_ATTACK2 ) == false ) && ( m_flSoonestAttack < gpGlobals->curtime ) )
	{
		m_flNextSecondaryAttack = gpGlobals->curtime - REFIRE_TIME;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponVecgun::AddViewKick( void )
{
	CBasePlayer *pPlayer  = ToBasePlayer( GetOwner() );
	
	if ( pPlayer == NULL )
		return;

	QAngle	viewPunch;

	viewPunch.x = random->RandomFloat( 0.25f, 0.5f );
	viewPunch.y = random->RandomFloat( -.6f, .6f );
	viewPunch.z = 0.0f;

	//Add it to the view punch
	pPlayer->ViewPunch( viewPunch );
}

// Our Ball Code

// Granting Punts
void CWeaponVecgun::SetCanFireBlue()
{
	CBasePlayer *pOwner = ToBasePlayer( GetOwner() );

	if ( pOwner == NULL )
		return;

	// Can we already fire this Punt?
	if (!m_bCanFireBlue)
	{
		DevMsg("CWeaponVecgun: Blue\n");

		// RUMBLE
		pOwner->RumbleEffect( RUMBLE_AR2, 0, RUMBLE_FLAG_RESTART );
		WeaponSound( RELOAD );
		m_bCanFireBlue = true;

		//m_nCurrentSelection = VECTRONIC_BALL_BLUE;
		 m_nCurrentSelection = m_iBall0;
	}
}

void CWeaponVecgun::SetCanFireGreen()
{
	CBasePlayer *pOwner = ToBasePlayer( GetOwner() );

	if ( pOwner == NULL )
		return;

	// Can we already fire this Punt?
	if (!m_bCanFireGreen)
	{
		DevMsg("CWeaponVecgun: Green\n");

		// RUMBLE
		pOwner->RumbleEffect( RUMBLE_AR2, 0, RUMBLE_FLAG_RESTART );
		WeaponSound( RELOAD );
		m_bCanFireGreen = true;

		//m_nCurrentSelection = VECTRONIC_BALL_GREEN;
		 m_nCurrentSelection = m_iBall1;
	}
}

void CWeaponVecgun::SetCanFireYellow()
{
	CBasePlayer *pOwner = ToBasePlayer( GetOwner() );

	if ( pOwner == NULL )
		return;

	// Can we already fire this Punt?
	if (!m_bCanFireYellow)
	{
		DevMsg("CWeaponVecgun: Purple\n");

		// RUMBLE
		pOwner->RumbleEffect( RUMBLE_AR2, 0, RUMBLE_FLAG_RESTART );
		WeaponSound( RELOAD );
		m_bCanFireYellow = true;

	    //m_nCurrentSelection = VECTRONIC_BALL_YELLOW;
		 m_nCurrentSelection = m_iBall2;
	}
}

// Clear the gun
void CWeaponVecgun::ClearGun()
{
	CBasePlayer *pOwner = ToBasePlayer( GetOwner() );
	
	if ( pOwner == NULL )
		return;

	// Test to see if we have any balls. If so, we play a cute
	// animation and sound.
	if ( m_bCanFireBlue || m_bCanFireGreen || m_bCanFireYellow )
	{
		//Play animation
		SendWeaponAnim( ACT_VM_FIZZLE );

		//Play Sound
		WeaponSound( TAUNT );

		// RUMBLE
		pOwner->RumbleEffect( RUMBLE_SHOTGUN_DOUBLE, 0, RUMBLE_FLAG_ONLYONE );

		// Remove Punts from gun.
		if (m_bCanFireBlue)
		{
			m_bCanFireBlue = false;
		}

		if (m_bCanFireGreen)
		{
			m_bCanFireGreen = false;
		}

		if (m_bCanFireYellow)
		{
			m_bCanFireYellow = false;
		}
	}
}

void CWeaponVecgun::ClearBlue()
{
	CBasePlayer *pOwner = ToBasePlayer( GetOwner() );
	
	if ( pOwner == NULL )
		return;

	// Test to see if we have any balls. If so, we play a cute
	// animation and sound.
	if ( m_bCanFireBlue)
	{
		//Play animation
		SendWeaponAnim( ACT_VM_FIZZLE );

		// RUMBLE
		pOwner->RumbleEffect( RUMBLE_SHOTGUN_DOUBLE, 0, RUMBLE_FLAG_ONLYONE );

		//Play Sound
		WeaponSound( TAUNT );

		m_bCanFireBlue = false;
	}
}

void CWeaponVecgun::ClearGreen()
{
	CBasePlayer *pOwner = ToBasePlayer( GetOwner() );
	
	if ( pOwner == NULL )
		return;

	// Test to see if we have any balls. If so, we play a cute
	// animation and sound.
	if ( m_bCanFireGreen)
	{
		//Play animation
		SendWeaponAnim( ACT_VM_FIZZLE );

		//Play Sound
		WeaponSound( TAUNT );

		// RUMBLE
		pOwner->RumbleEffect( RUMBLE_SHOTGUN_DOUBLE, 0, RUMBLE_FLAG_ONLYONE );

		m_bCanFireGreen = false;
	}
}

void CWeaponVecgun::ClearYellow()
{
	CBasePlayer *pOwner = ToBasePlayer( GetOwner() );
	
	if ( pOwner == NULL )
		return;

	// Test to see if we have any balls. If so, we play a cute
	// animation and sound.
	if ( m_bCanFireYellow)
	{
		//Play animation
		SendWeaponAnim( ACT_VM_FIZZLE );

		//Play Sound
		WeaponSound( TAUNT );

		// RUMBLE
		pOwner->RumbleEffect( RUMBLE_SHOTGUN_DOUBLE, 0, RUMBLE_FLAG_ONLYONE );

		m_bCanFireYellow = false;
	}
}

void CC_JumpToBlue( void )
{
	CVectronicPlayer *pPlayer = To_VectronicPlayer( UTIL_GetCommandClient() );

	CWeaponVecgun *pVecGun = static_cast<CWeaponVecgun*>( pPlayer->Weapon_OwnsThisType( "weapon_vecgun" ) );
	if ( pVecGun && pVecGun->CanFireBlue() )
	{
		pVecGun->WeaponSound( WPN_DOUBLE );
		if (pVecGun->IsUsingCustomBalls() == false )
		{
			pVecGun->SetJumpToSelection(0);
		}
		else
		{
			pVecGun->SetJumpToSelection(3);
		}
	}
}

static ConCommand jumpto_blue("jumpto_blue", CC_JumpToBlue, "Makes the guns current selection be blue.\n\tArguments:   	none ");

void CC_JumpToGreen( void )
{
	CVectronicPlayer *pPlayer = To_VectronicPlayer( UTIL_GetCommandClient() );

	CWeaponVecgun *pVecGun = static_cast<CWeaponVecgun*>( pPlayer->Weapon_OwnsThisType( "weapon_vecgun" ) );
	if ( pVecGun && pVecGun->CanFireGreen() )
	{
		pVecGun->WeaponSound( WPN_DOUBLE );
		if (pVecGun->IsUsingCustomBalls() == false )
		{
			pVecGun->SetJumpToSelection(1);
		}
		else
		{
			pVecGun->SetJumpToSelection(4);
		}
	}
}

static ConCommand jumpto_green("jumpto_green", CC_JumpToGreen, "Makes the guns current selection be green.\n\tArguments:   	none ");

void CC_JumpToPurple( void )
{
	CVectronicPlayer *pPlayer = To_VectronicPlayer( UTIL_GetCommandClient() );

	CWeaponVecgun *pVecGun = static_cast<CWeaponVecgun*>( pPlayer->Weapon_OwnsThisType( "weapon_vecgun" ) );
	if ( pVecGun && pVecGun->CanFireYellow() )
	{
		pVecGun->WeaponSound( WPN_DOUBLE );
		if (pVecGun->IsUsingCustomBalls() == false )
		{
			pVecGun->SetJumpToSelection(2);
		}
		else
		{
			pVecGun->SetJumpToSelection(5);
		}
	}
}

static ConCommand jumpto_purple("jumpto_purple", CC_JumpToPurple, "Makes the guns current selection be purple.\n\tArguments:   	none ");

void CC_JumpToNext( void )
{
	CVectronicPlayer *pPlayer = To_VectronicPlayer( UTIL_GetCommandClient() );

	CWeaponVecgun *pVecGun = static_cast<CWeaponVecgun*>( pPlayer->Weapon_OwnsThisType( "weapon_vecgun" ) );
	if ((gpGlobals->curtime - pVecGun->GetLastAttackTime() > 0.2f))
	{
		// Do we have balls at all?
		if (pVecGun->CanFireBlue() || pVecGun->CanFireGreen() || pVecGun->CanFireYellow())
		{
			pVecGun->m_nNumShotsFired = 0;
			pVecGun->ChangeBall();

		}
	}
	else
	{
		pVecGun->m_nNumShotsFired++;
	}

	float lastattack = pVecGun->GetLastAttackTime();
	float soonestattack = pVecGun->GetSoonestAttack();

	lastattack = gpGlobals->curtime;
	soonestattack = gpGlobals->curtime + 0.1f;

	if (pPlayer)
	{
		pPlayer->ViewPunchReset();
	}

	//--
	/*
	if (pVecGun && pVecGun->CanFireYellow())
	{
		pVecGun->WeaponSound( WPN_DOUBLE );
		pVecGun->m_nCurrentSelection++;
	}
	*/
}

static ConCommand jumpto_nextball("jumpto_nextball", CC_JumpToNext, "Makes the gun jump to the next ball slot.\n\tArguments:   	none ");