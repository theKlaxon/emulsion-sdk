//=========== Copyright © 2014, rHetorical, All rights reserved. =============
//
// Purpose: 
//		
//=============================================================================

#ifndef HUD_VEC_CROSSHAIR_H
#define HUD_VEC_CROSSHAIR_H
#ifdef _WIN32
#pragma once
#endif

#include "hudelement.h"
#include <vgui_controls/Panel.h>

namespace vgui
{
	class IScheme;
};

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CHudVecCrosshair : public CHudElement, public vgui::Panel
{
public:
	DECLARE_CLASS_SIMPLE( CHudVecCrosshair, vgui::Panel );

	CHudVecCrosshair( const char *name );

	virtual void OnThink();
	virtual void Paint();
	virtual void Init();
	void VidInit( void );
	virtual bool ShouldDraw();
	virtual void ApplySchemeSettings( vgui::IScheme *scheme );

	//virtual void LevelShutdown( void );

private:

	void	UpdateEventTime( void );
	bool	EventTimeElapsed( void );
	float	m_flLastEventTime;


	//int					m_iCrosshairTextureID;
	//IVguiMatInfo		*m_pCrosshair;

	//char				m_szPreviousCrosshair[256];	// name of the current crosshair
	//float				m_flAccuracy;	

	//CHudTexture		*m_icon01;

	CHudTexture	*m_icon_c;

	CHudTexture	*m_icon_rbn;	// right bracket
	CHudTexture	*m_icon_lbn;	// left bracket

	CHudTexture	*m_icon_rb;		// right bracket, full
	CHudTexture	*m_icon_lb;		// left bracket, full
	CHudTexture	*m_icon_rbe;	// right bracket, empty
	CHudTexture	*m_icon_lbe;	// left bracket, empty

	int m_intCurrentBall;
};

#endif // HUD_VEC_CROSSHAIR_H
