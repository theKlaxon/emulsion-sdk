#include "cbase.h"
#include "basemodpanel.h"
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
#include "vgui/ILocalize.h"
#include "swarm/basemodframe.h"

// emulsion ui headers
//#include "vmainmenu.h"
//

using namespace BaseModUI;
using namespace vgui;

ConVar ui_gameui_debug("ui_gameui_debug", "0", FCVAR_RELEASE);

int UI_IsDebug()
{
	return (*(int*)(&ui_gameui_debug)) ? ui_gameui_debug.GetInt() : 0;
}

CBaseModPanel* CBaseModPanel::m_CFactoryBasePanel = 0;

CBaseModPanel::CBaseModPanel(): BaseClass(0, "CBaseModPanel"),
	m_bClosingAllWindows( false ),
	m_lastActiveUserId( 0 )
{
	

#if !defined( NO_STEAM )
	// Set Steam overlay position
	if (steamapicontext && steamapicontext->SteamUtils())
	{
		steamapicontext->SteamUtils()->SetOverlayNotificationPosition(k_EPositionTopRight);
	}
#endif

	g_pVGuiLocalize->AddFile("Resource/basemodui_%language%.txt");
	m_CFactoryBasePanel = this;

	m_LevelLoading = false;

	for (int k = 0; k < WPRI_COUNT; ++k)
	{
		m_ActiveWindow[k] = WT_NONE;
	}

	// delay 3 frames before doing activation on initialization
	// needed to allow engine to exec startup commands (background map signal is 1 frame behind) 
	m_DelayActivation = 3;
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
//
////=============================================================================
//CBaseModFrame* CBaseModPanel::OpenWindow(const WINDOW_TYPE& wt, CBaseModFrame* caller, bool hidePrevious, KeyValues* pParameters)
//{
//	CBaseModFrame* newNav = m_Frames[wt].Get();
//	bool setActiveWindow = true;
//
//	// Window priority is used to track which windows are visible at all times
//	// it is used to resolve the situations when a game requests an error box to popup
//	// while a loading progress bar is active.
//	// Windows with a higher priority force all other windows to get hidden.
//	// After the high-priority window goes away it falls back to restore the low priority windows.
//	WINDOW_PRIORITY nWindowPriority = WPRI_NORMAL;
//
//	switch (wt)
//	{
//	case WT_PASSWORDENTRY:
//		setActiveWindow = false;
//		break;
//	}
//
//	switch (wt)
//	{
//	case WT_GENERICWAITSCREEN:
//		nWindowPriority = WPRI_WAITSCREEN;
//		break;
//	case WT_GENERICCONFIRMATION:
//		nWindowPriority = WPRI_MESSAGE;
//		break;
//	case WT_LOADINGPROGRESSBKGND:
//		nWindowPriority = WPRI_BKGNDSCREEN;
//		break;
//	case WT_LOADINGPROGRESS:
//		nWindowPriority = WPRI_LOADINGPLAQUE;
//		break;
//	case WT_PASSWORDENTRY:
//		nWindowPriority = WPRI_TOPMOST;
//		break;
//	case WT_TRANSITIONSCREEN:
//		nWindowPriority = WPRI_TOPMOST;
//		break;
//	}
//
//	if (!newNav)
//	{
//		switch (wt)
//		{
//		case WT_ACHIEVEMENTS:
//			//m_Frames[wt] = new Achievements(this, "Achievements");
//			break;
//
//		case WT_AUDIO:
//			//m_Frames[wt] = new Audio(this, "Audio");
//			break;
//
//		case WT_AUDIOVIDEO:
//			//m_Frames[wt] = new AudioVideo(this, "AudioVideo");
//			break;
//
//		case WT_CLOUD:
//			break;
//
//		case WT_CONTROLLER:
//			//m_Frames[wt] = new ControllerOptions(this, "ControllerOptions");
//			break;
//
//		case WT_CONTROLLER_STICKS:
//			//m_Frames[wt] = new ControllerOptionsSticks(this, "ControllerOptionsSticks");
//			break;
//
//		case WT_CONTROLLER_BUTTONS:
//			//m_Frames[wt] = new ControllerOptionsButtons(this, "ControllerOptionsButtons");
//			break;
//
//		case WT_DOWNLOADS:
//			break;
//
//		case WT_GAMELOBBY:
//			break;
//
//		case WT_GAMEOPTIONS:
//			//m_Frames[wt] = new GameOptions(this, "GameOptions");
//			break;
//
//		case WT_GAMESETTINGS:
//			//m_Frames[wt] = new GameSettings(this, "GameSettings");
//			break;
//
//		case WT_GENERICCONFIRMATION:
//			//m_Frames[wt] = new GenericConfirmation(this, "GenericConfirmation");
//			break;
//
//		case WT_INGAMEDIFFICULTYSELECT:
//			//m_Frames[wt] = new InGameDifficultySelect(this, "InGameDifficultySelect");
//			break;
//
//		case WT_INGAMEMAINMENU:
//			//m_Frames[wt] = new InGameMainMenu(this, "InGameMainMenu");
//			break;
//
//		case WT_INGAMECHAPTERSELECT:
//			//m_Frames[wt] = new InGameChapterSelect(this, "InGameChapterSelect");
//			break;
//
//		case WT_INGAMEKICKPLAYERLIST:
//			break;
//
//		case WT_VOTEOPTIONS:
//			break;
//
//		case WT_KEYBOARDMOUSE:
//			//m_Frames[wt] = new VKeyboard(this, "VKeyboard");
//			break;
//
//		case WT_LOADINGPROGRESSBKGND:
//			//m_Frames[wt] = new LoadingProgress(this, "LoadingProgress", LoadingProgress::LWT_BKGNDSCREEN);
//			break;
//
//		case WT_LOADINGPROGRESS:
//			//m_Frames[wt] = new LoadingProgress(this, "LoadingProgress", LoadingProgress::LWT_LOADINGPLAQUE);
//			break;
//
//		case WT_MAINMENU:
//			//m_Frames[wt] = new MainMenu(this, "MainMenu"); // TODO: FIRST
//			break;
//
//		case WT_MULTIPLAYER:
//			//m_Frames[wt] = new Multiplayer(this, "Multiplayer");
//			break;
//
//		case WT_OPTIONS:
//			//m_Frames[wt] = new Options(this, "Options");
//			break;
//
//		case WT_SIGNINDIALOG:
//			//m_Frames[wt] = new SignInDialog(this, "SignInDialog");
//			break;
//
//		case WT_GENERICWAITSCREEN:
//			//m_Frames[wt] = new GenericWaitScreen(this, "GenericWaitScreen");
//			break;
//
//		case WT_PASSWORDENTRY:
//			//m_Frames[wt] = new PasswordEntry(this, "PasswordEntry");
//			break;
//
//		case WT_ATTRACTSCREEN:
//			//m_Frames[wt] = new CAttractScreen(this, "AttractScreen");
//			break;
//
//		case WT_ALLGAMESEARCHRESULTS:
//			//m_Frames[wt] = new FoundGames(this, "FoundGames");
//			break;
//
//		case WT_FOUNDPUBLICGAMES:
//			//m_Frames[wt] = new FoundPublicGames(this, "FoundPublicGames");
//			break;
//
//		case WT_TRANSITIONSCREEN:
//			//m_Frames[wt] = new CTransitionScreen(this, "TransitionScreen");
//			break;
//
//		case WT_VIDEO:
//			//m_Frames[wt] = new Video(this, "Video");
//			break;
//
//		case WT_STEAMCLOUDCONFIRM:
//			break;
//
//		case WT_STEAMGROUPSERVERS:
//			break;
//
//		case WT_CUSTOMCAMPAIGNS:
//			break;
//
//		case WT_DOWNLOADCAMPAIGN:
//			break;
//
//		case WT_LEADERBOARD:
//			break;
//
//		case WT_ADDONS:
//			break;
//
//		case WT_JUKEBOX:
//			break;
//
//		case WT_ADDONASSOCIATION:
//			break;
//
//		case WT_GETLEGACYDATA:
//			break;
//
//		default:
//			Assert(false);	// unknown window type
//			break;
//		}
//
//		//
//		// Finish setting up the window
//		//
//
//		newNav = m_Frames[wt].Get();
//		if (!newNav)
//			return NULL;
//
//		newNav->SetWindowPriority(nWindowPriority);
//		newNav->SetWindowType(wt);
//		newNav->SetVisible(false);
//	}
//
//	newNav->SetDataSettings(pParameters);
//
//	if (setActiveWindow)
//	{
//		m_ActiveWindow[nWindowPriority] = wt;
//		newNav->AddActionSignalTarget(this);
//		newNav->SetCanBeActiveWindowType(true);
//	}
//	else if (nWindowPriority == WPRI_MESSAGE)
//	{
//		m_ActiveWindow[nWindowPriority] = wt;
//	}
//
//	//
//	// Now the window has been created, set it up
//	//
//
//	if (UI_IsDebug() && (wt != WT_LOADINGPROGRESS))
//	{
//		Msg("[GAMEUI] OnOpen( `%s`, caller = `%s`, hidePrev = %d, setActive = %d, wt=%d, wpri=%d )\n",
//			newNav->GetName(), caller ? caller->GetName() : "<NULL>", int(hidePrevious),
//			int(setActiveWindow), wt, nWindowPriority);
//		KeyValuesDumpAsDevMsg(pParameters, 1);
//	}
//
//	newNav->SetNavBack(caller);
//
//	if (hidePrevious && caller != 0)
//	{
//		caller->SetVisible(false);
//	}
//	else if (caller != 0)
//	{
//		caller->FindAndSetActiveControl();
//		//caller->SetAlpha(128);
//	}
//
//	// Check if a higher priority window is open
//	if (GetActiveWindowPriority() > newNav->GetWindowPriority())
//	{
//		if (UI_IsDebug())
//		{
//			CBaseModFrame* pOther = m_Frames[GetActiveWindowType()].Get();
//			Warning("[GAMEUI] OpenWindow: Another window %p`%s` is having priority %d, deferring `%s`!\n",
//				pOther, pOther ? pOther->GetName() : "<<null>>",
//				GetActiveWindowPriority(), newNav->GetName());
//		}
//
//		// There's a higher priority window that was open at the moment,
//		// hide our window for now, it will get restored later.
//		// newNav->SetVisible( false );
//	}
//	else
//	{
//		newNav->InvalidateLayout(false, false);
//		newNav->OnOpen();
//	}
//
//	if (UI_IsDebug() && (wt != WT_LOADINGPROGRESS))
//	{
//		DbgShowCurrentUIState();
//	}
//
//	return newNav;
//}