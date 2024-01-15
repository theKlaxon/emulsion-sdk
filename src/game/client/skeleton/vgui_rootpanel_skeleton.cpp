#include "cbase.h"
#include "vgui_int.h"
#include "ienginevgui.h"
#include "vgui_controls/Panel.h"
#include "vgui/IVgui.h"

using namespace vgui;

//-------------
// Root panel
//-------------
class C_SkeletonRootPanel : public Panel
{
	typedef Panel BaseClass;
public:
	C_SkeletonRootPanel( VPANEL parent, int slot );

	virtual void PaintTraverse( bool Repaint, bool allowForce = true );
	virtual void OnThink();

	int m_nSplitSlot;
};

C_SkeletonRootPanel::C_SkeletonRootPanel( VPANEL parent, int slot )
	: BaseClass( NULL, "SDK Root Panel" ), m_nSplitSlot( slot )
{
	SetParent( parent );
	SetPaintEnabled( false );
	SetPaintBorderEnabled( false );
	SetPaintBackgroundEnabled( false );

	// This panel does post child painting
	SetPostChildPaintEnabled( true );

	// Make it screen sized
	SetBounds( 0, 0, ScreenWidth(), ScreenHeight() );

	// Ask for OnTick messages
	ivgui()->AddTickSignal( GetVPanel() );
}

void C_SkeletonRootPanel::PaintTraverse( bool Repaint, bool allowForce /*= true*/ )
{
	ACTIVE_SPLITSCREEN_PLAYER_GUARD( m_nSplitSlot);
	BaseClass::PaintTraverse( Repaint, allowForce );
}
void C_SkeletonRootPanel::OnThink()
{
	ACTIVE_SPLITSCREEN_PLAYER_GUARD( m_nSplitSlot );
	BaseClass::OnThink();
}

//----------
// Globals
//----------
static Panel *g_pRootPanel[ MAX_SPLITSCREEN_PLAYERS ];
static Panel *g_pFullscreenRootPanel;

void VGUI_CreateClientDLLRootPanel( )
{
	for ( int i = 0 ; i < MAX_SPLITSCREEN_PLAYERS; ++i )
	{
		g_pRootPanel[ i ] = new C_SkeletonRootPanel( enginevgui->GetPanel( PANEL_CLIENTDLL ), i );
	}

	g_pFullscreenRootPanel = new C_SkeletonRootPanel( enginevgui->GetPanel( PANEL_CLIENTDLL ), 0 );
	g_pFullscreenRootPanel->SetZPos( 1 );
}

void VGUI_DestroyClientDLLRootPanel( void )
{
	for ( int i = 0 ; i < MAX_SPLITSCREEN_PLAYERS; ++i )
	{
		delete g_pRootPanel[ i ];
		g_pRootPanel[ i ] = NULL;
	}

	delete g_pFullscreenRootPanel;
	g_pFullscreenRootPanel = NULL;
}

void VGui_GetPanelList( CUtlVector< Panel * > &list )
{
	for ( int i = 0 ; i < MAX_SPLITSCREEN_PLAYERS; ++i )
	{
		list.AddToTail( g_pRootPanel[ i ] );
	}
}

VPANEL VGui_GetClientDLLRootPanel( void )
{
	//Assert( engine->IsLocalPlayerResolvable() );
	return g_pRootPanel[ engine->GetActiveSplitScreenPlayerSlot() ]->GetVPanel();
}

//-----------------------------------------------------------------------------
// Purpose: Fullscreen root panel for shared hud elements during splitscreen
// Output : Panel
//-----------------------------------------------------------------------------
Panel *VGui_GetFullscreenRootPanel( void )
{
	return g_pFullscreenRootPanel;
}

VPANEL VGui_GetFullscreenRootVPANEL( void )
{
	return g_pFullscreenRootPanel->GetVPanel();
}
