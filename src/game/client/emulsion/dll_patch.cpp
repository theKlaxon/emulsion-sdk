// ==== Emulsion Dll Patching Tools - Klaxon#3616 ============
// Use wisely! Can cause crashes if used incorrectly!
// ===========================================================

#include "cbase.h"
#include "dll_patch.h"
#include <memory>

CMatSysPatch g_MatSysPatch = CMatSysPatch();

// apply all patches
void PatchAll() {
	g_MatSysPatch.Patch();
}

void UnPatchAll() {
	g_MatSysPatch.UnPatch();
}

// Material System
void CMatSysPatch::Patch() {
#ifdef EMULSION_DLL
	// get the paint colour array, valve overrides the reflect colour using 
	// the portal colour for some stupid reason (thats why i'm here)
	Color* g_PaintColors = (Color*)(m_Offsets.g_PaintColors.GetAddress());

	// set the new colour
	static ConVarRef s_bounce_paint_color("bounce_paint_color");
	static ConVarRef s_reflect_paint_color("reflect_paint_color");
	static ConVarRef s_speed_paint_color("speed_paint_color");
	static ConVarRef s_portal_paint_color("portal_paint_color");
	static ConVarRef s_fifth_paint_color("fifth_paint_color");
	static ConVarRef s_sixth_paint_color("sixth_paint_color");
	static ConVarRef s_seventh_paint_color("seventh_paint_color");
	static ConVarRef s_erase_color("erase_color");

	g_PaintColors[BOUNCE_POWER]		= s_bounce_paint_color.GetColor();
	g_PaintColors[REFLECT_POWER]	= s_reflect_paint_color.GetColor();
	g_PaintColors[SPEED_POWER]		= s_speed_paint_color.GetColor();
	g_PaintColors[PORTAL_POWER]		= s_portal_paint_color.GetColor();

	// save the old data that was past the end of the colour array
	m_pOldData[0] = g_PaintColors[FIFTH_POWER];
	m_pOldData[1] = g_PaintColors[SIXTH_POWER];
	m_pOldData[2] = g_PaintColors[SEVENTH_POWER];

	g_PaintColors[FIFTH_POWER]		= s_fifth_paint_color.GetColor();
	g_PaintColors[SIXTH_POWER]		= s_sixth_paint_color.GetColor();
	g_PaintColors[SEVENTH_POWER]	= s_seventh_paint_color.GetColor();

	g_PaintColors[NO_POWER]			= s_erase_color.GetColor();

#ifdef DEBUG
	Msg("Bounce Color: (%i, %i, %i)\n", g_PaintColors[BOUNCE_POWER].r(), g_PaintColors[BOUNCE_POWER].g(), g_PaintColors[BOUNCE_POWER].b());
	Msg("Reflect Color: (%i, %i, %i)\n", g_PaintColors[REFLECT_POWER].r(), g_PaintColors[REFLECT_POWER].g(), g_PaintColors[REFLECT_POWER].b());
	Msg("Speed Color: (%i, %i, %i)\n", g_PaintColors[SPEED_POWER].r(), g_PaintColors[SPEED_POWER].g(), g_PaintColors[SPEED_POWER].b());
	Msg("Portal Color: (%i, %i, %i)\n", g_PaintColors[PORTAL_POWER].r(), g_PaintColors[PORTAL_POWER].g(), g_PaintColors[PORTAL_POWER].b());
	Msg("Fifth Color: (%i, %i, %i)\n", g_PaintColors[FIFTH_POWER].r(), g_PaintColors[FIFTH_POWER].g(), g_PaintColors[FIFTH_POWER].b());
	Msg("Sixth Color: (%i, %i, %i)\n", g_PaintColors[SIXTH_POWER].r(), g_PaintColors[SIXTH_POWER].g(), g_PaintColors[SIXTH_POWER].b());
	Msg("Seventh Color: (%i, %i, %i)\n", g_PaintColors[SEVENTH_POWER].r(), g_PaintColors[SEVENTH_POWER].g(), g_PaintColors[SEVENTH_POWER].b());
	Msg("Erase Color: (%i, %i, %i)\n", g_PaintColors[NO_POWER].r(), g_PaintColors[NO_POWER].g(), g_PaintColors[NO_POWER].b());
#endif

	g_PaintColors = nullptr;
#endif
}

void CMatSysPatch::UnPatch() {

	Color* g_PaintColors = (Color*)(m_Offsets.g_PaintColors.GetAddress());

	// put the data back, or suffer the consequences >:(
	g_PaintColors[FIFTH_POWER] = m_pOldData[0];
	g_PaintColors[SIXTH_POWER] = m_pOldData[1];
	g_PaintColors[SEVENTH_POWER] = m_pOldData[2];
}