#include "cbase.h"
#include "bspfile.h"
#include "filesystem.h"
#include <Windows.h>

namespace LightmapLoad {

	FileHandle_t g_hFile = null;
	dlightmappageinfo_t* g_Pages = nullptr;
	int g_nPageCnt = -1;

	void BeginLoadLump(uint lump, lump_t& _lump) {

		// Get the map path
		const char* pszMapName = modelinfo->GetModelName(modelinfo->GetModel(1));

		// Open map
		g_hFile = g_pFullFileSystem->Open(pszMapName, "rb");
		if (!g_hFile) {
			Warning("[LightmapLoad] Unable to open map\n");
			return;
		}

		// Read the BSP header. We don't need to do any version checks, etc. as we
		// can safely assume that the engine did this for us
		BSPHeader_t hdr;
		g_pFullFileSystem->Read(&hdr, sizeof(hdr), g_hFile);

		// Grab the light lump and seek to it
		_lump = hdr.lumps[lump];

		g_pFullFileSystem->Seek(g_hFile, _lump.fileofs, FILESYSTEM_SEEK_HEAD);
	}

	void EndLoadLump() {
		g_pFullFileSystem->Close(g_hFile);
	}

	void LoadPages() {

		lump_t& lightLump = lump_t();
		BeginLoadLump(LUMP_LIGHTING_HDR, lightLump);

		g_nPageCnt = lightLump.filelen / sizeof(dlightmappageinfo_t);

		g_Pages = new dlightmappageinfo_t[g_nPageCnt];
		g_pFullFileSystem->Read(g_Pages, lightLump.filelen, g_hFile);

		EndLoadLump();
	}

	void InstallLightmapLoader() {

		//DWORD oldFunc;
		//VirtualProtect()

	}
}