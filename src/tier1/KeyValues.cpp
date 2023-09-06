#include "cbase.h"
#include <keyvalues.h>
#include "filesystem.h"
#include <vstdlib/ikeyvaluessystem.h>

#include <color.h>
#include <stdlib.h>
#include <ctype.h>
#include "tier1/convar.h"
#include "tier0/dbg.h"
#include "tier0/mem.h"
#include "utlvector.h"
#include "utlbuffer.h"
#include "utlhash.h"
#include "tier0/vprof.h"

// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/memdbgon.h>

static char* s_LastFileLoadingFromEX = "unknown"; // just needed for error messages

bool KeyValues::LoadFromFileEX(IBaseFileSystem* filesystem, const char* resourceName, const char* pathID, GetSymbolProc_t pfnEvaluateSymbolProc)
{
	//TM_ZONE_FILTERED( TELEMETRY_LEVEL0, 50, TMZF_NONE, "%s %s", __FUNCTION__, tmDynamicString( TELEMETRY_LEVEL0, resourceName ) );

	FileHandle_t f = filesystem->Open(resourceName, "rb", pathID);
	if (!f)
		return false;

	s_LastFileLoadingFromEX = (char*)resourceName;

	// load file into a null-terminated buffer
	int fileSize = filesystem->Size(f);
	unsigned bufSize = ((IFileSystem*)filesystem)->GetOptimalReadSize(f, fileSize);

	char* buffer = (char*)(((IFileSystem*)filesystem)->AllocOptimalReadBuffer(f, bufSize, 0));
	Assert(buffer);

	// read into local buffer
	bool bRetOK = (((IFileSystem*)filesystem)->ReadEx(buffer, bufSize, fileSize, f) != 0);

	filesystem->Close(f);	// close file after reading (duh)

	if (bRetOK)
	{
		buffer[fileSize] = 0; // null terminate file as EOF
		buffer[fileSize + 1] = 0; // double NULL terminating in case this is a unicode file
		bRetOK = LoadFromBuffer(resourceName, buffer, filesystem, pathID, pfnEvaluateSymbolProc);
	}

	((IFileSystem*)filesystem)->FreeOptimalReadBuffer(buffer);

	return bRetOK;
}