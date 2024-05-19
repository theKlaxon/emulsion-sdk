//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef VECTRONIC_WEAPON_PARSE_H
#define VECTRONIC_PARSE_H
#ifdef _WIN32
#pragma once
#endif


#include "weapon_parse.h"
#include "networkvar.h"


//--------------------------------------------------------------------------------------------------------
class CVectronicWeaponInfo : public FileWeaponInfo_t
{
public:
	DECLARE_CLASS_GAMEROOT( CVectronicWeaponInfo, FileWeaponInfo_t );
	
	CVectronicWeaponInfo();
	
	virtual void Parse( ::KeyValues *pKeyValuesData, const char *szWeaponName );


public:

	int		m_iPlayerDamage;
//	int		m_iBall0;
//	int		m_iBall1;
//	int		m_iBall2;
};


#endif // HL2MP_WEAPON_PARSE_H
