#pragma once
#include "gamepadui_interface.h"

#ifdef BINK_VIDEO
#include "avi\ibik.h"
#else
// TODO: Add AVI support! (Klaxon)
#endif 

class GamepadUIMovieBackground {
public:
	GamepadUIMovieBackground();

	void SetMovie(const char* szName);
	void Update();
	void Clear(bool bDestroyTexture = true);

private:

	BIKMaterial_t m_nBikMaterial;
	const char* m_szMaterialName;
	int m_nTextureId;
	

};