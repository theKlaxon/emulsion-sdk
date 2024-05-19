//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Draws the normal TF2 or HL2 HUD.
//
//=============================================================================
#include "cbase.h"
#include "clientmode_vectronic.h"
#include "clientmode_shared.h"
#include "ivmodemanager.h"
#include "panelmetaclassmgr.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ConVar fov_desired( "fov_desired", "75", FCVAR_ARCHIVE | FCVAR_USERINFO, "Sets the base field-of-view.", true, 75.0, true, 90.0 );
ConVar default_fov( "default_fov", "90", FCVAR_CHEAT );

#if defined( GLOWS_ENABLE )
#include "clienteffectprecachesystem.h"
CLIENTEFFECT_REGISTER_BEGIN( PrecachePostProcessingEffectsGlow )
CLIENTEFFECT_MATERIAL( "dev/glow_color" )
CLIENTEFFECT_MATERIAL( "dev/halo_add_to_screen" )
CLIENTEFFECT_REGISTER_END_CONDITIONAL( engine->GetDXSupportLevel() >= 90 )
#endif

// Instance the singleton and expose the interface to it.
IClientMode *GetClientModeNormal()
{
	static ClientModeSDKNormal g_ClientModeNormal;
	return &g_ClientModeNormal;
}

//-----------------------------------------------------------------------------
// Purpose: this is the viewport that contains all the hud elements
//-----------------------------------------------------------------------------
class CHudViewport : public CBaseViewport
{
private:
	DECLARE_CLASS_SIMPLE( CHudViewport, CBaseViewport );

protected:
	virtual void ApplySchemeSettings( vgui::IScheme *pScheme )
	{
		BaseClass::ApplySchemeSettings( pScheme );

		gHUD.InitColors( pScheme );

		SetPaintBackgroundEnabled( false );
	}

	virtual void CreateDefaultPanels( void ) { /* don't create any panels yet*/ };
};

//-----------------------------------------------------------------------------
// ClientModeHLNormal implementation
//-----------------------------------------------------------------------------
ClientModeSDKNormal::ClientModeSDKNormal()
{
	m_pViewport = new CHudViewport();
	m_pViewport->Start( gameuifuncs, gameeventmanager );
}

//-----------------------------------------------------------------------------
// Purpose: Use of glow render effect.
//-----------------------------------------------------------------------------
bool ClientModeSDKNormal::DoPostScreenSpaceEffects( const CViewSetup *pSetup )
{
	#if defined( GLOWS_ENABLE )
		g_GlowObjectManager.RenderGlowEffects( pSetup, 0 );
	#endif

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
ClientModeSDKNormal::~ClientModeSDKNormal()
{
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void ClientModeSDKNormal::Init()
{
	BaseClass::Init();
}

// The current client mode. Always ClientModeNormal in HL.
IClientMode *g_pClientMode = NULL;

#define SCREEN_FILE		"scripts/vgui_screens.txt"

class CSDKModeManager : public IVModeManager
{
public:
				CSDKModeManager( void );
	virtual		~CSDKModeManager( void );

	virtual void	Init( void );
	virtual void	SwitchMode( bool commander, bool force );
	virtual void	OverrideView( CViewSetup *pSetup );
	virtual void	CreateMove( float flInputSampleTime, CUserCmd *cmd );
	virtual void	LevelInit( const char *newmap );
	virtual void	LevelShutdown( void );
};

CSDKModeManager::CSDKModeManager( void )
{
}

CSDKModeManager::~CSDKModeManager( void )
{
}

void CSDKModeManager::Init( void )
{
	g_pClientMode = GetClientModeNormal();
	PanelMetaClassMgr()->LoadMetaClassDefinitionFile( SCREEN_FILE );
}

void CSDKModeManager::SwitchMode( bool commander, bool force )
{
}

void CSDKModeManager::OverrideView( CViewSetup *pSetup )
{
}

void CSDKModeManager::CreateMove( float flInputSampleTime, CUserCmd *cmd )
{
}

void CSDKModeManager::LevelInit( const char *newmap )
{
	g_pClientMode->LevelInit( newmap );
}

void CSDKModeManager::LevelShutdown( void )
{
	g_pClientMode->LevelShutdown();
}


static CSDKModeManager g_SDKModeManager;
IVModeManager *modemanager = &g_SDKModeManager;

