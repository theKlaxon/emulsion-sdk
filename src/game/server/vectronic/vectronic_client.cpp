//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//

#include "cbase.h"
#include "vectronic_player.h"
#include "vectronic_gamerules.h"
#include "gamerules.h"
#include "entitylist.h"
#include "physics.h"
#include "game.h"
#include "player_resource.h"
#include "engine/IEngineSound.h"
#include "tier0/vprof.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

void Host_Say( edict_t *pEdict, bool teamonly );

extern CBaseEntity*	FindPickerEntityClass( CBasePlayer *pPlayer, char *classname );
extern bool			g_fGameOver;

//ConVar	vectronic_enable_respawn ( "vectronic_enable_respawn","1" );

/*
===========
ClientPutInServer

called each time a player is spawned into the game
============
*/
void ClientPutInServer( edict_t *pEdict, const char *playername )
{
	// Allocate a CBasePlayer for pev, and call spawn
	CVectronicPlayer *pPlayer = CVectronicPlayer::CreatePlayer( "player", pEdict );
	pPlayer->SetPlayerName( playername );
}


void ClientActive( edict_t *pEdict, bool bLoadGame )
{
	CVectronicPlayer *pPlayer = dynamic_cast< CVectronicPlayer* >( CBaseEntity::Instance( pEdict ) );
	Assert( pPlayer );

	if ( !pPlayer )
	{
		return;
	}

	pPlayer->InitialSpawn();
	if ( !bLoadGame )
	{
		pPlayer->Spawn();
	}
}

/*
===============
const char *GetGameDescription()

Returns the descriptive name of this .dll.  E.g., Half-Life, or Team Fortress 2
===============
*/
const char *GetGameDescription()
{
	if ( g_pGameRules ) // this function may be called before the world has spawned, and the game rules initialized
		return g_pGameRules->GetGameDescription();
	else
		return "Vectronic";
}

/*
//-----------------------------------------------------------------------------
// Purpose: Given a player and optional name returns the entity of that 
//			classname that the player is nearest facing
//			
// Input  :
// Output :
//-----------------------------------------------------------------------------
CBaseEntity* FindEntity( edict_t *pEdict, char *classname)
{
	// If no name was given set bits based on the picked
	if (FStrEq(classname,"")) 
	{
		return (FindPickerEntityClass( static_cast<CBasePlayer*>(GetContainingEntity(pEdict)), classname ));
	}
	return NULL;
}
*/

#ifdef SWARM_DLL
//-----------------------------------------------------------------------------
// Purpose: Precache game-specific models & sounds
//-----------------------------------------------------------------------------
PRECACHE_REGISTER_BEGIN( GLOBAL, ClientGamePrecache )
	PRECACHE( MODEL, "models/player.mdl");
	//PRECACHE( MODEL, "models/gibs/agibs.mdl" );
	//PRECACHE( MODEL, "models/gibs/hgibs.mdl" );
	//PRECACHE( MODEL, "models/gibs/hgibs_spine.mdl" );
	//PRECACHE( MODEL, "models/gibs/hgibs_scapula.mdl" );
	
	PRECACHE( GAMESOUND, "Player.Death"  );

	//PRECACHE( GAMESOUND, "FX_AntlionImpact.ShellImpact" );
	//PRECACHE( GAMESOUND, "Missile.ShotDown" );
	PRECACHE( GAMESOUND, "FX_RicochetSound.Ricochet" );
	//PRECACHE( GAMESOUND, "Bullets.GunshipNearmiss" );
	//PRECACHE( GAMESOUND, "Bullets.StriderNearmiss" );

	PRECACHE( GAMESOUND, "Geiger.BeepHigh" );
	PRECACHE( GAMESOUND, "Geiger.BeepLow" );

	// Vectronic
	PRECACHE( GAMESOUND, SOUND_USE_DENY );
	PRECACHE( GAMESOUND, SOUND_USE );
	PRECACHE( GAMESOUND, SOUND_WHOOSH );

	PRECACHE( KV_DEP_FILE, "resource/ParticleEmitters.txt" )
PRECACHE_REGISTER_END()

void ClientGamePrecache( void )
{
}
#else
//-----------------------------------------------------------------------------
// Purpose: Precache game-specific models & sounds
//-----------------------------------------------------------------------------
void ClientGamePrecache( void )
{
	CBaseEntity::PrecacheModel("models/player.mdl");
	CBaseEntity::PrecacheModel( "models/gibs/agibs.mdl" );
	CBaseEntity::PrecacheModel ("models/weapons/v_hands.mdl");

	CBaseEntity::PrecacheScriptSound( "Player.Death" );
	CBaseEntity::PrecacheScriptSound( "Player.Spawn" );
	CBaseEntity::PrecacheScriptSound( "HUDQuickInfo.LowAmmo" );
	CBaseEntity::PrecacheScriptSound( "HUDQuickInfo.LowHealth" );

	CBaseEntity::PrecacheScriptSound( "FX_AntlionImpact.ShellImpact" );
	CBaseEntity::PrecacheScriptSound( "Missile.ShotDown" );
	CBaseEntity::PrecacheScriptSound( "Bullets.DefaultNearmiss" );
	CBaseEntity::PrecacheScriptSound( "Bullets.GunshipNearmiss" );
	CBaseEntity::PrecacheScriptSound( "Bullets.StriderNearmiss" );
	
	CBaseEntity::PrecacheScriptSound( "Geiger.BeepHigh" );
	CBaseEntity::PrecacheScriptSound( "Geiger.BeepLow" );
}
#endif


// called by ClientKill and DeadThink
void respawn( CBaseEntity *pEdict, bool fCopyCorpse )
{
	if (rHGameRules()->IsRespawningEnabled())
	{
		if ( fCopyCorpse )
			dynamic_cast<CBasePlayer*>(pEdict)->CreateCorpse();

		pEdict->Spawn();
		// this /ought/ to be done by CBasePlayer...
		pEdict->RemoveSolidFlags( FSOLID_NOT_SOLID );
	}
	else
	{
		if (gpGlobals->coop || gpGlobals->deathmatch)
		{
			if ( fCopyCorpse )
			{
				// make a copy of the dead body for appearances sake
				((CVectronicPlayer *)pEdict)->CreateCorpse();
			}

			// respawn player
			pEdict->Spawn();
		}
		else
		{       // restart the entire server
			engine->ServerCommand("reload\n");
		}
	}
}

void GameStartFrame( void )
{
	VPROF("GameStartFrame()");
	if ( g_fGameOver )
		return;

	gpGlobals->teamplay = (teamplay.GetInt() != 0);
}

void FinishClientPutInServer( CBasePlayer* pPlayer )
{
}

void ClientFullyConnect( edict_t *pEntity )
{
}

//=========================================================
// instantiate the proper game rules object
//=========================================================
void InstallGameRules()
{
	CreateGameRulesObject( "CVectronicGameRules" );
}