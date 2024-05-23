//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $Workfile:     $
// $Date:         $
// $NoKeywords: $
//=============================================================================//
#if !defined( CLIENTMODE_SDK_H )
#define CLIENTMODE_SDK_H
#ifdef _WIN32
#pragma once
#endif

#include "clientmode_shared.h"
#include <vgui_controls/EditablePanel.h>
#include <vgui/Cursor.h>

class CHudViewport;

namespace vgui
{
	typedef unsigned long HScheme;
}

class ClientModeSDKNormal : public ClientModeShared
{
	DECLARE_CLASS( ClientModeSDKNormal, ClientModeShared );
public:

	ClientModeSDKNormal();
	~ClientModeSDKNormal();

	virtual void	Init();

	virtual bool	DoPostScreenSpaceEffects( const CViewSetup *pSetup );

	/*
	void InitViewport()
	{
		m_pViewport = new CHudViewport();
		//m_pViewport = new CBaseViewport;
		m_pViewport->Start( gameuifuncs, gameeventmanager );
		m_pViewport->SetPaintBackgroundEnabled(false);
		m_pViewport->SetName( "SDK viewport" );
		m_pViewport->SetBounds( 0, 0, ScreenWidth(), ScreenHeight() );
	}
	*/
};

extern IClientMode *GetClientModeNormal();

#endif // CLIENTMODE_SDK_H
