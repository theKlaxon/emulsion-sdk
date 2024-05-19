//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "basevectroniccombatweapon_shared.h"
#include "basecombatweapon_shared.h"
#include "in_buttons.h"
#include "vectronic_player_shared.h"
#include "vectronic_weapon_parse.h"
#include "physics_saverestore.h"

#if defined( CLIENT_DLL )

	#include "c_baseentity.h"
	#include "vgui/ISurface.h"
	#include "vgui_controls/Controls.h"
	//#include "c_sdk_player_sp.h"
	#include "hud_crosshair.h"

#else
	#include "baseentity.h"
	//#include "sdk_player_sp.h"
	#include "vphysics/constraints.h"

#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LINK_ENTITY_TO_CLASS( basevectroniccombatweapon, CBaseVectronicCombatWeapon );

IMPLEMENT_NETWORKCLASS_ALIASED( BaseVectronicCombatWeapon , DT_BaseVectronicCombatWeapon )

BEGIN_NETWORK_TABLE( CBaseVectronicCombatWeapon , DT_BaseVectronicCombatWeapon )
#if !defined( CLIENT_DLL )
//	SendPropInt( SENDINFO( m_bReflectViewModelAnimations ), 1, SPROP_UNSIGNED ),
#else
//	RecvPropInt( RECVINFO( m_bReflectViewModelAnimations ) ),
#endif
END_NETWORK_TABLE()

#if !defined( CLIENT_DLL )

#include "globalstate.h"

//---------------------------------------------------------
// Save/Restore
//---------------------------------------------------------
BEGIN_DATADESC( CBaseVectronicCombatWeapon )

	DEFINE_PHYSPTR( m_pConstraint ),

	DEFINE_FIELD( m_bLowered,			FIELD_BOOLEAN ),
	DEFINE_FIELD( m_flRaiseTime,		FIELD_TIME ),
	DEFINE_FIELD( m_flHolsterTime,		FIELD_TIME ),
	DEFINE_FIELD( m_iPrimaryAttacks,	FIELD_INTEGER ),
	DEFINE_FIELD( m_iSecondaryAttacks,	FIELD_INTEGER ),


END_DATADESC()

#endif

BEGIN_PREDICTION_DATA( CBaseVectronicCombatWeapon )
END_PREDICTION_DATA()

ConVar sk_auto_reload_time( "sk_auto_reload_time", "3", FCVAR_REPLICATED );

CBaseVectronicCombatWeapon::CBaseVectronicCombatWeapon()
{
#if !defined( CLIENT_DLL )
	m_pConstraint = NULL;
	OnBaseCombatWeaponCreated( this );
#endif

	m_bForcePickup = true;
	m_bDelayFire = true;
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CBaseVectronicCombatWeapon::~CBaseVectronicCombatWeapon( void )
{
#if !defined( CLIENT_DLL )
	//Remove our constraint, if we have one
	if ( m_pConstraint != NULL )
	{
		physenv->DestroyConstraint( m_pConstraint );
		m_pConstraint = NULL;
	}
	OnBaseCombatWeaponDestroyed( this );
#endif
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CBaseVectronicCombatWeapon::SetPickupTouch( void )
{
#if !defined( CLIENT_DLL )

	if (!m_bForcePickup )
	{	
		return;
	}

	SetTouch(&CBaseCombatWeapon::DefaultTouch);

	if ( gpGlobals->maxClients > 1 )
	{
		if ( GetSpawnFlags() & SF_NORESPAWN )
		{
			SetThink( &CBaseEntity::SUB_Remove );
			SetNextThink( gpGlobals->curtime + 30.0f );
		}
	}

	BaseClass::SetPickupTouch();
#endif
}

//-----------------------------------------------------------------------------
// Purpose: Become a child of the owner (MOVETYPE_FOLLOW)
//			disables collisions, touch functions, thinking
// Input  : *pOwner - new owner/operator
//-----------------------------------------------------------------------------
void CBaseVectronicCombatWeapon::Equip( CBaseCombatCharacter *pOwner )
{

	// Attach the weapon to an owner
	SetAbsVelocity( vec3_origin );
	RemoveSolidFlags( FSOLID_TRIGGER );
	FollowEntity( pOwner );
	SetOwner( pOwner );
	SetOwnerEntity( pOwner );

	// Break any constraint I might have to the world.
	RemoveEffects( EF_ITEM_BLINK );

#if !defined( CLIENT_DLL )

	if ( m_pConstraint != NULL )
	{
		RemoveSpawnFlags( SF_WEAPON_START_CONSTRAINED );
		physenv->DestroyConstraint( m_pConstraint );
		m_pConstraint = NULL;
	}
#endif


	m_flNextPrimaryAttack		= gpGlobals->curtime;
	m_flNextSecondaryAttack		= gpGlobals->curtime;
	SetTouch( NULL );
	SetThink( NULL );
#if !defined( CLIENT_DLL )
	VPhysicsDestroyObject();
#endif

	if ( pOwner->IsPlayer() )
	{
		SetModel( GetViewModel() );
	}
	else
	{
		// Make the weapon ready as soon as any NPC picks it up.
		m_flNextPrimaryAttack = gpGlobals->curtime;
		m_flNextSecondaryAttack = gpGlobals->curtime;
		SetModel( GetWorldModel() );
	}

}

#ifndef SWARM_DLL // Use base instead.
//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pPicker - 
//-----------------------------------------------------------------------------
void CBaseVectronicCombatWeapon::OnPickedUp( CBaseCombatCharacter *pNewOwner )
{
#if !defined( CLIENT_DLL )
	RemoveEffects( EF_ITEM_BLINK );

	if( pNewOwner->IsPlayer() )
	{
		m_OnPlayerPickup.FireOutput(pNewOwner, this);

		// Play the pickup sound for 1st-person observers
		CRecipientFilter filter;
		for ( int i=1; i <= gpGlobals->maxClients; ++i )
		{
			CBasePlayer *player = UTIL_PlayerByIndex(i);
			if ( player && !player->IsAlive() && player->GetObserverMode() == OBS_MODE_IN_EYE )
			{
				filter.AddRecipient( player );
			}
		}
		if ( filter.GetRecipientCount() )
		{

#ifndef WEAPON_QUIET_PICKUP
			// Quiet the pickup sound for Vectronic.
			CBaseEntity::EmitSound( filter, pNewOwner->entindex(), "Player.PickupWeapon" );
#endif
		}

		// Robin: We don't want to delete weapons the player has picked up, so 
		// clear the name of the weapon. This prevents wildcards that are meant 
		// to find NPCs finding weapons dropped by the NPCs as well.
		SetName( NULL_STRING );
	}
	else
	{
		m_OnNPCPickup.FireOutput(pNewOwner, this);
	}

#ifdef HL2MP
	HL2MPRules()->RemoveLevelDesignerPlacedObject( this );
#endif

	// Someone picked me up, so make it so that I can't be removed.
	SetRemoveable( false );
#endif
}
#endif

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CBaseVectronicCombatWeapon::ItemHolsterFrame( void )
{
	BaseClass::ItemHolsterFrame();

	// Must be player held
	if ( GetOwner() && GetOwner()->IsPlayer() == false )
		return;

	// We can't be active
	if ( GetOwner()->GetActiveWeapon() == this )
		return;

	// If it's been longer than three seconds, reload
	if ( ( gpGlobals->curtime - m_flHolsterTime ) > sk_auto_reload_time.GetFloat() )
	{
		// Just load the clip with no animations
		FinishReload();
		m_flHolsterTime = gpGlobals->curtime;
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CBaseVectronicCombatWeapon::CanLower()
{
	if ( SelectWeightedSequence( ACT_VM_IDLE_LOWERED ) == ACTIVITY_NOT_AVAILABLE )
		return false;
	return true;
}

//-----------------------------------------------------------------------------
// Purpose: Drops the weapon into a lowered pose
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CBaseVectronicCombatWeapon::Lower( void )
{
	//Don't bother if we don't have the animation
	if ( SelectWeightedSequence( ACT_VM_IDLE_LOWERED ) == ACTIVITY_NOT_AVAILABLE )
		return false;

	m_bLowered = true;
	return true;
}

//-----------------------------------------------------------------------------
// Purpose: Brings the weapon up to the ready position
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CBaseVectronicCombatWeapon::Ready( void )
{
	//Don't bother if we don't have the animation
	if ( SelectWeightedSequence( ACT_VM_LOWERED_TO_IDLE ) == ACTIVITY_NOT_AVAILABLE )
		return false;

	m_bLowered = false;	
	m_flRaiseTime = gpGlobals->curtime + 0.5f;
	return true;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CBaseVectronicCombatWeapon::Deploy( void )
{

#ifndef CLIENT_DLL
	m_flDecreaseShotsFired = gpGlobals->curtime;

	CBasePlayer *pFakePlayer = GetPlayerOwner();
	CVectronicPlayer *pPlayer = To_VectronicPlayer(pFakePlayer);

	if ( pPlayer )
	{
		pPlayer->m_iShotsFired = 0;
		m_bDelayFire = false;
	}
#endif

	m_bLowered = false;

	return BaseClass::Deploy();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CBaseVectronicCombatWeapon::DryFire( void )
{
	WeaponSound( EMPTY );
	SendWeaponAnim( ACT_VM_DRYFIRE );

	m_bDelayFire = false;
}

//-----------------------------------------------------------------------------
// Purpose: Secondary fire button attack
//-----------------------------------------------------------------------------
void CBaseVectronicCombatWeapon::PrimaryAttack( void )
{
	BaseClass::PrimaryAttack();

	CBasePlayer *pFakePlayer = GetPlayerOwner();
	CVectronicPlayer *pPlayer = To_VectronicPlayer(pFakePlayer);

	pPlayer->m_iShotsFired++;
	m_bDelayFire = true;

}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CBaseVectronicCombatWeapon::ItemPostFrame()
{
	CBasePlayer *pFakePlayer = GetPlayerOwner();
	CVectronicPlayer *pPlayer = To_VectronicPlayer(pFakePlayer);

	if ( !(pPlayer->m_nButtons & (IN_ATTACK|IN_ATTACK2) ) )
	{
		if ( m_bDelayFire )
		{
			m_bDelayFire = false;

			if (pPlayer->m_iShotsFired > 15)
				pPlayer->m_iShotsFired = 15;
			
			m_flDecreaseShotsFired = gpGlobals->curtime + 0.4;
		}

		if ( (pPlayer->m_iShotsFired > 0) && (m_flDecreaseShotsFired < gpGlobals->curtime)	)
		{
			m_flDecreaseShotsFired = gpGlobals->curtime + 0.0225;
			pPlayer->m_iShotsFired--;
		}
	}

	BaseClass::ItemPostFrame();
}

//-----------------------------------------------------------------------------
// Purpose: Get my data in the file weapon info array
//-----------------------------------------------------------------------------
const CVectronicWeaponInfo &CBaseVectronicCombatWeapon::GetSDKWpnData( void ) const
{
	const FileWeaponInfo_t *pWeaponInfo = &GetWpnData();
	const CVectronicWeaponInfo *pSDKInfo;

	#ifdef _DEBUG
		pSDKInfo = dynamic_cast< const CVectronicWeaponInfo* >( pWeaponInfo );
		Assert( pSDKInfo );
	#else
		pSDKInfo = static_cast< const CVectronicWeaponInfo* >( pWeaponInfo );
	#endif

	return *pSDKInfo;

//	return *GetFileWeaponInfoFromHandle( m_hWeaponFileInfo );
}

CBasePlayer* CBaseVectronicCombatWeapon::GetPlayerOwner() const
{
	return dynamic_cast< CBasePlayer* >( GetOwner() );
}

//For some reason, ASW has a problem calling our WeaponData
void CBaseVectronicCombatWeapon::FireBullets( const FireBulletsInfo_t &info )
{
	FireBulletsInfo_t modinfo = info;

//	modinfo.m_iPlayerDamage = GetSDKWpnData().m_iPlayerDamage;

	BaseClass::FireBullets( modinfo );
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CBaseVectronicCombatWeapon::Holster( CBaseCombatWeapon *pSwitchingTo )
{
	if ( BaseClass::Holster( pSwitchingTo ) )
	{
		m_flHolsterTime = gpGlobals->curtime;
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CBaseVectronicCombatWeapon::WeaponShouldBeLowered( void )
{
	// Can't be in the middle of another animation
  	if ( GetIdealActivity() != ACT_VM_IDLE_LOWERED && GetIdealActivity() != ACT_VM_IDLE &&
		 GetIdealActivity() != ACT_VM_IDLE_TO_LOWERED && GetIdealActivity() != ACT_VM_LOWERED_TO_IDLE )
  		return false;

	if ( m_bLowered )
		return true;
	
#if !defined( CLIENT_DLL )

	if ( GlobalEntity_GetState( "friendly_encounter" ) == GLOBAL_ON )
		return true;

#endif

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: Allows the weapon to choose proper weapon idle animation
//-----------------------------------------------------------------------------
void CBaseVectronicCombatWeapon::WeaponIdle( void )
{
	//See if we should idle high or low
	if ( WeaponShouldBeLowered() )
	{
#if !defined( CLIENT_DLL )
		CVectronicPlayer *pPlayer = dynamic_cast<CVectronicPlayer*>(GetOwner());

		if( pPlayer )
		{
		//	pPlayer->Weapon_Lower();
		}
#endif

		// Move to lowered position if we're not there yet
		if ( GetActivity() != ACT_VM_IDLE_LOWERED && GetActivity() != ACT_VM_IDLE_TO_LOWERED 
			 && GetActivity() != ACT_TRANSITION )
		{
			SendWeaponAnim( ACT_VM_IDLE_LOWERED );
		}
		else if ( HasWeaponIdleTimeElapsed() )
		{
			// Keep idling low
			SendWeaponAnim( ACT_VM_IDLE_LOWERED );
		}
	}
	else
	{
		// See if we need to raise immediately
		if ( m_flRaiseTime < gpGlobals->curtime && GetActivity() == ACT_VM_IDLE_LOWERED ) 
		{
			SendWeaponAnim( ACT_VM_IDLE );
		}
		else if ( HasWeaponIdleTimeElapsed() ) 
		{
			SendWeaponAnim( ACT_VM_IDLE );
		}
	}
}

float	g_lateralBob;
float	g_verticalBob;

#if defined( CLIENT_DLL ) && ( !defined( HL2MP ) && !defined( PORTAL ) )

#define	HL2_BOB_CYCLE_MIN	1.0f
#define	HL2_BOB_CYCLE_MAX	0.45f
#define	HL2_BOB			0.002f
#define	HL2_BOB_UP		0.5f


static ConVar	cl_bobcycle( "cl_bobcycle","0.8" );
static ConVar	cl_bob( "cl_bob","0.002" );
static ConVar	cl_bobup( "cl_bobup","0.5" );

// Register these cvars if needed for easy tweaking
static ConVar	v_iyaw_cycle( "v_iyaw_cycle", "2"/*, FCVAR_UNREGISTERED*/ );
static ConVar	v_iroll_cycle( "v_iroll_cycle", "0.5"/*, FCVAR_UNREGISTERED*/ );
static ConVar	v_ipitch_cycle( "v_ipitch_cycle", "1"/*, FCVAR_UNREGISTERED*/ );
static ConVar	v_iyaw_level( "v_iyaw_level", "0.3"/*, FCVAR_UNREGISTERED*/ );
static ConVar	v_iroll_level( "v_iroll_level", "0.1"/*, FCVAR_UNREGISTERED*/ );
static ConVar	v_ipitch_level( "v_ipitch_level", "0.3"/*, FCVAR_UNREGISTERED*/ );

//-----------------------------------------------------------------------------
// Purpose: 
// Output : float
//-----------------------------------------------------------------------------
float CBaseVectronicCombatWeapon::CalcViewmodelBob( void )
{
	static	float bobtime;
	static	float lastbobtime;
	float	cycle;
	
	CBasePlayer *player = ToBasePlayer( GetOwner() );
	//Assert( player );

	//NOTENOTE: For now, let this cycle continue when in the air, because it snaps badly without it

	if ( ( !gpGlobals->frametime ) || ( player == NULL ) )
	{
		//NOTENOTE: We don't use this return value in our case (need to restructure the calculation function setup!)
		return 0.0f;// just use old value
	}

	//Find the speed of the player
	float speed = player->GetLocalVelocity().Length2D();

	//FIXME: This maximum speed value must come from the server.
	//		 MaxSpeed() is not sufficient for dealing with sprinting - jdw

	speed = clamp( speed, -320, 320 );

	float bob_offset = RemapVal( speed, 0, 320, 0.0f, 1.0f );
	
	bobtime += ( gpGlobals->curtime - lastbobtime ) * bob_offset;
	lastbobtime = gpGlobals->curtime;

	//Calculate the vertical bob
	cycle = bobtime - (int)(bobtime/HL2_BOB_CYCLE_MAX)*HL2_BOB_CYCLE_MAX;
	cycle /= HL2_BOB_CYCLE_MAX;

	if ( cycle < HL2_BOB_UP )
	{
		cycle = M_PI * cycle / HL2_BOB_UP;
	}
	else
	{
		cycle = M_PI + M_PI*(cycle-HL2_BOB_UP)/(1.0 - HL2_BOB_UP);
	}
	
	g_verticalBob = speed*0.005f;
	g_verticalBob = g_verticalBob*0.3 + g_verticalBob*0.7*sin(cycle);

	g_verticalBob = clamp( g_verticalBob, -7.0f, 4.0f );

	//Calculate the lateral bob
	cycle = bobtime - (int)(bobtime/HL2_BOB_CYCLE_MAX*2)*HL2_BOB_CYCLE_MAX*2;
	cycle /= HL2_BOB_CYCLE_MAX*2;

	if ( cycle < HL2_BOB_UP )
	{
		cycle = M_PI * cycle / HL2_BOB_UP;
	}
	else
	{
		cycle = M_PI + M_PI*(cycle-HL2_BOB_UP)/(1.0 - HL2_BOB_UP);
	}

	g_lateralBob = speed*0.005f;
	g_lateralBob = g_lateralBob*0.3 + g_lateralBob*0.7*sin(cycle);
	g_lateralBob = clamp( g_lateralBob, -7.0f, 4.0f );
	
	//NOTENOTE: We don't use this return value in our case (need to restructure the calculation function setup!)
	return 0.0f;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : &origin - 
//			&angles - 
//			viewmodelindex - 
//-----------------------------------------------------------------------------
void CBaseVectronicCombatWeapon::AddViewmodelBob( CBaseViewModel *viewmodel, Vector &origin, QAngle &angles )
{
	Vector	forward, right;
	AngleVectors( angles, &forward, &right, NULL );

	CalcViewmodelBob();

	// Apply bob, but scaled down to 40%
	VectorMA( origin, g_verticalBob * 0.1f, forward, origin );
	
	// Z bob a bit more
	origin[2] += g_verticalBob * 0.1f;
	
	// bob the angles
	angles[ ROLL ]	+= g_verticalBob * 0.5f;
	angles[ PITCH ]	-= g_verticalBob * 0.4f;

	angles[ YAW ]	-= g_lateralBob  * 0.3f;

	VectorMA( origin, g_lateralBob * 0.8f, right, origin );
}

//-----------------------------------------------------------------------------
Vector CBaseVectronicCombatWeapon::GetBulletSpread( WeaponProficiency_t proficiency )
{
	return BaseClass::GetBulletSpread( proficiency );
}

//-----------------------------------------------------------------------------
float CBaseVectronicCombatWeapon::GetSpreadBias( WeaponProficiency_t proficiency )
{
	return BaseClass::GetSpreadBias( proficiency );
}
//-----------------------------------------------------------------------------

const WeaponProficiencyInfo_t *CBaseVectronicCombatWeapon::GetProficiencyValues()
{
	return NULL;
}

#else

// Server stubs
float CBaseVectronicCombatWeapon::CalcViewmodelBob( void )
{
	return 0.0f;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : &origin - 
//			&angles - 
//			viewmodelindex - 
//-----------------------------------------------------------------------------
void CBaseVectronicCombatWeapon::AddViewmodelBob( CBaseViewModel *viewmodel, Vector &origin, QAngle &angles )
{
}


//-----------------------------------------------------------------------------
Vector CBaseVectronicCombatWeapon::GetBulletSpread( WeaponProficiency_t proficiency )
{
	Vector baseSpread = BaseClass::GetBulletSpread( proficiency );

	const WeaponProficiencyInfo_t *pProficiencyValues = GetProficiencyValues();
	float flModifier = (pProficiencyValues)[ proficiency ].spreadscale;
	return ( baseSpread * flModifier );
}

//-----------------------------------------------------------------------------
float CBaseVectronicCombatWeapon::GetSpreadBias( WeaponProficiency_t proficiency )
{
	const WeaponProficiencyInfo_t *pProficiencyValues = GetProficiencyValues();
	return (pProficiencyValues)[ proficiency ].bias;
}

//-----------------------------------------------------------------------------
const WeaponProficiencyInfo_t *CBaseVectronicCombatWeapon::GetProficiencyValues()
{
	return GetDefaultProficiencyValues();
}

//-----------------------------------------------------------------------------
const WeaponProficiencyInfo_t *CBaseVectronicCombatWeapon::GetDefaultProficiencyValues()
{
	// Weapon proficiency table. Keep this in sync with WeaponProficiency_t enum in the header!!
	static WeaponProficiencyInfo_t g_BaseWeaponProficiencyTable[] =
	{
		{ 2.50, 1.0	},
		{ 2.00, 1.0	},
		{ 1.50, 1.0	},
		{ 1.25, 1.0 },
		{ 1.00, 1.0	},
	};

	COMPILE_TIME_ASSERT( ARRAYSIZE(g_BaseWeaponProficiencyTable) == WEAPON_PROFICIENCY_PERFECT + 1);

	return g_BaseWeaponProficiencyTable;
}

#endif

#ifdef CLIENT_DLL
//-----------------------------------------------------------------------------
// Purpose: Draw the weapon's crosshair
//-----------------------------------------------------------------------------
void CBaseVectronicCombatWeapon::DrawCrosshair()
{
	BaseClass::DrawCrosshair();

	if ( !crosshair.GetInt() )
		return;

	CHudCrosshair *pCrosshair = GET_HUDELEMENT( CHudCrosshair );

	if ( !pCrosshair )
		return;

	CBasePlayer *pFakePlayer = GetPlayerOwner();
	CVectronicPlayer *pPlayer = To_VectronicPlayer(pFakePlayer);

	if (pPlayer->PlayerHasObject())
	{
		pCrosshair->SetCrosshair( 0, Color( 255, 255, 255, 255 ) );
	}

	// clear crosshair
	pCrosshair->SetCrosshair( 0, Color( 255, 255, 255, 255 ) );

	/*
	C_VectronicPlayer * pPlayer = C_VectronicPlayer::GetLocalPlayer();

	if ( !pPlayer )
		return;
	
	if (!pPlayer->IsAlive() || pPlayer->IsObserver())
		return;
		*/

}

#endif