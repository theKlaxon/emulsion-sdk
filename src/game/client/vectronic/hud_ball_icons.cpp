//=========== Copyright © 2014, rHetorical, All rights reserved. =============
//
// Purpose: 
//		
//=============================================================================
//
//
// implementation of CHudBallIcons class
//
#include "cbase.h"
#include "hud.h"
#include "hud_macros.h"
#include "view.h"

#include "iclientmode.h"

#include <KeyValues.h>
#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui_controls/AnimationController.h>

using namespace vgui;

#include "hudelement.h"
#include "hud_numericdisplay.h"

#include "c_vectronic_player.h"
#include "c_weapon_vecgun.h"

#include "ConVar.h"

// Our sharedefs.
#include "vectronic_shareddefs.h"


extern ConVar crosshair;

extern ConVar vectronic_vecball3_color_r;
extern ConVar vectronic_vecball3_color_g;
extern ConVar vectronic_vecball3_color_b;

extern ConVar vectronic_vecball4_color_r;
extern ConVar vectronic_vecball4_color_g;
extern ConVar vectronic_vecball4_color_b;

extern ConVar vectronic_vecball5_color_r;
extern ConVar vectronic_vecball5_color_g;
extern ConVar vectronic_vecball5_color_b;

//-----------------------------------------------------------------------------
// Purpose: Health panel
//-----------------------------------------------------------------------------
class CHudBallIcons : public CHudElement, public vgui::Panel
{
	DECLARE_CLASS_SIMPLE( CHudBallIcons, vgui::Panel );

public:
	CHudBallIcons( const char *pElementName );
	virtual void Init( void );
	virtual void VidInit( void );
	virtual void Reset( void );
//	virtual void OnThink (void);
	bool ShouldDraw( void );

	virtual void Paint( void );
	virtual void ApplySchemeSettings( IScheme *scheme );

private:

	CHudTexture *m_pBall0Icon;
	CHudTexture *m_pBall1Icon;
	CHudTexture *m_pBall2Icon;

	CPanelAnimationVarAliasType( float, icon0_xpos, "icon0_xpos", "0", "proportional_float" );
	CPanelAnimationVarAliasType( float, icon0_ypos, "icon0_ypos", "0", "proportional_float" );

	CPanelAnimationVarAliasType( float, icon1_xpos, "icon1_xpos", "0", "proportional_float" );
	CPanelAnimationVarAliasType( float, icon1_ypos, "icon1_ypos", "0", "proportional_float" );

	CPanelAnimationVarAliasType( float, icon2_xpos, "icon2_xpos", "0", "proportional_float" );
	CPanelAnimationVarAliasType( float, icon2_ypos, "icon2_ypos", "0", "proportional_float" );

	float icon_tall;
	float icon_wide;

	bool m_bDraw;

};	

DECLARE_HUDELEMENT( CHudBallIcons );

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CHudBallIcons::CHudBallIcons( const char *pElementName ) : CHudElement (pElementName), BaseClass (NULL, "HudBallIcons")
{
#ifndef SWARM_DLL
	vgui::Panel *pParent = g_pClientMode->GetViewport();
#else
	vgui::Panel *pParent = GetClientMode()->GetViewport();
#endif
	SetParent (pParent);

	SetHiddenBits( HIDEHUD_PLAYERDEAD );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudBallIcons::Init()
{
}

void CHudBallIcons::ApplySchemeSettings( IScheme *scheme )
{
	BaseClass::ApplySchemeSettings( scheme );

	//m_cColor = scheme->GetColor( "Normal", Color( 255, 208, 64, 255 ) );

	SetPaintBackgroundEnabled( false );

	if( !m_pBall0Icon || !m_pBall1Icon || !m_pBall2Icon )
	{
#ifndef SWARM_DLL
		m_pBall0Icon = gHUD.GetIcon( "BallIcon" );
		m_pBall1Icon = gHUD.GetIcon( "BallIcon" );
		m_pBall2Icon = gHUD.GetIcon( "BallIcon" );
#else
		m_pBall0Icon = HudIcons().GetIcon( "BallIcon" );
		m_pBall1Icon = HudIcons().GetIcon( "BallIcon" );
		m_pBall2Icon = HudIcons().GetIcon( "BallIcon" );

#endif
	}	

	if( m_pBall0Icon )
	{
		icon_tall = GetTall() - YRES(2);
		float scale = icon_tall / (float)m_pBall0Icon->Height();
		icon_wide = ( scale ) * (float)m_pBall0Icon->Width() + 2;
	}

	if( m_pBall1Icon )
	{
		icon_tall = GetTall() - YRES(2);
		float scale = icon_tall / (float)m_pBall1Icon->Height();
		icon_wide = ( scale ) * (float)m_pBall1Icon->Width() + 2;
	}

	if( m_pBall2Icon )
	{
		icon_tall = GetTall() - YRES(2);
		float scale = icon_tall / (float)m_pBall2Icon->Height();
		icon_wide = ( scale ) * (float)m_pBall2Icon->Width() + 2;
	}

	//SetPaintBackgroundEnabled( false );
}

//-----------------------------------------------------------------------------
// Purpose: reset health to normal color at round restart
//-----------------------------------------------------------------------------
void CHudBallIcons::Reset()
{
//	g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("HealthRestored");
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudBallIcons::VidInit()
{
}
/*
//------------------------------------------------------------------------
// Purpose:
//------------------------------------------------------------------------
void CHudBallIcons:: OnThink (void)
{

//	ShouldDraw();
}
*/
bool CHudBallIcons::ShouldDraw( void )
{
	C_VectronicPlayer *local = C_VectronicPlayer ::GetLocalPlayer();

	if (!local)
		return false;

	if(!local->IsAlive() || local->IsObserver())
		return false;

	if (local->PlayerHasObject())
		return false;

	if ( !crosshair.GetBool() && !IsX360() )
		return false;

	return true;
}

void CHudBallIcons::Paint( void )
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

	Color WhiteColor(255,255,255,96);

	Color BlueColor(BALL0_COLOR_R, BALL0_COLOR_G, BALL0_COLOR_B, 255);
	Color FadedBlueColor(BALL0_COLOR_R, BALL0_COLOR_G, BALL0_COLOR_B, 150);

	Color GreenColor(BALL1_COLOR_R, BALL1_COLOR_G, BALL1_COLOR_B, 255);
	Color FadedGreenColor(BALL1_COLOR_R, BALL1_COLOR_G, BALL1_COLOR_B, 150);

	Color PurpleColor(BALL2_COLOR_R, BALL2_COLOR_G, BALL2_COLOR_B, 255);
	Color FadedPurpleColor(BALL2_COLOR_R, BALL2_COLOR_G, BALL2_COLOR_B, 150);

	Color Ball3Color(vectronic_vecball3_color_r.GetInt(), vectronic_vecball3_color_g.GetInt(), vectronic_vecball3_color_b.GetInt(), 255);
	Color FadedBall3Color(vectronic_vecball3_color_r.GetInt(), vectronic_vecball3_color_g.GetInt(), vectronic_vecball3_color_b.GetInt(), 150);

	Color Ball4Color(vectronic_vecball4_color_r.GetInt(), vectronic_vecball4_color_g.GetInt(), vectronic_vecball4_color_b.GetInt(), 255);
	Color FadedBall4Color(vectronic_vecball4_color_r.GetInt(), vectronic_vecball4_color_g.GetInt(), vectronic_vecball4_color_b.GetInt(), 150);

	Color Ball5Color(vectronic_vecball5_color_r.GetInt(), vectronic_vecball5_color_g.GetInt(), vectronic_vecball5_color_b.GetInt(), 255);
	Color FadedBall5Color(vectronic_vecball5_color_r.GetInt(), vectronic_vecball5_color_g.GetInt(), vectronic_vecball5_color_b.GetInt(), 150);

	if( !m_pBall0Icon || !m_pBall1Icon || !m_pBall2Icon )
		return;

	int m_intCurrentBall;
	m_intCurrentBall = pVecgun->GetCurrentSelection();

	if (pVecgun->IsUsingCustomBalls() == false )
	{
		if (pVecgun->CanFireBlue())
		{
			if ( m_intCurrentBall == 0 || m_intCurrentBall > 2)
			{
				m_pBall0Icon->DrawSelf( icon0_xpos, icon0_ypos, icon_wide, icon_tall, BlueColor );
			}
			else
			{
				m_pBall0Icon->DrawSelf( icon0_xpos, icon0_ypos, icon_wide, icon_tall, FadedBlueColor );
			}
		}
		else
		{
			m_pBall0Icon->DrawSelf( icon0_xpos, icon0_ypos, icon_wide, icon_tall, WhiteColor );
		}

		if (pVecgun->CanFireGreen())
		{
			if ( m_intCurrentBall == 1 )
			{
				m_pBall1Icon->DrawSelf( icon1_xpos, icon1_ypos, icon_wide, icon_tall, GreenColor );
			}
			else
			{
				m_pBall1Icon->DrawSelf( icon1_xpos, icon1_ypos, icon_wide, icon_tall, FadedGreenColor );
			}
		}
		else
		{
			m_pBall1Icon->DrawSelf( icon1_xpos, icon1_ypos, icon_wide, icon_tall, WhiteColor );
		}

		if (pVecgun->CanFireYellow())
		{
			if ( m_intCurrentBall == 2 )
			{
				m_pBall1Icon->DrawSelf( icon2_xpos, icon2_ypos, icon_wide, icon_tall, PurpleColor );
			}
			else
			{
				m_pBall1Icon->DrawSelf( icon2_xpos, icon2_ypos, icon_wide, icon_tall, FadedPurpleColor );
			}
		}
		else
		{
			m_pBall1Icon->DrawSelf( icon2_xpos, icon2_ypos, icon_wide, icon_tall, WhiteColor );
		}
	}
	else
	{
		if (pVecgun->CanFireBlue())
		{
			if ( m_intCurrentBall == 3 || m_intCurrentBall > 5)
			{
				m_pBall0Icon->DrawSelf( icon0_xpos, icon0_ypos, icon_wide, icon_tall, Ball3Color );
			}
			else
			{
				m_pBall0Icon->DrawSelf( icon0_xpos, icon0_ypos, icon_wide, icon_tall, FadedBall3Color );
			}
		}
		else
		{
			m_pBall0Icon->DrawSelf( icon0_xpos, icon0_ypos, icon_wide, icon_tall, WhiteColor );
		}

		if (pVecgun->CanFireGreen())
		{
			if ( m_intCurrentBall == 4 )
			{
				m_pBall1Icon->DrawSelf( icon1_xpos, icon1_ypos, icon_wide, icon_tall, Ball4Color );
			}
			else
			{
				m_pBall1Icon->DrawSelf( icon1_xpos, icon1_ypos, icon_wide, icon_tall, FadedBall4Color );
			}
		}
		else
		{
			m_pBall1Icon->DrawSelf( icon1_xpos, icon1_ypos, icon_wide, icon_tall, WhiteColor );
		}

		if (pVecgun->CanFireYellow())
		{
			if ( m_intCurrentBall == 5 )
			{
				m_pBall1Icon->DrawSelf( icon2_xpos, icon2_ypos, icon_wide, icon_tall, Ball5Color );
			}
			else
			{
				m_pBall1Icon->DrawSelf( icon2_xpos, icon2_ypos, icon_wide, icon_tall, FadedBall5Color );
			}
		}
		else
		{
			m_pBall1Icon->DrawSelf( icon2_xpos, icon2_ypos, icon_wide, icon_tall, WhiteColor );
		}
	}
	BaseClass::Paint();
}