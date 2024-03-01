#include "gamepadui_interface.h"
#include "gamepadui_basepanel.h"
#include "gamepadui_backgroundmovie.h"

#include "vgui/ISurface.h"
#include "vgui/ILocalize.h"
#include "vgui/IVGui.h"

#ifdef BINK_VIDEO
#include "avi\ibik.h"
extern IBik* bik;
#else
// TODO: Add AVI support! (Klaxon)
#endif 

#include "KeyValues.h"
#include "filesystem.h"
#include "tier0/memdbgon.h"

#define GAMEPADUI_BACKGROUNDMOVIE_MATERIALNAME "GamepadUIBIKMaterial"

GamepadUIMovieBackground::GamepadUIMovieBackground()
{
	m_nBikMaterial = BIKMATERIAL_INVALID;
	m_nTextureId = -1;

}

void GamepadUIMovieBackground::SetMovie(const char* szName) 
{
	if (m_nBikMaterial != BIKMATERIAL_INVALID)
		Clear();

	char szMaterial[MAX_PATH];
	Q_snprintf(szMaterial, sizeof(szMaterial), "BackgroundBIKMaterial%i", bik->GetGlobalMaterialAllocationNumber());
	m_nBikMaterial = bik->CreateMaterial(szMaterial, szName, "GAME", BIK_LOOP);
	m_szMaterialName = szMaterial;

	// TODO: AVI support!
}

void GamepadUIMovieBackground::Update()
{
	if (m_nBikMaterial == BIKMATERIAL_INVALID)
		return;

	// update bink
	if (bik->ReadyForSwap(m_nBikMaterial))
		if (!bik->Update(m_nBikMaterial)) {
			Clear(false);
			return;
		}
	
	if (m_nTextureId == -1)
		m_nTextureId = g_pMatSystemSurface->CreateNewTextureID(true);

	// draw!
	g_pMatSystemSurface->DrawSetTextureMaterial(m_nTextureId, bik->GetMaterial(m_nBikMaterial));

	int x, y, w, h;
	GamepadUI::GetInstance().GetBasePanel()->GetBounds(x, y, w, h);

	// center, 16:9 aspect ratio
	int width_at_ratio = h * (16.0f / 9.0f);
	x = (w * 0.5f) - (width_at_ratio * 0.5f);

	vgui::surface()->DrawSetColor(255, 255, 255, 255);
	vgui::surface()->DrawTexturedRect(x, y, x + width_at_ratio, y + h);
}

void GamepadUIMovieBackground::Clear(bool bDestroyTexture)
{
	if (m_nBikMaterial == BIKMATERIAL_INVALID)
		return;

	bik->DestroyMaterial(m_nBikMaterial);
	m_nBikMaterial = BIKMATERIAL_INVALID;

	if (bDestroyTexture)
		m_nTextureId = -1;
}