//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef VECTRONIC_PLAYER_SHARED_H
#define VECTRONIC_PLAYER_SHARED_H
#ifdef _WIN32
#pragma once
#endif

// Shared header file for players
#if defined( CLIENT_DLL )
#define C_VectronicPlayer CVectronicPlayer	//FIXME: Lovely naming job between server and client here...
#include "vectronic/c_vectronic_player.h"
#else
#include "vectronic/vectronic_player.h"
#endif

#endif // HL2_PLAYER_SHARED_H