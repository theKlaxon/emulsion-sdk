//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//


#include <stdio.h>

#include "GameConsole.h"
#include "GameConsoleDialog.h"
#include "LoadingDialog.h"
#include "vgui/ISurface.h"

#include "KeyValues.h"
#include "vgui/VGUI.h"
#include "vgui/IVGUI.h"
#include "vgui_controls/Panel.h"
#include "convar.h"
#include "GameUI/swarm/basemodui.h"
#include "GameUI/swarm/basemodpanel.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

static CGameConsole g_GameConsole;
//-----------------------------------------------------------------------------
// Purpose: singleton accessor
//-----------------------------------------------------------------------------
CGameConsole &GameConsole()
{
	return g_GameConsole;
}
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CGameConsole, IGameConsole, GAMECONSOLE_INTERFACE_VERSION, g_GameConsole);

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CGameConsole::CGameConsole()
{
	m_bInitialized = false;
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CGameConsole::~CGameConsole()
{
	m_bInitialized = false;
}

//-----------------------------------------------------------------------------
// Purpose: sets up the console for use
//-----------------------------------------------------------------------------
void CGameConsole::Initialize()
{
#ifndef _XBOX
	m_pConsole = vgui::SETUP_PANEL( new CGameConsoleDialog() ); // we add text before displaying this so set it up now!
	
	// set the console to taking up most of the right-half of the screen
	int swide, stall;
	vgui::surface()->GetScreenSize(swide, stall);
	int offset = vgui::scheme()->GetProportionalScaledValue(16);

	m_pConsole->SetBounds(
		swide / 2 - (offset * 4),
		offset,
		(swide / 2) + (offset * 3),
		stall - (offset * 8));

	m_bInitialized = true;
#endif
}

#include "..\public\vgui_controls\Controls.h"
extern BaseModUI::CBaseModPanel& GetUiBaseModPanelClass();
vgui::VPANEL pauseFrameParent;

//-----------------------------------------------------------------------------
// Purpose: activates the console, makes it visible and brings it to the foreground
//-----------------------------------------------------------------------------
void CGameConsole::Activate()
{
#ifndef _XBOX
	if (!m_bInitialized)
		return;

	vgui::surface()->RestrictPaintToSinglePanel(NULL);
	m_pConsole->Activate();

	//vgui::surface()->RestrictPaintToSinglePanel(NULL);
	//m_pConsole->SetEnabled(true);
	//m_pConsole->Activate();
	//m_pConsole->RequestFocus();

	////callback to te ingamemainmenu if there is one, and fuck it up real good
	//BaseModUI::CBaseModFrame* pauseFrame = GetUiBaseModPanelClass().m_Frames[BaseModUI::WT_INGAMEMAINMENU].Get();
	//if (pauseFrame) {
	//	pauseFrame->Close();
	//}
		
#endif
}

//-----------------------------------------------------------------------------
// Purpose: hides the console
//-----------------------------------------------------------------------------
void CGameConsole::Hide()
{
#ifndef _XBOX
	if (!m_bInitialized)
		return;

	m_pConsole->Hide();
#endif
}

//-----------------------------------------------------------------------------
// Purpose: clears the console
//-----------------------------------------------------------------------------
void CGameConsole::Clear()
{
#ifndef _XBOX
	if (!m_bInitialized)
		return;

	m_pConsole->Clear();
#endif
}


//-----------------------------------------------------------------------------
// Purpose: returns true if the console is currently in focus
//-----------------------------------------------------------------------------
bool CGameConsole::IsConsoleVisible()
{
#ifndef _XBOX
	if (!m_bInitialized)
		return false;
	
	return m_pConsole->IsVisible();
#else
	return false;
#endif
}

//-----------------------------------------------------------------------------
// Purpose: activates the console after a delay
//-----------------------------------------------------------------------------
void CGameConsole::ActivateDelayed(float time)
{
#ifndef _XBOX
	if (!m_bInitialized)
		return;

	m_pConsole->PostMessage(m_pConsole, new KeyValues("Activate"), time);
#endif
}

void CGameConsole::SetParent( int parent )
{	
#ifndef _XBOX
	if (!m_bInitialized)
		return;

	m_pConsole->SetParent( static_cast<vgui::VPANEL>( parent ));
#endif
}

vgui::VPANEL CGameConsole::GetParent() {
	if (!m_bInitialized)
		return 0;

	return m_pConsole->GetParent()->GetVPanel();
}

//-----------------------------------------------------------------------------
// Purpose: static command handler
//-----------------------------------------------------------------------------
void CGameConsole::OnCmdCondump()
{
#ifndef _XBOX
	g_GameConsole.m_pConsole->DumpConsoleTextToFile();
#endif
}

#ifndef _XBOX
CON_COMMAND( condump, "dump the text currently in the console to condumpXX.log" )
{
	g_GameConsole.OnCmdCondump();
}
#endif
