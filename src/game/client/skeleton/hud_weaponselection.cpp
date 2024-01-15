#include "cbase.h"
#include "weapon_selection.h"
#include "vgui_controls/Panel.h"
#include "iclientmode.h"

class CHudWeaponSelection : public CBaseHudWeaponSelection, public vgui::Panel
{
	DECLARE_CLASS_SIMPLE( CHudWeaponSelection, vgui::Panel );

	CHudWeaponSelection( const char* pElementName );

	void ApplySchemeSettings(vgui::IScheme* pScheme);

	void CycleToNextWeapon() {}
	void CycleToPrevWeapon() {}
	void SelectWeaponSlot(int iSlot) {}

	C_BaseCombatWeapon* GetWeaponInSlot( int iSlot, int iSlotPos ) { return null; }
	C_BaseCombatWeapon* GetSelectedWeapon() { return null; }
};

DECLARE_HUDELEMENT( CHudWeaponSelection );

CHudWeaponSelection::CHudWeaponSelection( const char* pElementName ) : CBaseHudWeaponSelection(pElementName), BaseClass(NULL, "HudWeaponSelection")
{
	SetParent( GetClientMode()->GetViewport() );
}

void CHudWeaponSelection::ApplySchemeSettings(vgui::IScheme* pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);
	SetPaintBackgroundEnabled(false);

	int x, y;
	GetPos(x, y);
	int screenWide, screenTall;
	GetHudSize(screenWide, screenTall);

	SetBounds( x, y, screenWide - x, screenTall - y );
}