#include "cbase.h"
#include "vgui_int.h"
#include "ienginevgui.h"
#include "vgui_controls/Panel.h"
#include "vgui/IVgui.h"
#include <vgui/ISurface.h>

using namespace vgui;

//-------------
// Root panel
//-------------
class C_VectronicRootPanel : public Panel {
	typedef Panel BaseClass;
public:
	C_VectronicRootPanel( VPANEL parent, int slot );
	~C_VectronicRootPanel();

	// Draw Panel effects here
	virtual void PostChildPaint();
	
	virtual void PaintTraverse( bool Repaint, bool allowForce = true );
	virtual void OnThink();

	// Render all panel effects
	void RenderPanelEffects(void);

	int m_nSplitSlot;
};

C_VectronicRootPanel::C_VectronicRootPanel( VPANEL parent, int slot )
	: BaseClass( NULL, "Vectronic Root Panel" ), m_nSplitSlot( slot ) {
	SetParent( parent );
	SetPaintEnabled( false );
	SetPaintBorderEnabled( false );
	SetPaintBackgroundEnabled( false );

	// This panel does post child painting
	SetPostChildPaintEnabled( true );

	int w, h;
	surface()->GetScreenSize(w, h);

	// Make it screen sized
	SetBounds( 0, 0, w, h );
	SetZPos(0);

	// Ask for OnTick messages
	ivgui()->AddTickSignal( GetVPanel() );
}

C_VectronicRootPanel::~C_VectronicRootPanel() {

}

void C_VectronicRootPanel::PostChildPaint() {
	BaseClass::PostChildPaint();

	// Draw all panel effects
	RenderPanelEffects();
}

void C_VectronicRootPanel::RenderPanelEffects() {

}

void C_VectronicRootPanel::PaintTraverse( bool Repaint, bool allowForce /*= true*/ )
{
	ACTIVE_SPLITSCREEN_PLAYER_GUARD( m_nSplitSlot);
	BaseClass::PaintTraverse( Repaint, allowForce );
}
void C_VectronicRootPanel::OnThink()
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
		g_pRootPanel[ i ] = new C_VectronicRootPanel( enginevgui->GetPanel( PANEL_CLIENTDLL ), i );
	}

	g_pFullscreenRootPanel = new C_VectronicRootPanel( enginevgui->GetPanel( PANEL_CLIENTDLL ), 0);
	g_pFullscreenRootPanel->SetZPos(1);
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
	ASSERT_LOCAL_PLAYER_RESOLVABLE(engine->IsLocalPlayerResolvable());
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
