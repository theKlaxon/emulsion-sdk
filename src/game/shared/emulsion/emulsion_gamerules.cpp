#include "cbase.h"
#include "singleplay_gamerules.h"
#include "ammodef.h"

#ifdef GAME_DLL
	#include "voice_gamemgr.h"
#endif

#ifdef CLIENT_DLL
	#define C_EmulsionGameRules CEmulsionGameRules
#endif

class CEmulsionGameRules : public CSingleplayRules
{
	DECLARE_CLASS(CEmulsionGameRules, CSingleplayRules);
public:

	virtual bool IsMultiplayer(void) { return false; }

	virtual void PlayerSpawn(CBasePlayer* pPlayer) {}
	virtual void PlayerThink(CBasePlayer* pPlayer) {}
	virtual bool ShouldCollide(int collisionGroup0, int collisionGroup1);

};

REGISTER_GAMERULES_CLASS(CEmulsionGameRules);

bool CEmulsionGameRules::ShouldCollide(int collisionGroup0, int collisionGroup1)
{
	if (collisionGroup0 > collisionGroup1)
	{
		// swap so that lowest is always first
		V_swap(collisionGroup0, collisionGroup1);
	}

	return BaseClass::ShouldCollide(collisionGroup0, collisionGroup1);
}


void InitBodyQue() { }

CAmmoDef* GetAmmoDef()
{
	static CAmmoDef ammo;
	
	static bool bInitialising = true;
	if ( bInitialising )
	{
		ammo.AddAmmoType("pistol",DMG_BULLET,TRACER_RAIL,200,200,180,30,0);
		bInitialising = false;
	}

	return &ammo;
}

#ifdef GAME_DLL

// This being required here is a bug. It should be in shared\BaseGrenade_shared.cpp
ConVar sk_plr_dmg_grenade( "sk_plr_dmg_grenade","0");		

class CVoiceGameMgrHelper : public IVoiceGameMgrHelper
{
public:
	virtual bool	CanPlayerHearPlayer( CBasePlayer* pListener, CBasePlayer* pTalker, bool &bProximity ) { return true; }
};

CVoiceGameMgrHelper g_VoiceGameMgrHelper;
IVoiceGameMgrHelper* g_pVoiceGameMgrHelper = &g_VoiceGameMgrHelper;

#endif // #ifdef GAME_DLL
