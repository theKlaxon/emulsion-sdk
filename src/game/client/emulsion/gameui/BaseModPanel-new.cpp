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
#include "basemodframe.h"
#include "vfooterpanel.h"
#include "VGuiMatSurface/IMatSystemSurface.h"
#include "gameconsole.h"

// emulsion ui headers
#include "vmainmenu.h"
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

	// set the UI scheme
	m_UIScheme = vgui::scheme()->LoadSchemeFromFileEx(0, "resource/swarmscheme.res", "SwarmScheme");
	SetScheme(m_UIScheme);

	// Only one user on the PC, so set it now
	SetLastActiveUserId(IsPC() ? 0 : -1);

	IScheme* pScheme = vgui::scheme()->GetIScheme(m_UIScheme);
	m_hDefaultFont = pScheme->GetFont("Default", true);
	vgui::surface()->PrecacheFontCharacters(m_hDefaultFont, NULL);
	//vgui::surface()->PrecacheFontCharacters(pScheme->GetFont("DefaultBold", true), NULL);
	//vgui::surface()->PrecacheFontCharacters(pScheme->GetFont("DefaultLarge", true), NULL);
	//vgui::surface()->PrecacheFontCharacters(pScheme->GetFont("FrameTitle", true), NULL);

	vgui::surface()->PrecacheFontCharacters(pScheme->GetFont("DefaultSmall", true), NULL);
	vgui::surface()->PrecacheFontCharacters(pScheme->GetFont("Default", true), NULL);
	vgui::surface()->PrecacheFontCharacters(pScheme->GetFont("FrameTitle", true), NULL);

	m_FooterPanel = new CBaseModFooterPanel(this, "FooterPanel");
	m_hOptionsDialog = NULL;

	m_bWarmRestartMode = false;
	m_ExitingFrameCount = 0;

	m_flBlurScale = 0;
	m_flLastBlurTime = 0;

	m_iBackgroundImageID = -1;
	m_iProductImageID = -1;

	m_backgroundMusic = "Misc.MainUI";
	m_nBackgroundMusicGUID = 0;

	m_nProductImageWide = 0;
	m_nProductImageTall = 0;
	m_flMovieFadeInTime = 0.0f;
	m_pBackgroundMaterial = NULL;
	m_pBackgroundTexture = NULL;

	// Subscribe to event notifications
	g_pMatchFramework->GetEventsSubscription()->Subscribe(this);
}

CBaseModPanel::~CBaseModPanel()
{
	//ReleaseStartupGraphic();

	// Unsubscribe from event notifications
	g_pMatchFramework->GetEventsSubscription()->Unsubscribe(this);

	if (m_FooterPanel)
	{
		//		delete m_FooterPanel;
		m_FooterPanel->MarkForDeletion();
		m_FooterPanel = NULL;
	}

	Assert(m_CFactoryBasePanel == this);
	m_CFactoryBasePanel = 0;

	surface()->DestroyTextureID(m_iBackgroundImageID);
	surface()->DestroyTextureID(m_iProductImageID);

	// Shutdown UI game data
	//CUIGameData::Shutdown();
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
	if (s_NavLock > 0)
	{
		--s_NavLock;
	}

	//GetAnimationController()->UpdateAnimations(Plat_FloatTime());

	CBaseModFrame::RunFrameOnListeners();

	CUIGameData::Get()->RunFrame();

	if (m_DelayActivation)
	{
		m_DelayActivation--;
		if (!m_LevelLoading && !m_DelayActivation)
		{
			if (UI_IsDebug())
			{
				Msg("[GAMEUI] Executing delayed UI activation\n");
			}
			OnGameUIActivated();
		}
	}

	bool bDoBlur = true;
	WINDOW_TYPE wt = GetActiveWindowType();
	switch (wt)
	{
	case WT_NONE:
	case WT_MAINMENU:
	case WT_LOADINGPROGRESSBKGND:
	case WT_LOADINGPROGRESS:
	case WT_AUDIOVIDEO:
		bDoBlur = false;
		break;
	}
	if (GetWindow(WT_ATTRACTSCREEN) || (enginevguifuncs && !enginevguifuncs->IsGameUIVisible()))
	{
		// attract screen might be open, but not topmost due to notification dialogs
		bDoBlur = false;
	}

	if (!bDoBlur)
	{
		bDoBlur = GameClientExports()->ClientWantsBlurEffect();
	}

	float nowTime = Plat_FloatTime();
	float deltaTime = nowTime - m_flLastBlurTime;
	if (deltaTime > 0)
	{
		m_flLastBlurTime = nowTime;
		m_flBlurScale += deltaTime * bDoBlur ? 0.05f : -0.05f;
		m_flBlurScale = clamp(m_flBlurScale, 0, 0.85f);
		engine->SetBlurFade(m_flBlurScale);
	}
}

void CBaseModPanel::PaintBackground()
{
}

void CBaseModPanel::CloseAllWindows( int ePolicyFlags )
{
	CAutoPushPop< bool > auto_m_bClosingAllWindows(m_bClosingAllWindows, true);

	if (UI_IsDebug())
	{
		Msg("[GAMEUI] CBaseModPanel::CloseAllWindows( 0x%x )\n", ePolicyFlags);
	}

	// make sure we also close any active flyout menus that might be hanging out.
	//FlyoutMenu::CloseActiveMenu();

	for (int i = 0; i < WT_WINDOW_COUNT; ++i)
	{
		CBaseModFrame* pFrame = m_Frames[i].Get();
		if (!pFrame)
			continue;

		int nPriority = pFrame->GetWindowPriority();

		switch (nPriority)
		{
		case WPRI_LOADINGPLAQUE:
			if (!(ePolicyFlags & CLOSE_POLICY_EVEN_LOADING))
			{
				if (UI_IsDebug())
				{
					Msg("[GAMEUI] CBaseModPanel::CloseAllWindows() - Keeping loading type %d of priority %d.\n", i, nPriority);
				}

				continue;
				m_ActiveWindow[WPRI_LOADINGPLAQUE] = WT_NONE;
			}
			break;

		case WPRI_MESSAGE:
			if (!(ePolicyFlags & CLOSE_POLICY_EVEN_MSGS))
			{
				if (UI_IsDebug())
				{
					Msg("[GAMEUI] CBaseModPanel::CloseAllWindows() - Keeping msgbox type %d of priority %d.\n", i, nPriority);
				}

				continue;
				m_ActiveWindow[WPRI_MESSAGE] = WT_NONE;
			}
			break;

		case WPRI_BKGNDSCREEN:
			if (ePolicyFlags & CLOSE_POLICY_KEEP_BKGND)
			{
				if (UI_IsDebug())
				{
					Msg("[GAMEUI] CBaseModPanel::CloseAllWindows() - Keeping bkgnd type %d of priority %d.\n", i, nPriority);
				}

				continue;
				m_ActiveWindow[WPRI_BKGNDSCREEN] = WT_NONE;
			}
			break;
		}

		// Close the window
		pFrame->Close();
		m_Frames[i] = NULL;
	}

	if (UI_IsDebug())
	{
		Msg("[GAMEUI] After close all windows:\n");
		DbgShowCurrentUIState();
	}

	m_ActiveWindow[WPRI_NORMAL] = WT_NONE;
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
	//if (m_DelayActivation)
	//	return;

	COM_TimestampedLog("CBaseModPanel::OnGameUIActivated()");

	SetVisible(true);

	if (WT_GAMELOBBY == GetActiveWindowType())
	{
		return;
	}
	else if (!IsX360() && WT_LOADINGPROGRESS == GetActiveWindowType())
	{
		// Ignore UI activations when loading poster is up
		return;
	}
	else if ((!m_LevelLoading && !engine->IsConnected()) || GameUI().IsInBackgroundLevel())
	{
		bool bForceReturnToFrontScreen = false;
		WINDOW_TYPE wt = GetActiveWindowType();
		switch (wt)
		{
		default:
			break;
		case WT_NONE:
		case WT_INGAMEMAINMENU:
		case WT_GENERICCONFIRMATION:
			// bForceReturnToFrontScreen = !g_pMatchFramework->GetMatchmaking()->ShouldPreventOpenFrontScreen();
			bForceReturnToFrontScreen = true; // this used to be some magic about mid-disconnecting-states on PC...
			break;
		}
		if (!IsPC() || bForceReturnToFrontScreen)
		{
			OpenFrontScreen();
		}
	}
	else if (engine->IsConnected() && !m_LevelLoading)
	{
		CBaseModFrame* pInGameMainMenu = m_Frames[WT_INGAMEMAINMENU].Get();

		if (!pInGameMainMenu || !pInGameMainMenu->IsAutoDeleteSet())
		{
			// Prevent in game menu from opening if it already exists!
			// It might be hiding behind a modal window that needs to keep focus
			OpenWindow(WT_INGAMEMAINMENU, 0);
		}
	}
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
	if (IsPC())
		GameConsole().Hide();
}

void CBaseModPanel::OnEvent( KeyValues *pEvent )
{
}

void CBaseModPanel::SetHelpText(const char* helpText)
{
	if (m_FooterPanel)
	{
		m_FooterPanel->SetHelpText(helpText);
	}
}

void CBaseModPanel::SetOkButtonEnabled(bool bEnabled)
{
	if (m_FooterPanel)
	{
		CBaseModFooterPanel::FooterButtons_t buttons = m_FooterPanel->GetButtons();
		if (bEnabled)
			buttons |= FB_ABUTTON;
		else
			buttons &= ~FB_ABUTTON;
		m_FooterPanel->SetButtons(buttons, m_FooterPanel->GetFormat(), m_FooterPanel->GetHelpTextEnabled());
	}
}

void CBaseModPanel::SetCancelButtonEnabled(bool bEnabled)
{
	if (m_FooterPanel)
	{
		CBaseModFooterPanel::FooterButtons_t buttons = m_FooterPanel->GetButtons();
		if (bEnabled)
			buttons |= FB_BBUTTON;
		else
			buttons &= ~FB_BBUTTON;
		m_FooterPanel->SetButtons(buttons, m_FooterPanel->GetFormat(), m_FooterPanel->GetHelpTextEnabled());
	}
}

BaseModUI::CBaseModFooterPanel* CBaseModPanel::GetFooterPanel()
{
	// EVIL HACK
	if (!this)
	{
		Assert(0);
		Warning("CBaseModPanel::GetFooterPanel() called on NULL CBaseModPanel!!!\n");
		return NULL;
	}
	return m_FooterPanel;
}

void CBaseModPanel::SetLastActiveUserId(int userId)
{
	if (m_lastActiveUserId != userId)
	{
		DevWarning("SetLastActiveUserId: %d -> %d\n", m_lastActiveUserId, userId);
	}

	m_lastActiveUserId = userId;
}

int CBaseModPanel::GetLastActiveUserId()
{
	return m_lastActiveUserId;
}

const char* CBaseModPanel::GetUISoundName(UISound_t UISound)
{
	switch (UISound)
	{
	case UISOUND_BACK:
		return "UI/menu_back.wav";
	case UISOUND_ACCEPT:
		return "UI/menu_accept.wav";
	case UISOUND_INVALID:
		return "UI/menu_invalid.wav";
	case UISOUND_COUNTDOWN:
		return "UI/menu_countdown.wav";
	case UISOUND_FOCUS:
		return "UI/menu_focus.wav";
	case UISOUND_CLICK:
		return "UI/buttonclick.wav";
	case UISOUND_DENY:
		return "UI/menu_invalid.wav";
	}
	return NULL;
}

void CBaseModPanel::PlayUISound(UISound_t UISound)
{
	const char* pSound = GetUISoundName(UISound);
	if (pSound)
	{
		vgui::surface()->PlaySound(pSound);
	}
}

//=============================================================================
void CBaseModPanel::OnFrameClosed(WINDOW_PRIORITY pri, WINDOW_TYPE wt)
{
	if (UI_IsDebug())
	{
		Msg("[GAMEUI] CBaseModPanel::OnFrameClosed( %d, %d )\n", pri, wt);
		DbgShowCurrentUIState();
	}

	// Mark the frame that just closed as NULL so that nobody could find it
	m_Frames[wt] = NULL;

	if (m_bClosingAllWindows)
	{
		if (UI_IsDebug())
		{
			Msg("[GAMEUI] Closing all windows\n");
		}
		return;
	}

	if (pri <= WPRI_NORMAL)
		return;

	for (int k = 0; k < WPRI_COUNT; ++k)
	{
		if (m_ActiveWindow[k] == wt)
			m_ActiveWindow[k] = WT_NONE;
	}

	//
	// We only care to resurrect windows of lower priority when
	// higher priority windows close
	//

	for (int k = WPRI_COUNT; k-- > 0; )
	{
		if (m_ActiveWindow[k] == WT_NONE)
			continue;

		CBaseModFrame* pFrame = m_Frames[m_ActiveWindow[k]].Get();
		if (!pFrame)
			continue;

		// pFrame->AddActionSignalTarget(this);

		pFrame->InvalidateLayout(false, false);
		pFrame->OnOpen();
		pFrame->SetVisible(true);
		pFrame->Activate();

		if (UI_IsDebug())
		{
			Msg("[GAMEUI] CBaseModPanel::OnFrameClosed( %d, %d ) -> Activated `%s`, pri=%d\n",
				pri, wt, pFrame->GetName(), pFrame->GetWindowPriority());
			DbgShowCurrentUIState();
		}

		return;
	}
}

void CBaseModPanel::DbgShowCurrentUIState()
{
	if (UI_IsDebug() < 2)
		return;

	Msg("[GAMEUI] Priorities WT: ");
	for (int i = 0; i < WPRI_COUNT; ++i)
	{
		Msg(" %d ", m_ActiveWindow[i]);
	}
	Msg("\n");
	for (int i = 0; i < WT_WINDOW_COUNT; ++i)
	{
		CBaseModFrame* pFrame = m_Frames[i].Get();
		if (pFrame)
		{
			Msg("        %2d. `%s` pri%d vis%d\n",
				i, pFrame->GetName(), pFrame->GetWindowPriority(), pFrame->IsVisible());
		}
		else
		{
			Msg("        %2d. NULL\n", i);
		}
	}
}

bool CBaseModPanel::IsLevelLoading()
{
	return m_LevelLoading;
}

//-----------------------------------------------------------------------------
// Purpose: moves the game menu button to the right place on the taskbar
//-----------------------------------------------------------------------------
static void BaseUI_PositionDialog(vgui::PHandle dlg)
{
	if (!dlg.Get())
		return;

	int x, y, ww, wt, wide, tall;
	vgui::surface()->GetWorkspaceBounds(x, y, ww, wt);
	dlg->GetSize(wide, tall);

	// Center it, keeping requested size
	dlg->SetPos(x + ((ww - wide) / 2), y + ((wt - tall) / 2));
}

void CBaseModPanel::OpenOptionsDialog(Panel* parent) {
	
}

void CBaseModPanel::OpenFrontScreen() {
	WINDOW_TYPE frontWindow = WT_MAINMENU;

	if (GetActiveWindowType() != frontWindow)
	{
		CloseAllWindows();
		OpenWindow(frontWindow, NULL);
	}
}

//=============================================================================
CBaseModFrame* CBaseModPanel::GetWindow(const WINDOW_TYPE& wt)
{
	return m_Frames[wt].Get();
}

//=============================================================================
WINDOW_TYPE CBaseModPanel::GetActiveWindowType()
{
	for (int k = WPRI_COUNT; k-- > 0; )
	{
		if (m_ActiveWindow[k] != WT_NONE)
		{
			CBaseModFrame* pFrame = m_Frames[m_ActiveWindow[k]].Get();
			if (!pFrame || !pFrame->IsVisible())
				continue;

			return m_ActiveWindow[k];
		}
	}
	return WT_NONE;
}

//=============================================================================
WINDOW_PRIORITY CBaseModPanel::GetActiveWindowPriority()
{
	for (int k = WPRI_COUNT; k-- > 0; )
	{
		if (m_ActiveWindow[k] != WT_NONE)
		{
			CBaseModFrame* pFrame = m_Frames[m_ActiveWindow[k]].Get();
			if (!pFrame || !pFrame->IsVisible())
				continue;

			return WINDOW_PRIORITY(k);
		}
	}
	return WPRI_NONE;
}

//=============================================================================
void CBaseModPanel::SetActiveWindow(CBaseModFrame* frame)
{
	if (!frame)
		return;

	m_ActiveWindow[frame->GetWindowPriority()] = frame->GetWindowType();

	if (GetActiveWindowPriority() > frame->GetWindowPriority())
	{
		if (UI_IsDebug())
		{
			CBaseModFrame* pOther = m_Frames[GetActiveWindowType()].Get();
			Warning("[GAMEUI] SetActiveWindow: Another window %p`%s` is having priority %d, deferring `%s`!\n",
				pOther, pOther ? pOther->GetName() : "<<null>>",
				GetActiveWindowPriority(), frame->GetName());
		}

		// frame->SetVisible( false );
	}
	else
	{
		frame->OnOpen();
	}
}

//=============================================================================
CBaseModFrame* CBaseModPanel::OpenWindow(const WINDOW_TYPE& wt, CBaseModFrame* caller, bool hidePrevious, KeyValues* pParameters)
{
	CBaseModFrame* newNav = m_Frames[wt].Get();
	bool setActiveWindow = true;

	// Window priority is used to track which windows are visible at all times
	// it is used to resolve the situations when a game requests an error box to popup
	// while a loading progress bar is active.
	// Windows with a higher priority force all other windows to get hidden.
	// After the high-priority window goes away it falls back to restore the low priority windows.
	WINDOW_PRIORITY nWindowPriority = WPRI_NORMAL;

	switch (wt)
	{
	case WT_PASSWORDENTRY:
		setActiveWindow = false;
		break;
	}

	switch (wt)
	{
	case WT_GENERICWAITSCREEN:
		nWindowPriority = WPRI_WAITSCREEN;
		break;
	case WT_GENERICCONFIRMATION:
		nWindowPriority = WPRI_MESSAGE;
		break;
	case WT_LOADINGPROGRESSBKGND:
		nWindowPriority = WPRI_BKGNDSCREEN;
		break;
	case WT_LOADINGPROGRESS:
		nWindowPriority = WPRI_LOADINGPLAQUE;
		break;
	case WT_PASSWORDENTRY:
		nWindowPriority = WPRI_TOPMOST;
		break;
	case WT_TRANSITIONSCREEN:
		nWindowPriority = WPRI_TOPMOST;
		break;
	}

	if (!newNav)
	{
		switch (wt)
		{
		case WT_ACHIEVEMENTS:
			//m_Frames[wt] = new Achievements(this, "Achievements");
			break;

		case WT_AUDIO:
			//m_Frames[wt] = new Audio(this, "Audio");
			break;

		case WT_AUDIOVIDEO:
			//m_Frames[wt] = new AudioVideo(this, "AudioVideo");
			break;

		case WT_CLOUD:
			break;

		case WT_CONTROLLER:
			//m_Frames[wt] = new ControllerOptions(this, "ControllerOptions");
			break;

		case WT_CONTROLLER_STICKS:
			//m_Frames[wt] = new ControllerOptionsSticks(this, "ControllerOptionsSticks");
			break;

		case WT_CONTROLLER_BUTTONS:
			//m_Frames[wt] = new ControllerOptionsButtons(this, "ControllerOptionsButtons");
			break;

		case WT_DOWNLOADS:
			break;

		case WT_GAMELOBBY:
			break;

		case WT_GAMEOPTIONS:
			//m_Frames[wt] = new GameOptions(this, "GameOptions");
			break;

		case WT_GAMESETTINGS:
			//m_Frames[wt] = new GameSettings(this, "GameSettings");
			break;

		case WT_GENERICCONFIRMATION:
			//m_Frames[wt] = new GenericConfirmation(this, "GenericConfirmation");
			break;

		case WT_INGAMEDIFFICULTYSELECT:
			//m_Frames[wt] = new InGameDifficultySelect(this, "InGameDifficultySelect");
			break;

		case WT_INGAMEMAINMENU:
			//m_Frames[wt] = new InGameMainMenu(this, "InGameMainMenu");
			break;

		case WT_INGAMECHAPTERSELECT:
			//m_Frames[wt] = new InGameChapterSelect(this, "InGameChapterSelect");
			break;

		case WT_INGAMEKICKPLAYERLIST:
			break;

		case WT_VOTEOPTIONS:
			break;

		case WT_KEYBOARDMOUSE:
			//m_Frames[wt] = new VKeyboard(this, "VKeyboard");
			break;

		case WT_LOADINGPROGRESSBKGND:
			//m_Frames[wt] = new LoadingProgress(this, "LoadingProgress", LoadingProgress::LWT_BKGNDSCREEN);
			break;

		case WT_LOADINGPROGRESS:
			//m_Frames[wt] = new LoadingProgress(this, "LoadingProgress", LoadingProgress::LWT_LOADINGPLAQUE);
			break;

		case WT_MAINMENU:
			m_Frames[wt] = new MainMenu(this, "MainMenu"); // TODO: FIRST
			break;

		case WT_MULTIPLAYER:
			//m_Frames[wt] = new Multiplayer(this, "Multiplayer");
			break;

		case WT_OPTIONS:
			//m_Frames[wt] = new Options(this, "Options");
			break;

		case WT_SIGNINDIALOG:
			//m_Frames[wt] = new SignInDialog(this, "SignInDialog");
			break;

		case WT_GENERICWAITSCREEN:
			//m_Frames[wt] = new GenericWaitScreen(this, "GenericWaitScreen");
			break;

		case WT_PASSWORDENTRY:
			//m_Frames[wt] = new PasswordEntry(this, "PasswordEntry");
			break;

		case WT_ATTRACTSCREEN:
			//m_Frames[wt] = new CAttractScreen(this, "AttractScreen");
			break;

		case WT_ALLGAMESEARCHRESULTS:
			//m_Frames[wt] = new FoundGames(this, "FoundGames");
			break;

		case WT_FOUNDPUBLICGAMES:
			//m_Frames[wt] = new FoundPublicGames(this, "FoundPublicGames");
			break;

		case WT_TRANSITIONSCREEN:
			//m_Frames[wt] = new CTransitionScreen(this, "TransitionScreen");
			break;

		case WT_VIDEO:
			//m_Frames[wt] = new Video(this, "Video");
			break;

		case WT_STEAMCLOUDCONFIRM:
			break;

		case WT_STEAMGROUPSERVERS:
			break;

		case WT_CUSTOMCAMPAIGNS:
			break;

		case WT_DOWNLOADCAMPAIGN:
			break;

		case WT_LEADERBOARD:
			break;

		case WT_ADDONS:
			break;

		case WT_JUKEBOX:
			break;

		case WT_ADDONASSOCIATION:
			break;

		case WT_GETLEGACYDATA:
			break;

		default:
			Assert(false);	// unknown window type
			break;
		}

		//
		// Finish setting up the window
		//

		newNav = m_Frames[wt].Get();
		if (!newNav)
			return NULL;

		newNav->SetWindowPriority(nWindowPriority);
		newNav->SetWindowType(wt);
		newNav->SetVisible(false);
	}

	newNav->SetDataSettings(pParameters);

	if (setActiveWindow)
	{
		m_ActiveWindow[nWindowPriority] = wt;
		newNav->AddActionSignalTarget(this);
		newNav->SetCanBeActiveWindowType(true);
	}
	else if (nWindowPriority == WPRI_MESSAGE)
	{
		m_ActiveWindow[nWindowPriority] = wt;
	}

	//
	// Now the window has been created, set it up
	//

	if (UI_IsDebug() && (wt != WT_LOADINGPROGRESS))
	{
		Msg("[GAMEUI] OnOpen( `%s`, caller = `%s`, hidePrev = %d, setActive = %d, wt=%d, wpri=%d )\n",
			newNav->GetName(), caller ? caller->GetName() : "<NULL>", int(hidePrevious),
			int(setActiveWindow), wt, nWindowPriority);
		KeyValuesDumpAsDevMsg(pParameters, 1);
	}

	newNav->SetNavBack(caller);

	if (hidePrevious && caller != 0)
	{
		caller->SetVisible(false);
	}
	else if (caller != 0)
	{
		caller->FindAndSetActiveControl();
		//caller->SetAlpha(128);
	}

	// Check if a higher priority window is open
	if (GetActiveWindowPriority() > newNav->GetWindowPriority())
	{
		if (UI_IsDebug())
		{
			CBaseModFrame* pOther = m_Frames[GetActiveWindowType()].Get();
			Warning("[GAMEUI] OpenWindow: Another window %p`%s` is having priority %d, deferring `%s`!\n",
				pOther, pOther ? pOther->GetName() : "<<null>>",
				GetActiveWindowPriority(), newNav->GetName());
		}

		// There's a higher priority window that was open at the moment,
		// hide our window for now, it will get restored later.
		// newNav->SetVisible( false );
	}
	else
	{
		newNav->InvalidateLayout(false, false);
		newNav->OnOpen();
	}

	if (UI_IsDebug() && (wt != WT_LOADINGPROGRESS))
	{
		DbgShowCurrentUIState();
	}

	return newNav;
}