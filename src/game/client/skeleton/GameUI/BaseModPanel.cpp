#include "cbase.h"
#include "gameui/swarm/basemodpanel.h"
#include "./GameUI/IGameUI.h"
#include "ienginevgui.h"
#include "engine/ienginesound.h"
#include "EngineInterface.h"
#include "tier0/dbg.h"
#include "ixboxsystem.h"
#include "GameUI_Interface.h"
#include "game/client/IGameClientExports.h"
#include "gameui/igameconsole.h"
#include "inputsystem/iinputsystem.h"
#include "FileSystem.h"
#include "filesystem/IXboxInstaller.h"
#include "tier2/renderutils.h"
#include "steam/steam_api.h"

using namespace BaseModUI;
using namespace vgui;

CBaseModPanel* CBaseModPanel::m_CFactoryBasePanel = 0;

CBaseModPanel::CBaseModPanel(): BaseClass(0, "CBaseModPanel"),
	m_bClosingAllWindows( false ),
	m_lastActiveUserId( 0 )
{
	m_CFactoryBasePanel = this;

#if !defined( NO_STEAM )
	// Set Steam overlay position
	if (steamapicontext && steamapicontext->SteamUtils())
	{
		steamapicontext->SteamUtils()->SetOverlayNotificationPosition(k_EPositionTopRight);
	}
#endif

}

CBaseModPanel::~CBaseModPanel()
{
}

CBaseModPanel& CBaseModPanel::GetSingleton()
{
	Assert(m_CFactoryBasePanel != 0);
	return *m_CFactoryBasePanel;
}

void CBaseModPanel::ApplySchemeSettings(IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);
}

void CBaseModPanel::RunFrame()
{
}

void CBaseModPanel::PaintBackground()
{
}

void CBaseModPanel::CloseAllWindows( int ePolicyFlags )
{
}

void CBaseModPanel::OnCommand(const char *command)
{
	BaseClass::OnCommand( command );
}

void CBaseModPanel::OnSetFocus()
{
	BaseClass::OnSetFocus();
}

void CBaseModPanel::OnGameUIActivated()
{
	if ( m_DelayActivation )
		return;

	SetVisible(true);
}

void CBaseModPanel::OnGameUIHidden()
{
	SetVisible(false);
}

void CBaseModPanel::OnLevelLoadingStarted( char const *levelName, bool bShowProgressDialog )
{
	CloseAllWindows();
}

bool CBaseModPanel::UpdateProgressBar( float progress, const char *statusText )
{
	return false;
}

void CBaseModPanel::OnNavigateTo( const char* panelName )
{
}

void CBaseModPanel::OnMovedPopupToFront()
{
}

void CBaseModPanel::OnEvent( KeyValues *pEvent )
{
}