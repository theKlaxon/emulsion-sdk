// ==== Emulsion Dll Patching Tools - Klaxon#3616 ============
// Use wisely! Can cause crashes if used incorrectly!
// ===========================================================

//#include "cbase.h"
#include "dll_patch.h"
#include <memory>

//ConVar debug_paint_alpha("debug_paint_alpha", "0", FCVAR_DEVELOPMENTONLY);

CMatSysPatch g_MatSysPatch = CMatSysPatch();
CEnginePatch g_EnginePatch = CEnginePatch();

CClientPatch g_ClientPatch = CClientPatch();
CServerPatch g_ServerPatch = CServerPatch();

// apply all patches
void PatchAll() {
	g_MatSysPatch.Patch();
	g_EnginePatch.Patch();
	g_ClientPatch.Patch();
	g_ServerPatch.Patch();
}

void CMatSysPatch::Patch() {
	
}

void CEnginePatch::Patch() {
	
}

void CClientPatch::Patch() {

}

void CServerPatch::Patch() {

}