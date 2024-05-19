//======== Copyright © 2013 - 2014, rHetorical, All rights reserved. ==========
//
// Purpose:
//		
//=============================================================================

#include "cbase.h"
#include "model_types.h"
#include "c_weapon_vecgun.h"
#ifndef SWARM_DLL
#include "clienteffectprecachesystem.h"
#endif
#include "basevectroniccombatweapon_shared.h"
#include "fx.h"
#include "c_te_effect_dispatch.h"
#include "beamdraw.h"

IMPLEMENT_CLIENTCLASS_DT( C_WeaponVecgun, DT_WeaponVecgun, CWeaponVecgun )
	RecvPropBool( RECVINFO(m_bCanFireBlue) ),
	RecvPropBool( RECVINFO(m_bCanFireGreen) ),
	RecvPropBool( RECVINFO(m_bCanFireYellow) ),
	RecvPropBool( RECVINFO(m_bUseCustoms) ),
	RecvPropInt( RECVINFO( m_nNumShotsFired ) ),
	RecvPropInt( RECVINFO( m_nCurrentSelection ) ),
	RecvPropInt( RECVINFO( m_nDelay ) ),
END_RECV_TABLE()

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
 C_WeaponVecgun:: C_WeaponVecgun( void )
{
}