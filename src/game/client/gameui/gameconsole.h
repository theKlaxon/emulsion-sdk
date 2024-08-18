//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef GAMECONSOLE_H
#define GAMECONSOLE_H
#ifdef _WIN32
#pragma once
#endif

#include "GameUI/IGameConsole.h"

class CGameConsoleDialog;

//-----------------------------------------------------------------------------
// Purpose: VGui implementation of the game/dev console
//-----------------------------------------------------------------------------
class CGameConsole : public IGameConsole
{
public:
	CGameConsole();
	~CGameConsole();

	// sets up the console for use
	void Initialize();

	// activates the console, makes it visible and brings it to the foreground
	virtual void Activate();
	// hides the console
	virtual void Hide();
	// clears the console
	virtual void Clear();

	// returns true if the console is currently in focus
	virtual bool IsConsoleVisible();

	virtual void SetParent(int parent);

	// activates the console after a delay
	void ActivateDelayed(float time);

	// override used by sativa ui, unused here.
	virtual void Override(IGameConsole* pConsole) {};

	static void OnCmdCondump();
private:

	bool m_bInitialized;
	CGameConsoleDialog *m_pConsole;
};

extern IGameConsole &GameConsole();

#endif // GAMECONSOLE_H
