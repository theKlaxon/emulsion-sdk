#include "cbase.h"
#include "proxy_imaterialsystem.h"
#include "proxy_filesystem.h"

IMaterialSysASW g_matsysasw;
IMaterialSysASW* g_pMaterialSysASW = &g_matsysasw;
IMaterialSysASW* materiASW = &g_matsysasw;

IFileSysPrx g_filesysasw;
IFileSysPrx* g_pFullFileSysPrx = &g_filesysasw;