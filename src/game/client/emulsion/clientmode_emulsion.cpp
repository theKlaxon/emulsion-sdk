#include "cbase.h"
#include "clientmode_emulsion.h"
#include "ivmodemanager.h"
#include "c_weapon__stubs.h"
#include "hud_basechat.h"
#include "iinput.h"
#include "dll_patch.h"

ConVar default_fov( "default_fov", "90", FCVAR_CHEAT );

DECLARE_HUDELEMENT (CBaseHudChat);

// sometimes better than messing with an autoexec or a stupid default config file
static void OverrideDefaultConVars() {
	engine->ClientCmd_Unrestricted("r_shadowrendertotexture 1");
}

void ClientModeEmulsion::InitViewport()
{
	m_pViewport = new CBaseViewport();
	m_pViewport->Start( gameuifuncs, gameeventmanager );
	m_pViewport->SetPaintBackgroundEnabled(false);

	char name[32];
	V_snprintf(name,32,"Emulsion viewport (ss slot %i)", engine->GetActiveSplitScreenPlayerSlot());
	DevMsg("\nEmulsion viewport (ss slot %i)", engine->GetActiveSplitScreenPlayerSlot());
	m_pViewport->SetName(name);

	// To actually support splitscreen, resize/reposition the viewport
	m_pViewport->FindPanelByName("scores")->ShowPanel(false);

	OverrideDefaultConVars();
}

void ClientModeEmulsionFullScreen::InitViewport()
{
	m_pViewport = new CEmulsionViewportFullscreen();
	m_pViewport->Start( gameuifuncs, gameeventmanager );
	m_pViewport->SetPaintBackgroundEnabled(false);
	m_pViewport->SetName( "Emulsion viewport (fullscreen)" );

	OverrideDefaultConVars();
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

ClientModeEmulsion g_ClientModeNormal[ MAX_SPLITSCREEN_PLAYERS ]; // The default mode
IClientMode* GetClientModeNormal()
{
	ASSERT_LOCAL_PLAYER_RESOLVABLE();
	return &g_ClientModeNormal[ engine->GetActiveSplitScreenPlayerSlot() ];
}

static ClientModeEmulsionFullScreen g_FullscreenClientMode; // There in also a singleton fullscreen mode which covers all splitscreen players
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

		engine->ClientCmd("r_flashlightbrightness 2");

		// patch the dlls
		PatchAll();
	}
	void LevelShutdown()
	{
		UnPatchAll();

		for( int i = 0; i < MAX_SPLITSCREEN_PLAYERS; ++i )
		{
			ACTIVE_SPLITSCREEN_PLAYER_GUARD( i );
			GetClientMode()->LevelShutdown();
		}
		GetFullscreenClientMode()->LevelShutdown();
	}
};

IVModeManager* modemanager = (IVModeManager*)new SkeletonModeManager;