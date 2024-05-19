//=========== Copyright © 2014, rHetorical, All rights reserved. =============
//
// Purpose: 
//		
//=============================================================================

#include "cbase.h"
#include "hud.h"
#include "hudelement.h"
#include "iclientmode.h"
#include "engine/IEngineSound.h"
#include "vgui_controls/AnimationController.h"
#include "vgui_controls/Controls.h"
#include <vgui_controls/Panel.h>
#include <vgui/isurface.h>
#include "clientmode.h"
#include "hud_vectronic_quickinfo.h"
#include "c_vectronic_player.h"
#include "c_weapon_vecgun.h"
#include "materialsystem/IMaterial.h"
#include "materialsystem/IMesh.h"
#include "materialsystem/imaterialvar.h"
#include "mathlib/mathlib.h"
#include "../hud_crosshair.h"

#ifdef SIXENSE
#include "sixense/in_sixense.h"
#include "view.h"
int ScreenTransform( const Vector& point, Vector& screen );
#endif

// Our sharedefs.
#include "vectronic_shareddefs.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

// CROSSHAIRS TOO BIG! THIS GAME IS FOR N00BS #CrosshairGate EXPOSED!
//#define CrosshairGate


#define QUICKINFO_EVENT_DURATION	1.0f

extern ConVar vectronic_vecball3_color_r;
extern ConVar vectronic_vecball3_color_g;
extern ConVar vectronic_vecball3_color_b;

extern ConVar vectronic_vecball4_color_r;
extern ConVar vectronic_vecball4_color_g;
extern ConVar vectronic_vecball4_color_b;

extern ConVar vectronic_vecball5_color_r;
extern ConVar vectronic_vecball5_color_g;
extern ConVar vectronic_vecball5_color_b;

using namespace vgui;

DECLARE_HUDELEMENT( CHudVecCrosshair );

CHudVecCrosshair::CHudVecCrosshair( const char *pName ) :
	vgui::Panel( NULL, "HUDQuickInfo" ), CHudElement( pName )
{
#ifndef SWARM_DLL
	vgui::Panel *pParent = g_pClientMode->GetViewport();
#else
	vgui::Panel *pParent = GetClientMode()->GetViewport();
#endif
	SetParent( pParent );

	SetHiddenBits( HIDEHUD_CROSSHAIR );

	/*
	m_szPreviousCrosshair[0] = '\0';
	m_flAccuracy = 0.1;
	*/


}

void CHudVecCrosshair::ApplySchemeSettings( IScheme *scheme )
{
	BaseClass::ApplySchemeSettings( scheme );

	//m_icon01 = gHUD.GetIcon( "vectronic_crosshair" );
	//SetSize( ScreenWidth(), ScreenHeight() );

	SetPaintBackgroundEnabled( false );
#ifndef SWARM_DLL
	SetForceStereoRenderToFrameBuffer( true );
#endif

}

void CHudVecCrosshair::Init()
{
	//m_iCrosshairTextureID = vgui::surface()->CreateNewTextureID();
	m_flLastEventTime   = 0.0f;
}

void CHudVecCrosshair::VidInit( void )
{
	Init();

#ifndef SWARM_DLL
	m_icon_c = gHUD.GetIcon( "crosshair" );

	m_icon_rb = gHUD.GetIcon( "vectronic_crosshair_right_full" );
	m_icon_lb = gHUD.GetIcon( "vectronic_crosshair_left_full" );
	m_icon_rbe = gHUD.GetIcon( "vectronic_crosshair_right_empty" );
	m_icon_lbe = gHUD.GetIcon( "vectronic_crosshair_left_empty" );
	m_icon_rbn = gHUD.GetIcon( "vectronic_crosshair_right" );
	m_icon_lbn = gHUD.GetIcon( "vectronic_crosshair_left" );
#else
	m_icon_c = HudIcons().GetIcon( "crosshair" );

#ifndef CrosshairGate
	m_icon_rb = HudIcons().GetIcon( "vectronic_crosshair_right_full" );
	m_icon_lb = HudIcons().GetIcon( "vectronic_crosshair_left_full" );
	m_icon_rbe = HudIcons().GetIcon( "vectronic_crosshair_right_empty" );
	m_icon_lbe = HudIcons().GetIcon( "vectronic_crosshair_left_empty" );
	m_icon_rbn = HudIcons().GetIcon( "vectronic_crosshair_right" );
	m_icon_lbn = HudIcons().GetIcon( "vectronic_crosshair_left" );
#else
	m_icon_rb = HudIcons().GetIcon( "vectronic_crosshair_right_full_small" );
	m_icon_lb = HudIcons().GetIcon( "vectronic_crosshair_left_full_small" );
	m_icon_rbe = HudIcons().GetIcon( "vectronic_crosshair_right_empty_small" );
	m_icon_lbe = HudIcons().GetIcon( "vectronic_crosshair_left_empty_small" );
	m_icon_rbn = HudIcons().GetIcon( "vectronic_crosshair_right_small" );
	m_icon_lbn = HudIcons().GetIcon( "vectronic_crosshair_left_small" );
#endif
#endif
}

/*
void CHudVecCrosshair::LevelShutdown( void )
{
	// forces m_pFrameVar to recreate next map
	m_szPreviousCrosshair[0] = '\0';

	if ( m_pCrosshair )
	{
		delete m_pCrosshair;
		m_pCrosshair = NULL;
	}
}
*/

bool CHudVecCrosshair::ShouldDraw()
{
	if ( !m_icon_c || !m_icon_rb || !m_icon_rbe || !m_icon_lb || !m_icon_lbe )
		return false;

	C_VectronicPlayer *pPlayer = C_VectronicPlayer::GetLocalPlayer();
	if ( !pPlayer )
		return false;

	if( !pPlayer ->IsAlive() || pPlayer ->IsObserver())
		return false;

	if ( !crosshair.GetBool() && !IsX360() )
		return false;

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: Checks if the hud element needs to fade out
//-----------------------------------------------------------------------------
void CHudVecCrosshair::OnThink()
{
	BaseClass::OnThink();

	C_BasePlayer *player = C_BasePlayer::GetLocalPlayer();
	if ( player == NULL )
		return;

}

void CHudVecCrosshair::Paint()
{
	C_VectronicPlayer *pPlayer = C_VectronicPlayer::GetLocalPlayer();
	if( !pPlayer )
		return;

#ifndef SWARM_DLL
	C_BaseCombatWeapon *pWeapon = GetActiveWeapon();
	if ( pWeapon == NULL )
		return;

	C_WeaponVecgun *pVecgun = To_Vecgun(pWeapon);
	if ( pVecgun == NULL )
		return;
#else
	C_WeaponVecgun *pVecgun = To_Vecgun(pPlayer->GetActiveWeapon());
	if ( pVecgun == NULL )
		return;
#endif

	if (pPlayer->PlayerHasObject())
		return;
/*
	float fX, fY;
	bool bBehindCamera = false;
	CHudCrosshair::GetDrawPosition( &fX, &fY, &bBehindCamera );

	// if the crosshair is behind the camera, don't draw it
	if( bBehindCamera )
		return;

	float flPlayerScale = 1.0f;
	float flWeaponScale = 1.0f;
	int iTextureW = m_icon01->Width();
	int iTextureH = m_icon01->Height();

	float flWidth = flWeaponScale * flPlayerScale * (float)iTextureW;
	float flHeight = flWeaponScale * flPlayerScale * (float)iTextureH;
	int iWidth = (int)( flWidth + 0.5f );
	int iHeight = (int)( flHeight + 0.5f );
	int iX = (int)( fX + 0.5f );
	int iY = (int)( fY + 0.5f );

//	Color clr( cl_crosshair_red.GetInt(), cl_crosshair_green.GetInt(), cl_crosshair_blue.GetInt(), 255 );

	Color XhairWhiteColor(255,255,255,255);
	Color XhairBlueColor(BALL0_COLOR);
	Color XhairGreenColor(BALL1_COLOR);
	Color XhairYellowColor(BALL2_COLOR);

	Color clr;

	m_intCurrentBall = pVecgun->GetCurrentSelection();
	if ( pVecgun->CanFireBlue() || pVecgun->CanFireGreen() || pVecgun->CanFireYellow() )
	{
		if ( m_intCurrentBall == 0 )
		{
			clr = XhairBlueColor;
		}
		else if ( m_intCurrentBall == 1 )
		{
			clr = XhairGreenColor;
		}
		else if ( m_intCurrentBall == 2 )
		{
			clr = XhairYellowColor;
		}
	}
	else
	{
		clr = XhairWhiteColor;
	} 

	Color Finalclr = clr;

	m_icon01->DrawSelfCropped (
	iX-(iWidth/2), iY-(iHeight/2),
	0, 0,
	iTextureW, iTextureH,
	iWidth, iHeight,
	Finalclr );

*/

#ifndef SWARM_DLL
	float fX, fY;
	bool bBehindCamera = false;
	CHudCrosshair::GetDrawPosition( &fX, &fY, &bBehindCamera );

	// if the crosshair is behind the camera, don't draw it
	if( bBehindCamera )
		return;

	int		xCenter	= (int)fX;
	int		yCenter = (int)fY - m_icon_lb->Height() / 2;
	
	Color clrNormal = gHUD.m_clrNormal;
	clrNormal[3] = 255;

	m_icon_c->DrawSelf( xCenter, yCenter, clrNormal );
#else
	int		xCenter	= ScreenWidth() / 2;
	int		yCenter = ( ScreenHeight() - m_icon_c->Height() ) / 2;
	
	Color clrNormal = CHud().m_clrNormal;
	clrNormal[3] = 255;

#endif

	m_icon_c->DrawSelf( xCenter, yCenter, clrNormal );

	if( IsX360() )
	{
		// Because the fixed reticle draws on half-texels, this rather unsightly hack really helps
		// center the appearance of the quickinfo on 360 displays.
		xCenter += 1;
	}

	Color XhairWhiteColor(255,255,255,255);
	Color XhairBlueColor(BALL0_COLOR_R, BALL0_COLOR_G, BALL0_COLOR_B, 255);
	Color XhairGreenColor(BALL1_COLOR_R, BALL1_COLOR_G, BALL1_COLOR_B, 255);
	Color XhairYellowColor(BALL2_COLOR_R, BALL2_COLOR_G, BALL2_COLOR_B, 255);

	Color XhairBall3Color(vectronic_vecball3_color_r.GetInt(), vectronic_vecball3_color_g.GetInt(), vectronic_vecball3_color_b.GetInt(), 255);
	Color XhairBall4Color(vectronic_vecball4_color_r.GetInt(), vectronic_vecball4_color_g.GetInt(), vectronic_vecball4_color_b.GetInt(), 255);
	Color XhairBall5Color(vectronic_vecball5_color_r.GetInt(), vectronic_vecball5_color_g.GetInt(), vectronic_vecball5_color_b.GetInt(), 255);

	Color clr;

	m_intCurrentBall = pVecgun->GetCurrentSelection();
	if ( pVecgun->CanFireBlue() || pVecgun->CanFireGreen() || pVecgun->CanFireYellow() )
	{
		if (pVecgun->IsUsingCustomBalls() == false )
		{
			if ( m_intCurrentBall == 0 )
			{
				clr = XhairBlueColor;
			}
			else if ( m_intCurrentBall == 1 )
			{
				clr = XhairGreenColor;
			}
			else if ( m_intCurrentBall == 2 )
			{
				clr = XhairYellowColor;
			}
		}
		else
		{
			if ( m_intCurrentBall == 3 )
			{
				clr = XhairBall3Color;
			}
			else if ( m_intCurrentBall == 4 )
			{
				clr = XhairBall4Color;
			}
			else if ( m_intCurrentBall == 5 )
			{
				clr = XhairBall5Color;
			}
		}
	}
	else
	{
		clr = XhairWhiteColor;
	} 

	Color Finalclr = clr;

	const unsigned char iAlphaStart = 150;
	Finalclr[ 3 ] = iAlphaStart;

	float fireDelay = pVecgun->GetDelay();
	float DelayPerc = (float) fireDelay / 100.0f;
	DelayPerc = clamp( DelayPerc, 0.0f, 1.0f );

#ifndef SWARM_DLL
	gHUD.DrawIconProgressBar( xCenter - (m_icon_lb->Width() * 2), yCenter, m_icon_lb, m_icon_lbe, ( 1.0f - DelayPerc * 2 ), Finalclr, CHud::HUDPB_VERTICAL );
	gHUD.DrawIconProgressBar( xCenter + m_icon_rb->Width(), yCenter, m_icon_rb, m_icon_rbe, ( 1.0f - DelayPerc * 2 ), Finalclr, CHud::HUDPB_VERTICAL );
#else
	CHud().DrawIconProgressBar( xCenter - (m_icon_lb->Width() * 2), yCenter, m_icon_lb, m_icon_lbe, ( 1.0f - DelayPerc * 2 ), Finalclr, CHud::HUDPB_VERTICAL );
	CHud().DrawIconProgressBar( xCenter + m_icon_rb->Width(), yCenter, m_icon_rb, m_icon_rbe, ( 1.0f - DelayPerc * 2 ), Finalclr, CHud::HUDPB_VERTICAL );
#endif
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudVecCrosshair::UpdateEventTime( void )
{
	m_flLastEventTime = gpGlobals->curtime;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CHudVecCrosshair::EventTimeElapsed( void )
{
	if (( gpGlobals->curtime - m_flLastEventTime ) > QUICKINFO_EVENT_DURATION )
		return true;

	return false;
}

