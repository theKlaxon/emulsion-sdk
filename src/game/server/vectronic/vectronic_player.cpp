//=========== Copyright © 2013 - 2014, rHetorical, All rights reserved. =============
//
// Purpose: A useable player simular to the HL2 Player minus the bullshit ties with 
// HL2.
//		
//====================================================================================

#include "cbase.h"
#include "vectronic_player.h"
#include "vectronic_gamerules.h"
#include "effect_dispatch_data.h"
#include "te_effect_dispatch.h" 
#include "predicted_viewmodel.h"
#include "player.h"
#include "simtimer.h"
#include "player_pickup.h"
#include "game.h"
#include "gamerules.h"
#include "trains.h"
#include "in_buttons.h" 
#include "globalstate.h"
#include "KeyValues.h"
#include "eventqueue.h"
#include "engine/IEngineSound.h"
#include "ai_basenpc.h"
#include "SoundEmitterSystem/isoundemittersystembase.h"
#include "vphysics/player_controller.h"
#include "datacache/imdlcache.h"
#include "soundenvelope.h"
#include "ai_speech.h"		
#include "sceneentity.h"
#include "hintmessage.h"
#include "items.h"
#include "weapon_vecgun.h"

// Our Player walk speed value.
ConVar player_walkspeed( "player_walkspeed", "190" );

// Show annotations?
ConVar hud_show_annotations( "hud_show_annotations", "1" );

// The delay from when we last got hurt to generate.
// This is already defined in ASW
#ifdef SWARM_DLL
extern ConVar sv_regeneration_wait_time;
#else
ConVar sv_regeneration_wait_time ("sv_regeneration_wait_time", "1.0", FCVAR_REPLICATED );
#endif

// Link us!
LINK_ENTITY_TO_CLASS( player, CVectronicPlayer );

IMPLEMENT_SERVERCLASS_ST (CVectronicPlayer, DT_VectronicPlayer) 
	SendPropBool( SENDINFO(m_bPlayerPickedUpObject) ),
	SendPropInt( SENDINFO( m_iShotsFired ), 8, SPROP_UNSIGNED ),
END_SEND_TABLE()

BEGIN_DATADESC( CVectronicPlayer )

	DEFINE_FIELD( m_bPlayerPickedUpObject, FIELD_BOOLEAN ),

	DEFINE_AUTO_ARRAY( m_vecMissPositions, FIELD_POSITION_VECTOR ),
	DEFINE_FIELD( m_nNumMissPositions, FIELD_INTEGER ),

	DEFINE_SOUNDPATCH( m_pAirSound ),

	DEFINE_FIELD( m_fTimeLastHurt, FIELD_TIME )

END_DATADESC()

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------
// Basics
//-----------------------
CVectronicPlayer::CVectronicPlayer()
{
	m_nNumMissPositions = 0;

	// Set up the hints.
	m_pHintMessageQueue = new CHintMessageQueue(this);
	m_iDisplayHistoryBits = 0;
	m_bShowHints = true;

	if ( m_pHintMessageQueue )
	{
		m_pHintMessageQueue->Reset();
	}
	
	// We did not fire any shots.
	m_iShotsFired = 0;
}

CVectronicPlayer::~CVectronicPlayer()
{
	delete m_pHintMessageQueue;
	m_pHintMessageQueue = NULL;

	m_flNextMouseoverUpdate = gpGlobals->curtime;

}

CBaseEntity* CVectronicPlayer::EntSelectSpawnPoint( void )
{
	extern CBaseEntity				*g_pLastSpawn;

	CBaseEntity *pSpot = NULL;
	//CBaseEntity *pLastSpawnPoint = g_pLastSpawn;
	const char *pSpawnpointName = "info_player_vecspawn";
	m_pSpawnPoint = dynamic_cast<CVectronicSpawn*>( pSpot );

	if ( SelectSpawnSpot( pSpawnpointName, pSpot ) )
	{
	//	g_pLastSpawn[ GetTeamNumber() ] = pSpot; //??
	}

	if ( !pSpot  )
	{
		Warning( "PutClientInServer: no %s on level! Spawning at default spawn instead!\n", pSpawnpointName );
		pSpot = gEntList.FindEntityByClassname( pSpot, "info_player_start" );

		if ( !pSpot )
		{
			Warning( "PutClientInServer: no %s on level\n", pSpawnpointName );
			return CBaseEntity::Instance( INDEXENT(0) );
		}

	}

	return pSpot;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CVectronicPlayer::SelectSpawnSpot( const char *pEntClassName, CBaseEntity* &pSpot )
{
	// Get an initial spawn point.
	pSpot = gEntList.FindEntityByClassname( pSpot, pEntClassName );
	if ( !pSpot )
	{
		// Sometimes the first spot can be NULL????
		pSpot = gEntList.FindEntityByClassname( pSpot, pEntClassName );
	}

	// First we try to find a spawn point that is fully clear. If that fails,
	// we look for a spawnpoint that's clear except for another players. We
	// don't collide with our team members, so we should be fine.
	bool bIgnorePlayers = false;

	CBaseEntity *pFirstSpot = pSpot;
	do 
	{
		if ( pSpot )
		{
			// Check to see if this is a valid team spawn (player is on this team, etc.).
			if( rHGameRules()->IsSpawnPointValid( pSpot, this, bIgnorePlayers ) )
			{
				// Check for a bad spawn entity.
				if ( pSpot->GetAbsOrigin() == Vector( 0, 0, 0 ) )
				{
					pSpot = gEntList.FindEntityByClassname( pSpot, pEntClassName );
					continue;
				}

				// Found a valid spawn point.
				return true;
			}
		}

		// Get the next spawning point to check.
		pSpot = gEntList.FindEntityByClassname( pSpot, pEntClassName );

		if ( pSpot == pFirstSpot && !bIgnorePlayers )
		{
			// Loop through again, ignoring players
			bIgnorePlayers = true;
			pSpot = gEntList.FindEntityByClassname( pSpot, pEntClassName );
		}
	} 
	// Continue until a valid spawn point is found or we hit the start.
	while ( pSpot != pFirstSpot ); 

	return false;
}

void CVectronicPlayer::Precache( void )
{
	BaseClass::Precache();

	// We don't precache sounds here but in the vectonic_client in ASW.
#ifndef SWARM_DLL
	// Sounds
	PrecacheScriptSound( SOUND_HINT );
	PrecacheScriptSound( SOUND_USE );
	PrecacheScriptSound( SOUND_USE_DENY );

	PrecacheScriptSound( SOUND_WHOOSH );
#endif

	// Last, precache the player model or else the game will crash when the player dies.
	PrecacheModel ( "models/player.mdl" );
}

void CVectronicPlayer::Spawn()
{
	// Dying without a player model crashes the client
	SetModel("models/player.mdl");
	SetMaxSpeed( PLAYER_WALK_SPEED );
	BaseClass::Spawn();
	StartWalking();	


#ifdef PLAYER_MOUSEOVER_HINTS
	m_iDisplayHistoryBits &= ~DHM_ROUND_CLEAR;
	SetLastSeenEntity ( NULL );
#endif

	// We did not fire any shots.
	m_iShotsFired = 0;

	GiveDefaultItems();

	GetPlayerProxy();

}

//------------------------------------------------------------------------------
// Purpose :
// Input   :
// Output  :
//------------------------------------------------------------------------------
Class_T  CVectronicPlayer::Classify ( void )
{
	return CLASS_PLAYER;
}

void CVectronicPlayer::PreThink()
{
	BaseClass::PreThink();

	if ( m_pHintMessageQueue )
		m_pHintMessageQueue->Update();
}

void CVectronicPlayer::PostThink()
{
	BaseClass::PostThink();

	// Keep the model upright; pose params will handle pitch aiming.
	QAngle angles = GetLocalAngles();
	angles[PITCH] = 0;
	SetLocalAngles(angles);

	//Play Woosh!
	CreateSounds();
	UpdateWooshSounds();

	if ( m_flNextMouseoverUpdate < gpGlobals->curtime )
	{
		m_flNextMouseoverUpdate = gpGlobals->curtime + 0.2f;
		if ( m_bShowHints )
		{
			#ifdef PLAYER_MOUSEOVER_HINTS
			UpdateMouseoverHints();
			#endif
		}
	}

	// Regenerate heath after 3 seconds
	if ( IsAlive() && GetHealth() < GetMaxHealth() )
	{
		// Color to overlay on the screen while the player is taking damage
		color32 hurtScreenOverlay = {64,0,0,64};

		if ( gpGlobals->curtime > m_fTimeLastHurt + sv_regeneration_wait_time.GetFloat() )
		{
			TakeHealth( 1, DMG_GENERIC );
			m_bIsRegenerating = true;

			if ( GetHealth() >= GetMaxHealth() )
			{
				m_bIsRegenerating = false;
			}
		}
		else
		{
			m_bIsRegenerating = false;
			UTIL_ScreenFade( this, hurtScreenOverlay, 1.0f, 0.1f, FFADE_IN|FFADE_PURGE );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Makes a splash when the player transitions between water states
//-----------------------------------------------------------------------------
void CVectronicPlayer::Splash( void )
{
	CEffectData data;
	data.m_fFlags = 0;
	data.m_vOrigin = GetAbsOrigin();
	data.m_vNormal = Vector(0,0,1);
	data.m_vAngles = QAngle( 0, 0, 0 );
	
	if ( GetWaterType() & CONTENTS_SLIME )
	{
		data.m_fFlags |= FX_WATER_IN_SLIME;
	}

	float flSpeed = GetAbsVelocity().Length();
	if ( flSpeed < 300 )
	{
		data.m_flScale = random->RandomFloat( 10, 12 );
		DispatchEffect( "waterripple", data );
	}
	else
	{
		data.m_flScale = random->RandomFloat( 6, 8 );
		DispatchEffect( "watersplash", data );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVectronicPlayer::UpdateClientData( void )
{
	if (m_DmgTake || m_DmgSave || m_bitsHUDDamage != m_bitsDamageType)
	{
		// Comes from inside me if not set
		Vector damageOrigin = GetLocalOrigin();
		// send "damage" message
		// causes screen to flash, and pain compass to show direction of damage
		damageOrigin = m_DmgOrigin;

		// only send down damage type that have hud art
		int iShowHudDamage = g_pGameRules->Damage_GetShowOnHud();
		int visibleDamageBits = m_bitsDamageType & iShowHudDamage;

		m_DmgTake = clamp( m_DmgTake, 0, 255 );
		m_DmgSave = clamp( m_DmgSave, 0, 255 );

		// If we're poisoned, but it wasn't this frame, don't send the indicator
		// Without this check, any damage that occured to the player while they were
		// recovering from a poison bite would register as poisonous as well and flash
		// the whole screen! -- jdw
		if ( visibleDamageBits & DMG_POISON )
		{
			float flLastPoisonedDelta = gpGlobals->curtime - m_tbdPrev;
			if ( flLastPoisonedDelta > 0.1f )
			{
				visibleDamageBits &= ~DMG_POISON;
			}
		}

		CSingleUserRecipientFilter user( this );
		user.MakeReliable();
		UserMessageBegin( user, "Damage" );
			WRITE_BYTE( m_DmgSave );
			WRITE_BYTE( m_DmgTake );
			WRITE_LONG( visibleDamageBits );
			WRITE_FLOAT( damageOrigin.x );	//BUG: Should be fixed point (to hud) not floats
			WRITE_FLOAT( damageOrigin.y );	//BUG: However, the HUD does _not_ implement bitfield messages (yet)
			WRITE_FLOAT( damageOrigin.z );	//BUG: We use WRITE_VEC3COORD for everything else
		MessageEnd();
	
		m_DmgTake = 0;
		m_DmgSave = 0;
		m_bitsHUDDamage = m_bitsDamageType;
		
		// Clear off non-time-based damage indicators
		int iTimeBasedDamage = g_pGameRules->Damage_GetTimeBased();
		m_bitsDamageType &= iTimeBasedDamage;
	}

	BaseClass::UpdateClientData();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------
// Viewmodel and weapon stuff!
//-----------------------
void CVectronicPlayer::CreateViewModel( int index )
{
	BaseClass::CreateViewModel( index );
	return;
	Assert( index >= 0 && index < MAX_VIEWMODELS );

	if ( GetViewModel( index ) )
		return;

	CPredictedViewModel *vm = ( CPredictedViewModel * )CreateEntityByName( "predicted_viewmodel" );
	if ( vm )
	{
		vm->SetAbsOrigin( GetAbsOrigin() );
		vm->SetOwner( this );
		vm->SetIndex( index );
		DispatchSpawn( vm );
		vm->FollowEntity( this, false );
		m_hViewModel.Set( index, vm );
	}
}

bool CVectronicPlayer::Weapon_Switch( CBaseCombatWeapon *pWeapon, int viewmodelindex )
{
	bool bRet = BaseClass::Weapon_Switch( pWeapon, viewmodelindex );

	return bRet;
}

void CVectronicPlayer::Weapon_Equip( CBaseCombatWeapon *pWeapon )
{
	BaseClass::Weapon_Equip( pWeapon );
}

extern int	gEvilImpulse101;
//-----------------------------------------------------------------------------
// Purpose: Player reacts to bumping a weapon. 
// Input  : pWeapon - the weapon that the player bumped into.
// Output : Returns true if player picked up the weapon
//-----------------------------------------------------------------------------
bool CVectronicPlayer::BumpWeapon( CBaseCombatWeapon *pWeapon )
{
	CBaseCombatCharacter *pOwner = pWeapon->GetOwner();

	// Can I have this weapon type?
	if ( !IsAllowedToPickupWeapons() )
		return false;

	if ( pOwner || !Weapon_CanUse( pWeapon ) || !g_pGameRules->CanHavePlayerItem( this, pWeapon ) )
	{
		if ( gEvilImpulse101 )
		{
			UTIL_Remove( pWeapon );
		}
		return false;
	}

	// Don't let the player fetch weapons through walls (use MASK_SOLID so that you can't pickup through windows)
	if( !pWeapon->FVisible( this, MASK_SOLID ) && !(GetFlags() & FL_NOTARGET) )
	{
		return false;
	}

	// ----------------------------------------
	// If I already have it just take the ammo
	// ----------------------------------------
	if (Weapon_OwnsThisType( pWeapon->GetClassname(), pWeapon->GetSubType())) 
	{
		//Only remove the weapon if we attained ammo from it
		if ( Weapon_EquipAmmoOnly( pWeapon ) == false )
			return false;

		// Only remove me if I have no ammo left
		// Can't just check HasAnyAmmo because if I don't use clips, I want to be removed, 
		if ( pWeapon->UsesClipsForAmmo1() && pWeapon->HasPrimaryAmmo() )
			return false;

		UTIL_Remove( pWeapon );
		return false;
	}
	// -------------------------
	// Otherwise take the weapon
	// -------------------------
	else 
	{
		//Make sure we're not trying to take a new weapon type we already have
		if ( Weapon_SlotOccupied( pWeapon ) )
		{
			CBaseCombatWeapon *pActiveWeapon = Weapon_GetSlot( 2 );

			if ( pActiveWeapon != NULL && pActiveWeapon->HasAnyAmmo() == false && Weapon_CanSwitchTo( pWeapon ) )
			{
				Weapon_Equip( pWeapon );
				return true;
			}

			//Attempt to take ammo if this is the gun we're holding already
			if ( Weapon_OwnsThisType( pWeapon->GetClassname(), pWeapon->GetSubType() ) )
			{
				Weapon_EquipAmmoOnly( pWeapon );
			}

			return false;
		}
	}

	pWeapon->CheckRespawn();
	Weapon_Equip( pWeapon );

	return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------
// Walking
//-----------------------
void CVectronicPlayer::StartWalking( void )
{
	SetMaxSpeed( PLAYER_WALK_SPEED );
	m_fIsWalking = true;
}

void CVectronicPlayer::StopWalking( void )
{
	SetMaxSpeed( PLAYER_WALK_SPEED );
	m_fIsWalking = false;
}

void CVectronicPlayer::HandleSpeedChanges( void )
{
	bool bIsWalking = IsWalking();
	bool bWantWalking;
	
	bWantWalking = true;
	
	if( bIsWalking != bWantWalking )
	{
		if ( bWantWalking )
		{
			StartWalking();
		}
		else
		{
			StopWalking();
		}
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------
// Use + Pickup
//-----------------------
bool CVectronicPlayer::CanPickupObject( CBaseEntity *pObject, float massLimit, float sizeLimit )
{
	// reep: Ported from the base since in the base this is HL2 exclusive. Yeah, don't you LOVE base code? 

	//Must be valid
	if ( pObject == NULL )
		return false;

	//Must move with physics
	if ( pObject->GetMoveType() != MOVETYPE_VPHYSICS )
		return false;

	IPhysicsObject *pList[VPHYSICS_MAX_OBJECT_LIST_COUNT];
	int count = pObject->VPhysicsGetObjectList( pList, ARRAYSIZE(pList) );

	//Must have a physics object
	if (!count)
		return false;

	float objectMass = 0;
	bool checkEnable = false;
	for ( int i = 0; i < count; i++ )
	{
		objectMass += pList[i]->GetMass();
		if ( !pList[i]->IsMoveable() )
		{
			checkEnable = true;
		}
		if ( pList[i]->GetGameFlags() & FVPHYSICS_NO_PLAYER_PICKUP )
			return false;
		if ( pList[i]->IsHinged() )
			return false;
	}

	//Must be under our threshold weight
	if ( massLimit > 0 && objectMass > massLimit )
		return false;
	/*
	// reep: Could not get it to recognize classes. Think this is for the physcannon anyway. 
	if ( checkEnable )
	{
		#include "props.h"
		#include "vphysics/player_controller.h"
		#include "physobj.h"
		// Allow pickup of phys props that are motion enabled on player pickup
		CPhysicsProp *pProp = dynamic_cast<CPhysicsProp*>(pObject);
		CPhysBox *pBox = dynamic_cast<CPhysBox*>(pObject);
		if ( !pProp && !pBox )
			return false;

		if ( pProp && !(pProp->HasSpawnFlags( SF_PHYSPROP_ENABLE_ON_PHYSCANNON )) )
			return false;

		if ( pBox && !(pBox->HasSpawnFlags( SF_PHYSBOX_ENABLE_ON_PHYSCANNON )) )
			return false;
	}
	*/

	if ( sizeLimit > 0 )
	{
		const Vector &size = pObject->CollisionProp()->OBBSize();
		if ( size.x > sizeLimit || size.y > sizeLimit || size.z > sizeLimit )
			return false;
	}

	return true;
}

void CVectronicPlayer::PickupObject( CBaseEntity *pObject, bool bLimitMassAndSize )
{
	// can't pick up what you're standing on
	if ( GetGroundEntity() == pObject )
	{
		DevMsg("Failed to pickup object: Player is standing on object!\n");
		PlayUseDenySound();
		return;
	}

	if ( bLimitMassAndSize == true )
	{
		if ( CanPickupObject( pObject, PLAYER_MAX_LIFT_MASS, PLAYER_MAX_LIFT_SIZE ) == false )
		{
			DevMsg("Failed to pickup object: Object too heavy!\n");
			PlayUseDenySound();
			return;
		}
	}

	// Can't be picked up if NPCs are on me
	if ( pObject->HasNPCsOnIt() )
		return;

	// Bool is to tell the client that we have an object. This is incase you want to change the crosshair 
	// or something for your project.
	m_bPlayerPickedUpObject = true;

	PlayerPickupObject( this, pObject );

}

void CVectronicPlayer::ForceDropOfCarriedPhysObjects( CBaseEntity *pOnlyIfHoldingThis )
{
	m_bPlayerPickedUpObject = false;
	BaseClass::ForceDropOfCarriedPhysObjects( pOnlyIfHoldingThis );
}

void CVectronicPlayer::PlayerUse ( void )
{
	// Was use pressed or released?
	if ( ! ((m_nButtons | m_afButtonPressed | m_afButtonReleased) & IN_USE) )
		return;

	if ( m_afButtonPressed & IN_USE )
	{
		// Currently using a latched entity?
		if ( ClearUseEntity() )
		{
			if (m_bPlayerPickedUpObject)
			{
				m_bPlayerPickedUpObject = false;
			}
			return;
		}
		else
		{
			if ( m_afPhysicsFlags & PFLAG_DIROVERRIDE )
			{
				m_afPhysicsFlags &= ~PFLAG_DIROVERRIDE;
				m_iTrain = TRAIN_NEW|TRAIN_OFF;
				return;
			}
		}

		// Tracker 3926:  We can't +USE something if we're climbing a ladder
		if ( GetMoveType() == MOVETYPE_LADDER )
		{
			return;
		}
	}

	if( m_flTimeUseSuspended > gpGlobals->curtime )
	{
		// Something has temporarily stopped us being able to USE things.
		// Obviously, this should be used very carefully.(sjb)
		return;
	}

	CBaseEntity *pUseEntity = FindUseEntity();

	bool usedSomething = false;

	// Found an object
	if ( pUseEntity )
	{
		//!!!UNDONE: traceline here to prevent +USEing buttons through walls			
		int caps = pUseEntity->ObjectCaps();
		variant_t emptyVariant;

		if ( m_afButtonPressed & IN_USE )
		{
			// Robin: Don't play sounds for NPCs, because NPCs will allow respond with speech.
			if ( !pUseEntity->MyNPCPointer() )
			{
				EmitSound( SOUND_USE );
			}
		}

		if ( ( (m_nButtons & IN_USE) && (caps & FCAP_CONTINUOUS_USE) ) ||
			 ( (m_afButtonPressed & IN_USE) && (caps & (FCAP_IMPULSE_USE|FCAP_ONOFF_USE)) ) )
		{
			if ( caps & FCAP_CONTINUOUS_USE )
				m_afPhysicsFlags |= PFLAG_USING;

			pUseEntity->AcceptInput( "Use", this, this, emptyVariant, USE_TOGGLE );

			usedSomething = true;
		}
		// UNDONE: Send different USE codes for ON/OFF.  Cache last ONOFF_USE object to send 'off' if you turn away
		else if ( (m_afButtonReleased & IN_USE) && (pUseEntity->ObjectCaps() & FCAP_ONOFF_USE) )	// BUGBUG This is an "off" use
		{
			pUseEntity->AcceptInput( "Use", this, this, emptyVariant, USE_TOGGLE );

			usedSomething = true;
		}
	}

	else if ( m_afButtonPressed & IN_USE )
	{
		// Signal that we want to play the deny sound, unless the user is +USEing on a ladder!
		// The sound is emitted in ItemPostFrame, since that occurs after GameMovement::ProcessMove which
		// lets the ladder code unset this flag.
		m_bPlayUseDenySound = true;
	}

	// Debounce the use key
	if ( usedSomething && pUseEntity )
	{
		m_Local.m_nOldButtons |= IN_USE;
		m_afButtonPressed &= ~IN_USE;
	}
}

void CVectronicPlayer::ClearUsePickup()
{
	m_bPlayerPickedUpObject = false;
}

void CVectronicPlayer::PlayUseDenySound()
{
	m_bPlayUseDenySound = true;
}

void CVectronicPlayer::ItemPostFrame()
{
	BaseClass::ItemPostFrame();

	if ( m_bPlayUseDenySound )
	{
		m_bPlayUseDenySound = false;
		EmitSound( SOUND_USE_DENY );
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------
// Damage
//-----------------------
bool CVectronicPlayer::PassesDamageFilter( const CTakeDamageInfo &info )
{
	CBaseEntity *pAttacker = info.GetAttacker();
	if( pAttacker && pAttacker->MyNPCPointer() && pAttacker->MyNPCPointer()->IsPlayerAlly() )
	{
		return false;
	}

	if( m_hPlayerProxy && !m_hPlayerProxy->PassesDamageFilter( info ) )
	{
		return false;
	}

	return BaseClass::PassesDamageFilter( info );
}

int	CVectronicPlayer::OnTakeDamage( const CTakeDamageInfo &info )
{
	CTakeDamageInfo inputInfoCopy( info );

	// If you shoot yourself, make it hurt but push you less
	if ( inputInfoCopy.GetAttacker() == this && inputInfoCopy.GetDamageType() == DMG_BULLET )
	{
		inputInfoCopy.ScaleDamage( 5.0f );
		inputInfoCopy.ScaleDamageForce( 0.05f );
	}

	int ret = BaseClass::OnTakeDamage( inputInfoCopy );
	m_DmgOrigin = info.GetDamagePosition();

#ifdef PLAYER_IGNORE_FALLDAMAGE
	// ignore fall damage if instructed to do so by input
	if ( ( info.GetDamageType() & DMG_FALL ) )
	{
		inputInfoCopy.SetDamage(0.0f);
#ifdef SWARM_DLL
		return 0;
#endif
	}
#endif

	if ( GetHealth() < 100 )
	{
		m_fTimeLastHurt = gpGlobals->curtime;
	}

	return ret;
}

int CVectronicPlayer::OnTakeDamage_Alive( const CTakeDamageInfo &info )
{
	// set damage type sustained
	m_bitsDamageType |= info.GetDamageType();

	if ( !CBaseCombatCharacter::OnTakeDamage_Alive( info ) )
		return 0;

	CBaseEntity * attacker = info.GetAttacker();

	if ( !attacker )
		return 0;

	Vector vecDir = vec3_origin;
	if ( info.GetInflictor() )
	{
		vecDir = info.GetInflictor()->WorldSpaceCenter() - Vector ( 0, 0, 10 ) - WorldSpaceCenter();
		VectorNormalize( vecDir );
	}

	if ( info.GetInflictor() && (GetMoveType() == MOVETYPE_WALK) && 
		( !attacker->IsSolidFlagSet(FSOLID_TRIGGER)) )
	{
		Vector force = vecDir;// * -DamageForce( WorldAlignSize(), info.GetBaseDamage() );
		if ( force.z > 250.0f )
		{
			force.z = 250.0f;
		}
		ApplyAbsVelocityImpulse( force );
	}

	// Burnt
	if ( info.GetDamageType() & DMG_BURN )
	{
		EmitSound( "HL2Player.BurnPain" );
	}

	// fire global game event

	IGameEvent * event = gameeventmanager->CreateEvent( "player_hurt" );
	if ( event )
	{
		event->SetInt("userid", GetUserID() );
		event->SetInt("health", MAX(0, m_iHealth) );
		event->SetInt("priority", 5 );	// HLTV event priority, not transmitted

		if ( attacker->IsPlayer() )
		{
			CBasePlayer *player = ToBasePlayer( attacker );
			event->SetInt("attacker", player->GetUserID() ); // hurt by other player
		}
		else
		{
			event->SetInt("attacker", 0 ); // hurt by "world"
		}

		gameeventmanager->FireEvent( event );
	}

	// Insert a combat sound so that nearby NPCs hear battle
	if ( attacker->IsNPC() )
	{
		CSoundEnt::InsertSound( SOUND_COMBAT, GetAbsOrigin(), 512, 0.5, this );
	}

	return 1;
}

void CVectronicPlayer::OnDamagedByExplosion( const CTakeDamageInfo &info )
{
	if ( info.GetInflictor() && info.GetInflictor()->ClassMatches( "mortarshell" ) )
	{
		// No ear ringing for mortar
		UTIL_ScreenShake( info.GetInflictor()->GetAbsOrigin(), 4.0, 1.0, 0.5, 1000, SHAKE_START, false );
		return;
	}
	BaseClass::OnDamagedByExplosion( info );
}

#ifndef SWARM_DLL
void CVectronicPlayer::FirePlayerProxyOutput( const char *pszOutputName, variant_t variant, CBaseEntity *pActivator, CBaseEntity *pCaller )
{
	if ( GetPlayerProxy() == NULL )
		return;

	GetPlayerProxy()->FireNamedOutput( pszOutputName, variant, pActivator, pCaller );
}
#endif

void CVectronicPlayer::Event_Killed( const CTakeDamageInfo &info )
{
	BaseClass::Event_Killed( info );

	CWeaponVecgun *pVecgun = dynamic_cast<CWeaponVecgun*>( Weapon_OwnsThisType( "weapon_vecgun" ) );

	if(pVecgun)
	{
		UTIL_Remove( pVecgun );
	}


	FirePlayerProxyOutput( "PlayerDied", variant_t(), this, this );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------
// Falling Air Sound
//-----------------------
void CVectronicPlayer::CreateSounds()
{
	if ( !m_pAirSound )
	{
		CSoundEnvelopeController &controller = CSoundEnvelopeController::GetController();

		CPASAttenuationFilter filter( this );

		m_pAirSound = controller.SoundCreate( filter, entindex(), SOUND_WHOOSH );
		controller.Play( m_pAirSound, 0, 100 );
	}
}

void CVectronicPlayer::StopLoopingSounds()
{
	if ( m_pAirSound )
	{
		CSoundEnvelopeController &controller = CSoundEnvelopeController::GetController();

		controller.SoundDestroy( m_pAirSound );
		m_pAirSound = NULL;
	}

	BaseClass::StopLoopingSounds();
}

void CVectronicPlayer::UpdateWooshSounds( void )
{
	if ( m_pAirSound )
	{
		CSoundEnvelopeController &controller = CSoundEnvelopeController::GetController();

		float fWooshVolume = GetAbsVelocity().Length() - 300;

		if ( fWooshVolume < 0.0f )
		{
			controller.SoundChangeVolume( m_pAirSound, 0.0f, 0.1f );
			return;
		}

		fWooshVolume /= 2000.0f;
		if ( fWooshVolume > 1.0f )
			fWooshVolume = 1.0f;

		controller.SoundChangeVolume( m_pAirSound, fWooshVolume, 0.1f );
	}
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------
// Item Gifting
//-----------------------
extern int	gEvilImpulse101;

void CVectronicPlayer::CheatImpulseCommands( int iImpulse )
{
	switch ( iImpulse )
	{
	case 101:
		{
			if( sv_cheats->GetBool() )
			{
				GiveAllItems();
			}
		}
		break;

	default:
		BaseClass::CheatImpulseCommands( iImpulse );
	}
}

void CVectronicPlayer::GiveAllItems( void )
{
	// Impluse 101
	CWeaponVecgun *pVecGun = static_cast<CWeaponVecgun*>(Weapon_OwnsThisType( "weapon_vecgun" ) );
	if ( pVecGun )
	{
		pVecGun->SetCanFireBlue();
		pVecGun->SetCanFireGreen();
		pVecGun->SetCanFireYellow();
	}
}

void CVectronicPlayer::GiveDefaultItems( void )
{
	// If you want the player to always start with something, give it
	// to them here.

	GiveNamedItem( "weapon_vecgun" );
}

void CVectronicPlayer::Weapon_ChangeBallLife(float value)
{
	CWeaponVecgun *pVecgun = dynamic_cast<CWeaponVecgun*>(Weapon_OwnsThisType("weapon_vecgun"));

	if (pVecgun)
	{
		pVecgun->SetBallLife(value);
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------
// Hint Systems
//-----------------------
void CVectronicPlayer::HintMessage( const char *pMessage, bool bDisplayIfDead, bool bOverrideClientSettings, bool bQuiet )
{
	if (!hud_show_annotations.GetBool())
		return;

	if ( !bDisplayIfDead && !IsAlive() || !IsNetClient() || !m_pHintMessageQueue )
		return;

#ifndef SWARM_DLL
	//Are we gonna play a sound?
	if(!bQuiet)
	{
		EmitSound(SOUND_HINT);
	}
#endif

	if ( bOverrideClientSettings || m_bShowHints )
		m_pHintMessageQueue->AddMessage( pMessage );
}

// All other mouse over hints.
#ifdef PLAYER_MOUSEOVER_HINTS
void CVectronicPlayer::UpdateMouseoverHints()
{
	// Don't show if we are DEAD!
	if ( !IsAlive())
		return;

	if(m_bPlayerPickedUpObject)
		return;

	Vector forward, up;
	EyeVectors( &forward, NULL, &up );

	trace_t tr;
	// Search for objects in a sphere (tests for entities that are not solid, yet still useable)
	Vector searchStart = EyePosition();
	Vector searchEnd = searchStart + forward * 2048;

	int useableContents = MASK_NPCSOLID_BRUSHONLY | MASK_VISIBLE_AND_NPCS;

	bool bItemGlowing = false;

	UTIL_TraceLine( searchStart, searchEnd, useableContents, this, COLLISION_GROUP_NONE, &tr );

	if ( tr.fraction != 1.0f )
	{
		if (tr.DidHitNonWorldEntity() && tr.m_pEnt)
		{
			CBaseEntity *pObject = tr.m_pEnt;

			int classify = pObject->Classify();

			// You can make it so that when the player roles over a class type, it will activate. Like
			// if we run into another player or an ally, tell the user via hint that they're friends. <3
			if( classify == CLASS_PLAYER || classify == CLASS_PLAYER_ALLY )
			{
				if ( g_pGameRules->PlayerRelationship( this, pObject ) == GR_TEAMMATE )
				{
					if ( !(m_iDisplayHistoryBits & DHF_FRIEND_SEEN) )
					{
						m_iDisplayHistoryBits |= DHF_FRIEND_SEEN;
						HintMessage( "#Hint_spotted_a_friend", true );
					}
				}

				return;
			}

			// You can also be entity specific. Like here, if the player roles over any weapon class,
			// it will use the engine's glow system. Keep in mind ASW's and This branch of source uses
			// diffrent glow systems. Plus, both are called via server this way. For SP, this is fine,
			// But MP needs to do this cliently somehow. 
			if ( pObject->ClassMatches("weapon_*") )
			{
				// Ok, check to see if we have glown any other entities.
				if( m_hLastSeenEntity.Get() )
				{
					// If we did (!= null), then make the last get entity unglow.
					if (m_hLastSeenEntity != NULL)
					{
						m_hLastSeenEntity->SetGlow(false);
						bItemGlowing = false;
						SetLastSeenEntity ( NULL );
					}
				}

				if (!bItemGlowing)
				{
					pObject->SetGlow(true);
					bItemGlowing = true;
					SetLastSeenEntity ( pObject );
				} 
				return;
			}
		}

		// Reset everything!
		else if ( m_bPlayerPickedUpObject || tr.DidHitWorld() && tr.m_pEnt || tr.fraction >= 1.0f ) // We are looking at the world.
		{
			if( m_hLastSeenEntity.Get() )
			{
				m_hLastSeenEntity->SetGlow(false);
				bItemGlowing = false;
				m_bMouseOverEnemy = false;
				SetLastSeenEntity ( NULL );
			}

			m_bMouseOverEnemy = false;

			return;
		}
	}
}
#endif

CLogicPlayerProxy *CVectronicPlayer::GetPlayerProxy( void )
{
	CLogicPlayerProxy *pProxy = dynamic_cast< CLogicPlayerProxy* > ( m_hPlayerProxy.Get() );

	if ( pProxy == NULL )
	{
		pProxy = (CLogicPlayerProxy*)gEntList.FindEntityByClassname(NULL, "logic_playerproxy" );

		if ( pProxy == NULL )
			return NULL;

		pProxy->m_hPlayer = this;
		m_hPlayerProxy = pProxy;
	}

	return pProxy;
}

//-----------------------------------------------------------------------------
// Here we have our hud hint entity, since it only works in SP due to
// UTIL_GetLocalPlayer(), we will put it here.
//-----------------------------------------------------------------------------

#define SF_HUDHINT_ALLPLAYERS			0x0001

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CHudAnnotation : public CPointEntity
{
public:
	DECLARE_CLASS( CHudAnnotation, CPointEntity );

	void	Spawn( void );
	void	Precache( void );

	inline	void	MessageSet( const char *pMessage ) { m_iszMessage = AllocPooledString(pMessage); }
	inline	const char *MessageGet( void )	{ return STRING( m_iszMessage ); }

private:

	inline	bool	AllPlayers( void ) { return (m_spawnflags & SF_HUDHINT_ALLPLAYERS) != 0; }

	CHandle<CBasePlayer> m_pPlayer;
	bool m_bWriteOnScreen;
	bool m_bHintQuiet;

	void InputShowHudHint( inputdata_t &inputdata );

	string_t m_iszMessage;

	DECLARE_DATADESC();
};

LINK_ENTITY_TO_CLASS( hud_annotation, CHudAnnotation );

BEGIN_DATADESC( CHudAnnotation )

	DEFINE_KEYFIELD( m_iszMessage, FIELD_STRING, "display_text" ),
	DEFINE_KEYFIELD( m_bWriteOnScreen, FIELD_BOOLEAN, "simpledisplay" ),
	DEFINE_KEYFIELD( m_bHintQuiet, FIELD_BOOLEAN, "quiet" ),

	DEFINE_INPUTFUNC( FIELD_VOID, "Show", InputShowHudHint ),

END_DATADESC()



//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudAnnotation::Spawn( void )
{
	Precache();

	SetSolid( SOLID_NONE );
	SetMoveType( MOVETYPE_NONE );
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudAnnotation::Precache( void )
{
}

//-----------------------------------------------------------------------------
// Purpose: Input handler for showing the message and/or playing the sound.
//-----------------------------------------------------------------------------
void CHudAnnotation::InputShowHudHint( inputdata_t &inputdata )
{
	CBaseEntity *pGetPlayer = NULL;
	if ( inputdata.pActivator && inputdata.pActivator->IsPlayer() )
	{
		pGetPlayer = inputdata.pActivator;
	}
	else
	{
		pGetPlayer = UTIL_GetLocalPlayer();
	}

	CVectronicPlayer *pPlayer = To_VectronicPlayer(pGetPlayer);
	if (!m_bWriteOnScreen)
	{
		pPlayer->HintMessage( MessageGet(), false, false, m_bHintQuiet );
	}
	else
	{
		//Display on screen only for the player. 
		CSingleUserRecipientFilter user( (CBasePlayer *)pPlayer );
		user.MakeReliable();

		if (!AllPlayers())
		{
			UTIL_ClientPrintFilter( user, HUD_PRINTCENTER, MessageGet() );
		}
		else
		{
			UTIL_ClientPrintAll( HUD_PRINTCENTER, MessageGet() );
		}
	}
}

// Player Spawn
BEGIN_DATADESC( CVectronicSpawn )

	DEFINE_KEYFIELD( m_bDisabled, FIELD_BOOLEAN, "StartDisabled" ),

	// Inputs.
	DEFINE_INPUTFUNC( FIELD_VOID, "Enable", InputEnable ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Disable", InputDisable ),

	// Outputs.

END_DATADESC()

LINK_ENTITY_TO_CLASS( info_player_vecspawn, CVectronicSpawn );

//-----------------------------------------------------------------------------
// Purpose: Constructor.
//-----------------------------------------------------------------------------
CVectronicSpawn::CVectronicSpawn()
{
	m_bDisabled = false;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVectronicSpawn::Activate( void )
{
	BaseClass::Activate();

	Vector mins = g_pGameRules->GetViewVectors()->m_vHullMin;
	Vector maxs = g_pGameRules->GetViewVectors()->m_vHullMax;

	trace_t trace;
	UTIL_TraceHull( GetAbsOrigin(), GetAbsOrigin(), mins, maxs, MASK_PLAYERSOLID, NULL, COLLISION_GROUP_PLAYER_MOVEMENT, &trace );
	bool bClear = ( trace.fraction == 1 && trace.allsolid != 1 && (trace.startsolid != 1) );
	if ( !bClear )
	{
		Warning("Spawnpoint at (%.2f %.2f %.2f) is not clear.\n", GetAbsOrigin().x, GetAbsOrigin().y, GetAbsOrigin().z );
		// m_debugOverlays |= OVERLAY_TEXT_BIT;
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CVectronicSpawn::InputEnable( inputdata_t &inputdata )
{
	m_bDisabled = false;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CVectronicSpawn::InputDisable( inputdata_t &inputdata )
{
	m_bDisabled = true;
}
