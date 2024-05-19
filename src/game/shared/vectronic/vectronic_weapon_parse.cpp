//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include <KeyValues.h>
#include "vectronic_weapon_parse.h"
#include "ammodef.h"

FileWeaponInfo_t* CreateWeaponInfo()
{
	return new CVectronicWeaponInfo;
}

CVectronicWeaponInfo::CVectronicWeaponInfo()
{
	m_iPlayerDamage = 0;
}


void CVectronicWeaponInfo::Parse( KeyValues *pKeyValuesData, const char *szWeaponName )
{
	BaseClass::Parse( pKeyValuesData, szWeaponName );

	m_iPlayerDamage = pKeyValuesData->GetInt( "damage", 0 );

	//m_iBall0 = pKeyValuesData->GetInt( "ballslot0", 0 );
	//m_iBall1 = pKeyValuesData->GetInt( "ballslot1", 1 );
	//m_iBall2 = pKeyValuesData->GetInt( "ballslot2", 2 );
}


