#include "cbase.h"
#include "clientmode_skeleton.h"
#include "ivmodemanager.h"
#include "c_weapon__stubs.h"
#include "hud_basechat.h"

ConVar default_fov( "default_fov", "90", FCVAR_CHEAT );

DECLARE_HUDELEMENT (CBaseHudChat);

void ClientModeSkeleton::InitViewport()
{
	m_pViewport = new CBaseViewport();
	m_pViewport->Start( gameuifuncs, gameeventmanager );
	m_pViewport->SetPaintBackgroundEnabled(false);

	char name[32];
	V_snprintf(name,32,"Skeleton viewport (ss slot %i)",engine->GetActiveSplitScreenPlayerSlot());
	m_pViewport->SetName(name);

	// To actually support splitscreen, resize/reposition the viewport
	m_pViewport->FindPanelByName("scores")->ShowPanel(false);
}

void ClientModeSkeletonFullScreen::InitViewport()
{
	m_pViewport = new CSkeletonViewportFullscreen();
	m_pViewport->Start( gameuifuncs, gameeventmanager );
	m_pViewport->SetPaintBackgroundEnabled(false);
	m_pViewport->SetName( "Skeleton viewport (fullscreen)" );
}

/******************
     Accessors
******************/

static IClientMode* g_pClientMode[ MAX_SPLITSCREEN_PLAYERS ]; // Pointer to the active mode
IClientMode* GetClientMode()
{
	ASSERT_LOCAL_PLAYER_RESOLVABLE();
	return g_pClientMode[ engine->GetActiveSplitScreenPlayerSlot() ];
}

ClientModeSkeleton g_ClientModeNormal[ MAX_SPLITSCREEN_PLAYERS ]; // The default mode
IClientMode* GetClientModeNormal()
{
	ASSERT_LOCAL_PLAYER_RESOLVABLE();
	return &g_ClientModeNormal[ engine->GetActiveSplitScreenPlayerSlot() ];
}

static ClientModeSkeletonFullScreen g_FullscreenClientMode; // There in also a singleton fullscreen mode which covers all splitscreen players
IClientMode* GetFullscreenClientMode()
{
	return &g_FullscreenClientMode;
}

/******************
    ModeManager
******************/

class SkeletonModeManager : public IVModeManager
{
	void Init()
	{
		for( int i = 0; i < MAX_SPLITSCREEN_PLAYERS; ++i )
		{
			ACTIVE_SPLITSCREEN_PLAYER_GUARD( i );
			g_pClientMode[ i ] = GetClientModeNormal();
		}
	}
	
	void SwitchMode( bool commander, bool force ) {}

	void LevelInit( const char* newmap )
	{
		for( int i = 0; i < MAX_SPLITSCREEN_PLAYERS; ++i )
		{
			ACTIVE_SPLITSCREEN_PLAYER_GUARD( i );
			GetClientMode()->LevelInit(newmap);
		}
		GetFullscreenClientMode()->LevelInit(newmap);
	}
	void LevelShutdown()
	{
		for( int i = 0; i < MAX_SPLITSCREEN_PLAYERS; ++i )
		{
			ACTIVE_SPLITSCREEN_PLAYER_GUARD( i );
			GetClientMode()->LevelShutdown();
		}
		GetFullscreenClientMode()->LevelShutdown();
	}
};

IVModeManager* modemanager = (IVModeManager*)new SkeletonModeManager;