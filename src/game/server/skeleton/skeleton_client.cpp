#include "cbase.h"
#include "tier0/vprof.h"

#define DEFAULT_PLAYER_MODEL "models/skeleton/player.mdl"

extern bool	g_fGameOver;

const char* GetGameDescription()
{
	return "Skeleton";
}

void InstallGameRules()
{
	CreateGameRulesObject( "CSkeletonGameRules" );
}

void ClientGamePrecache()
{
	CBaseEntity::PrecacheModel(DEFAULT_PLAYER_MODEL);
}

void GameStartFrame()
{
	VPROF( "GameStartFrame" );

	if ( g_pGameRules )
		g_pGameRules->Think();

	if ( g_fGameOver )
		return;

//	Uncomment this to add bot support
//	extern void Bot_RunAll();
//	Bot_RunAll();
}

void ClientPutInServer( edict_t* pEdict, const char* playername )
{
	CBasePlayer* pPlayer = CBasePlayer::CreatePlayer( "player", pEdict );
	pPlayer->SetPlayerName( playername );
}

void FinishClientPutInServer( CBasePlayer* pPlayer )
{
	pPlayer->InitialSpawn();
	// Dying without a player model crashes the client
	pPlayer->SetModel(DEFAULT_PLAYER_MODEL);
	pPlayer->Spawn();

	// Security measure: remove any % characters
	char sName[128];
	V_strncpy( sName, pPlayer->GetPlayerName(), sizeof(sName) );
	for ( char* pApersand = sName; pApersand != NULL && *pApersand != 0; pApersand++ )
	{
		if ( *pApersand == '%' )
				*pApersand = ' ';
	}

	UTIL_ClientPrintAll( HUD_PRINTNOTIFY, "%s has joined the game\n", sName[0] != 0 ? sName : "<unconnected>" );
}

void ClientActive( edict_t* pEdict, bool bLoadGame )
{
	CBasePlayer* pPlayer = ToBasePlayer( CBaseEntity::Instance( pEdict ) );
	FinishClientPutInServer( pPlayer );
}

void ClientFullyConnect( edict_t *pEntity )
{
}

// called by ClientKill and DeadThink
void respawn( CBaseEntity* pEdict, bool fCopyCorpse )
{
	if (gpGlobals->coop || gpGlobals->deathmatch)
	{
		if ( fCopyCorpse )
			dynamic_cast<CBasePlayer*>(pEdict)->CreateCorpse();

		pEdict->Spawn();
		// this /ought/ to be done by CBasePlayer...
		pEdict->RemoveSolidFlags( FSOLID_NOT_SOLID );
	}
	else
		engine->ServerCommand("reload\n");
}