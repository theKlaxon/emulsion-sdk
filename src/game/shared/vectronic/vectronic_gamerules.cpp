#include "cbase.h"
#include "singleplay_gamerules.h"
#include "vectronic_gamerules.h"
#include "ammodef.h"

#ifdef GAME_DLL
	#include "vectronic_player.h"
	#include "voice_gamemgr.h"
#endif

// ===Ammo/Damage===

//Pistol
ConVar	sk_plr_dmg_pistol			( "sk_plr_dmg_pistol","0", FCVAR_REPLICATED );
ConVar	sk_npc_dmg_pistol			( "sk_npc_dmg_pistol","0", FCVAR_REPLICATED);
ConVar	sk_max_pistol				( "sk_max_pistol","0", FCVAR_REPLICATED);

// shared ammo definition
// JAY: Trying to make a more physical bullet response
#define BULLET_MASS_GRAINS_TO_LB(grains)	(0.002285*(grains)/16.0f)
#define BULLET_MASS_GRAINS_TO_KG(grains)	lbs2kg(BULLET_MASS_GRAINS_TO_LB(grains))

// exaggerate all of the forces, but use real numbers to keep them consistent
#define BULLET_IMPULSE_EXAGGERATION			3.5
// convert a velocity in ft/sec and a mass in grains to an impulse in kg in/s
#define BULLET_IMPULSE(grains, ftpersec)	((ftpersec)*12*BULLET_MASS_GRAINS_TO_KG(grains)*BULLET_IMPULSE_EXAGGERATION)

CAmmoDef* GetAmmoDef()
{
	static CAmmoDef def;
	static bool bInitted = false;
	
	if ( !bInitted )
	{
		bInitted = true;
		def.AddAmmoType("Pistol",	DMG_BULLET,		TRACER_LINE_AND_WHIZ, "sk_plr_dmg_pistol",	"sk_npc_dmg_pistol", "sk_max_pistol", BULLET_IMPULSE(200, 1225), 0 );
	}

	return &def;
}

// =====Convars=============

// This convar is for storing a small bit for the hub.
ConVar	vectronic_onlvl	( "vectronic_onlvl","0", FCVAR_REPLICATED | FCVAR_HIDDEN );

// Colors are used many diffrent ways, we need to seporate the color values
ConVar	vectronic_vecball3_color_r	( "vectronic_vecball3_color_r",				"193",	FCVAR_REPLICATED );
ConVar	vectronic_vecball3_color_g	( "vectronic_vecball3_color_g",				"124",	FCVAR_REPLICATED );
ConVar	vectronic_vecball3_color_b	( "vectronic_vecball3_color_b",				"123",	FCVAR_REPLICATED );

ConVar	vectronic_vecball4_color_r	( "vectronic_vecball4_color_r",				"193", FCVAR_REPLICATED );
ConVar	vectronic_vecball4_color_g	( "vectronic_vecball4_color_g",				"183", FCVAR_REPLICATED );
ConVar	vectronic_vecball4_color_b	( "vectronic_vecball4_color_b",				"123", FCVAR_REPLICATED );

ConVar	vectronic_vecball5_color_r	( "vectronic_vecball5_color_r",				"123",	FCVAR_REPLICATED );
ConVar	vectronic_vecball5_color_g	( "vectronic_vecball5_color_g",				"124",	FCVAR_REPLICATED );
ConVar	vectronic_vecball5_color_b	( "vectronic_vecball5_color_b",				"193",	FCVAR_REPLICATED );

// This is so modders can change ball stuff 3-5.
ConVar	vectronic_vecball3_sprite	( "vectronic_vecball3_sprite",	BALL_SPRITE_DEFAULT, FCVAR_REPLICATED | FCVAR_RELOAD_MATERIALS | FCVAR_RELOAD_TEXTURES );
ConVar	vectronic_vecball4_sprite	( "vectronic_vecball4_sprite",	BALL_SPRITE_DEFAULT, FCVAR_REPLICATED | FCVAR_RELOAD_MATERIALS | FCVAR_RELOAD_TEXTURES );
ConVar	vectronic_vecball5_sprite	( "vectronic_vecball5_sprite",	BALL_SPRITE_DEFAULT, FCVAR_REPLICATED | FCVAR_RELOAD_MATERIALS | FCVAR_RELOAD_TEXTURES );

extern ConVar	vectronic_vecgun_use_custom_balls;
extern ConVar	vectronic_vecgun_ball_lifetime;

// Apply default colors.
void CC_RestoreColors( void )
{
	vectronic_vecball3_color_r.SetValue(193);
	vectronic_vecball3_color_g.SetValue(124);
	vectronic_vecball3_color_b.SetValue(123);

	vectronic_vecball4_color_r.SetValue(193);
	vectronic_vecball4_color_g.SetValue(183);
	vectronic_vecball4_color_b.SetValue(123);

	vectronic_vecball5_color_r.SetValue(123); 
	vectronic_vecball5_color_g.SetValue(124);
	vectronic_vecball5_color_b.SetValue(193);
}

static ConCommand restorecolors("vectronic_restore_custom_colors", CC_RestoreColors, "Restores colors for custom balls.\n", FCVAR_CHEAT );

//=========================================================

// This being required here is a bug. It should be in shared\BaseGrenade_shared.cpp
ConVar sk_plr_dmg_grenade( "sk_plr_dmg_grenade","0");		
#ifdef GAME_DLL
class CVoiceGameMgrHelper : public IVoiceGameMgrHelper
{
public:
	virtual bool	CanPlayerHearPlayer( CBasePlayer* pListener, CBasePlayer* pTalker, bool &bProximity ) { return true; }
};

CVoiceGameMgrHelper g_VoiceGameMgrHelper;
IVoiceGameMgrHelper* g_pVoiceGameMgrHelper = &g_VoiceGameMgrHelper;
#endif

//=========================================================

REGISTER_GAMERULES_CLASS( CVectronicGameRules );
//BEGIN_NETWORK_TABLE_NOBASE( CVectronicGameRules, DT_VectronicGameRules )
//END_NETWORK_TABLE()

LINK_ENTITY_TO_CLASS( vectronic_gamerules, CVectronicGameRulesProxy );

#ifdef GAME_DLL
/*
BEGIN_DATADESC( CVectronicGameRulesProxy )
	DEFINE_KEYFIELD( m_bCustomGun, FIELD_BOOLEAN, "customgun" ),
	// Inputs.
END_DATADESC()
*/

bool CVectronicGameRulesProxy::KeyValue( const char *szKeyName, const char *szValue ) 
{
	if ( FStrEq( szKeyName, "ball3color" ) )
	{
		int color[3];
		V_StringToIntArray( color, 3, szValue );
		m_ball3_r = color[0];
		m_ball3_g = color[1];
		m_ball3_b = color[2];
		rHGameRules()->SetBall3Color(m_ball3_r, m_ball3_g, m_ball3_b);
		return true;
	}
	else if ( FStrEq( szKeyName, "ball4color" ) )
	{
		int color[3];
		V_StringToIntArray( color, 3, szValue );
		m_ball4_r = color[0];
		m_ball4_g = color[1];
		m_ball4_b = color[2];
		rHGameRules()->SetBall4Color(m_ball4_r, m_ball4_g, m_ball4_b);
		return true;
	}
	else if ( FStrEq( szKeyName, "ball5color" ) )
	{
		int color[3];
		V_StringToIntArray( color, 3, szValue );
		m_ball5_r = color[0];
		m_ball5_g = color[1];
		m_ball5_b = color[2];
		rHGameRules()->SetBall5Color(m_ball5_r, m_ball5_g, m_ball5_b);
		return true;
	}
	else if ( FStrEq( szKeyName, "customgun" ) )
	{
		rHGameRules()->SetVecGunSelection(atof(szValue));
		return true;
	}

	else if (FStrEq(szKeyName, "balllife"))
	{
		rHGameRules()->SetBallLife(atof(szValue));
		return true;

	}
	else if (FStrEq(szKeyName, "ballspeed"))
	{
		rHGameRules()->SetBallSpeed(atof(szValue));
		return true;

	}
	else if (FStrEq(szKeyName, "respawn"))
	{
		rHGameRules()->ToggleRespawning(atof(szValue));
		return true;

	}

	/*
	else if ( FStrEq( szKeyName, "ball3sprite" ) )
	{	
		rHGameRules()->SetBall3Sprite(szValue);
	}
	else if ( FStrEq( szKeyName, "ball4sprite" ) )
	{
		rHGameRules()->SetBall4Sprite(szValue);
	}
	else if ( FStrEq( szKeyName, "ball5sprite" ) )
	{
		rHGameRules()->SetBall5Sprite(szValue);
	}
	*/
	else
		return BaseClass::KeyValue( szKeyName, szValue );

	return true;
}	

#endif // GAME_DLL

void InitBodyQue() { }

//=========================================================
//=========================================================
CVectronicGameRules::CVectronicGameRules()
{
	// Force values if we don't have a manager.
	if (vectronic_vecball3_sprite.GetString() == NULL )
	{
#ifdef CLIENT_DLL
		vectronic_vecball3_sprite.SetValue(BALL_SPRITE_DEFAULT);
#endif	
	}

	if (vectronic_vecball4_sprite.GetString() == NULL )
	{
#ifdef CLIENT_DLL
		vectronic_vecball4_sprite.SetValue(BALL_SPRITE_DEFAULT);
#endif	
	}

	if (vectronic_vecball5_sprite.GetString() == NULL )
	{
#ifdef CLIENT_DLL
		vectronic_vecball5_sprite.SetValue(BALL_SPRITE_DEFAULT);
#endif	
	}

#ifndef CLIENT_DLL
	m_bRespawnPlayer = true;
	vectronic_vecgun_use_custom_balls.SetValue(0);
	m_flBallLife = 1;
	m_flBallSpeed = 1000.0f;
#endif	

}

bool CVectronicGameRules::IsMultiplayer( void )
{
	return false;
}

void CVectronicGameRules::PlayerThink( CBasePlayer *pPlayer )
{
}

#ifdef GAME_DLL

// -------------------------------------------------------------------------------- //
/*
Vector DropToGround( 
	CBaseEntity *pMainEnt, 
	const Vector &vPos, 
	const Vector &vMins, 
	const Vector &vMaxs )
{
	trace_t trace;
	UTIL_TraceHull( vPos, vPos + Vector( 0, 0, -500 ), vMins, vMaxs, MASK_SOLID, pMainEnt, COLLISION_GROUP_NONE, &trace );
	return trace.endpos;
}
*/

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CBaseEntity *CVectronicGameRules::GetPlayerSpawnSpot( CBasePlayer *pPlayer )
{
/*
	// get valid spawn point
	CBaseEntity *pSpawnSpot = pPlayer->EntSelectSpawnPoint();

	// drop down to ground
	Vector GroundPos = DropToGround( pPlayer, pSpawnSpot->GetAbsOrigin(), VEC_HULL_MIN, VEC_HULL_MAX );

	// Move the player to the place it said.
	pPlayer->SetLocalOrigin( GroundPos + Vector(0,0,1) );
	pPlayer->SetAbsVelocity( vec3_origin );
	pPlayer->SetLocalAngles( pSpawnSpot->GetLocalAngles() );
	pPlayer->m_Local.m_vecPunchAngle = vec3_angle;
	pPlayer->m_Local.m_vecPunchAngleVel = vec3_angle;
	pPlayer->SnapEyeAngles( pSpawnSpot->GetLocalAngles() );

	return pSpawnSpot;

*/

	CBaseEntity *pSpawnSpot = pPlayer->EntSelectSpawnPoint();
	Assert( pSpawnSpot );
	if ( pSpawnSpot == NULL )
		return NULL;

	pPlayer->SetLocalOrigin( pSpawnSpot->GetAbsOrigin() + Vector(0,0,1) );
	pPlayer->SetAbsVelocity( vec3_origin );
	pPlayer->SetLocalAngles( pSpawnSpot->GetLocalAngles() );
	pPlayer->m_Local.m_vecPunchAngle = vec3_angle;
	pPlayer->m_Local.m_vecPunchAngleVel = vec3_angle;
	pPlayer->SnapEyeAngles( pSpawnSpot->GetLocalAngles() );

	return pSpawnSpot;
}

//-----------------------------------------------------------------------------
// Purpose: Checks to see if the player is on the correct team and whether or
//          not the spawn point is available.
//-----------------------------------------------------------------------------
bool CVectronicGameRules::IsSpawnPointValid( CBaseEntity *pSpot, CBasePlayer *pPlayer, bool bIgnorePlayers )
{
	// Check the team.
	//if ( pSpot->GetTeamNumber() != pPlayer->GetTeamNumber() )
	//	return false;

	if ( !pSpot->IsTriggered( pPlayer ) )
		return false;

	CVectronicSpawn *pSpawn = dynamic_cast<CVectronicSpawn*>( pSpot );
	if ( pSpawn )
	{
		if ( pSpawn->IsDisabled() )
			return false;
	}

	Vector mins = GetViewVectors()->m_vHullMin;
	Vector maxs = GetViewVectors()->m_vHullMax;

	if ( !bIgnorePlayers )
	{
		Vector vTestMins = pSpot->GetAbsOrigin() + mins;
		Vector vTestMaxs = pSpot->GetAbsOrigin() + maxs;
		return UTIL_IsSpaceEmpty( pPlayer, vTestMins, vTestMaxs );
	}

	trace_t trace;
	UTIL_TraceHull( pSpot->GetAbsOrigin(), pSpot->GetAbsOrigin(), mins, maxs, MASK_PLAYERSOLID, pPlayer, COLLISION_GROUP_PLAYER_MOVEMENT, &trace );
	return ( trace.fraction == 1 && trace.allsolid != 1 && (trace.startsolid != 1) );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVectronicGameRules::Activate()
{
}

#ifdef GAME_DLL
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVectronicGameRules::ToggleRespawning(int b)
{
	m_bRespawnPlayer = b;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVectronicGameRules::SetBall3Color( int r, int g, int b )
{
	vectronic_vecball3_color_r.SetValue(r);
	vectronic_vecball3_color_g.SetValue(g);
	vectronic_vecball3_color_b.SetValue(b);
}

void CVectronicGameRules::SetBall4Color( int r, int g, int b )
{
	vectronic_vecball4_color_r.SetValue(r);
	vectronic_vecball4_color_g.SetValue(g);
	vectronic_vecball4_color_b.SetValue(b);
}

void CVectronicGameRules::SetBall5Color( int r, int g, int b )
{
	vectronic_vecball5_color_r.SetValue(r);
	vectronic_vecball5_color_g.SetValue(g);
	vectronic_vecball5_color_b.SetValue(b);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVectronicGameRules::SetVecGunSelection( int value )
{
	vectronic_vecgun_use_custom_balls.SetValue(value);
}

#endif

void CVectronicGameRules::SetBall3Sprite(const char *szTexture)
{
#ifdef CLIENT_DLL
	vectronic_vecball3_sprite.SetValue(szTexture);
#endif
}

void CVectronicGameRules::SetBall4Sprite(const char *szTexture)
{
#ifdef CLIENT_DLL
	vectronic_vecball4_sprite.SetValue(szTexture);
#endif
}

void CVectronicGameRules::SetBall5Sprite(const char *szTexture)
{
#ifdef CLIENT_DLL
	vectronic_vecball5_sprite.SetValue(szTexture);
#endif
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CVectronicGameRules::ShouldCollide( int collisionGroup0, int collisionGroup1 )
{
	// The smaller number is always first
	if ( collisionGroup0 > collisionGroup1 )
	{
		// swap so that lowest is always first
		int tmp = collisionGroup0;
		collisionGroup0 = collisionGroup1;
		collisionGroup1 = tmp;
	}

	if ( collisionGroup0 == COLLISION_GROUP_PLAYER_MOVEMENT )
	{
		collisionGroup0 = COLLISION_GROUP_PLAYER;
	}

	if( collisionGroup1 == COLLISION_GROUP_PLAYER_MOVEMENT )
	{
		collisionGroup1 = COLLISION_GROUP_PLAYER;
	}

	//If collisionGroup0 is not a player then NPC_ACTOR behaves just like an NPC.
	if ( collisionGroup1 == COLLISION_GROUP_NPC_ACTOR && collisionGroup0 != COLLISION_GROUP_PLAYER )
	{
		collisionGroup1 = COLLISION_GROUP_NPC;
	}

	//players don't collide against NPC Actors.
	//I could've done this up where I check if collisionGroup0 is NOT a player but I decided to just
	//do what the other checks are doing in this function for consistency sake.
	if ( collisionGroup1 == COLLISION_GROUP_NPC_ACTOR && collisionGroup0 == COLLISION_GROUP_PLAYER )
		return false;
		
	// In cases where NPCs are playing a script which causes them to interpenetrate while riding on another entity,
	// such as a train or elevator, you need to disable collisions between the actors so the mover can move them.
	if ( collisionGroup0 == COLLISION_GROUP_NPC_SCRIPTED && collisionGroup1 == COLLISION_GROUP_NPC_SCRIPTED )
		return false;

	if ( collisionGroup0 == COLLISION_GROUP_PROJECTILE )
	{
		if ( collisionGroup1 == COLLISION_GROUP_PROJECTILE)
			return false;
	}

	if ( ( collisionGroup0 == COLLISION_GROUP_WEAPON ) ||
		( collisionGroup0 == COLLISION_GROUP_PLAYER ) ||
		( collisionGroup0 == COLLISION_GROUP_PROJECTILE ) )
	{
		if ( collisionGroup1 == COLLISION_GROUP_PROJECTILE ) //COLLISION_GROUP_VECTRONIC_BALL
			return false;
	}

	if ( collisionGroup0 == COLLISION_GROUP_DEBRIS )
	{
		if ( collisionGroup1 == COLLISION_GROUP_PROJECTILE )
			return true;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVectronicGameRules::InitDefaultAIRelationships( void )
{
	int i, j;

	//  Allocate memory for default relationships
	CBaseCombatCharacter::AllocateDefaultRelationships();

	// --------------------------------------------------------------
	// First initialize table so we can report missing relationships
	// --------------------------------------------------------------
	for (i=0;i<NUM_AI_CLASSES;i++)
	{
		for (j=0;j<NUM_AI_CLASSES;j++)
		{
			// By default all relationships are neutral of priority zero
			CBaseCombatCharacter::SetDefaultRelationship( (Class_T)i, (Class_T)j, D_NU, 0 );
		}
	}

		// ------------------------------------------------------------
		//	> CLASS_PLAYER
		// ------------------------------------------------------------
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER,			CLASS_NONE,				D_NU, 0);			
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER,			CLASS_PLAYER,			D_NU, 0);			
}

	const char* CVectronicGameRules::AIClassText(int classType)
	{
		switch (classType)
		{
			case CLASS_NONE:			return "CLASS_NONE";
			case CLASS_PLAYER:			return "CLASS_PLAYER";
			default:					return "MISSING CLASS in ClassifyText()";
		}
	}
#endif // #ifdef GAME_DLL
