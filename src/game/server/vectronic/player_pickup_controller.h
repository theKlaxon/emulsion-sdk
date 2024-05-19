//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef PLAYER_PICKUP_CONTROLLER_H
#define PLAYER_PICKUP_CONTROLLER_H
#ifdef _WIN32
#pragma once
#endif

class CGrabController;

bool PlayerPickupControllerIsHoldingEntity( CBaseEntity *pPickupController, CBaseEntity *pHeldEntity );
void ShutdownPickupController( CBaseEntity *pPickupControllerEntity );
float PlayerPickupGetHeldObjectMass( CBaseEntity *pPickupControllerEntity, IPhysicsObject *pHeldObject );

CBaseEntity *GetPlayerHeldEntity( CBasePlayer *pPlayer );
CBasePlayer *GetPlayerHoldingEntity( CBaseEntity *pEntity );

/*
CGrabController *GetGrabControllerForPlayer( CBasePlayer *pPlayer );
CGrabController *GetGrabControllerForPhysCannon( CBaseCombatWeapon *pActiveWeapon );
void GetSavedParamsForCarriedPhysObject( CGrabController *pGrabController, IPhysicsObject *pObject, float *pSavedMassOut, float *pSavedRotationalDampingOut );
void UpdateGrabControllerTargetPosition( CBasePlayer *pPlayer, Vector *vPosition, QAngle *qAngles );
bool PhysCannonAccountableForObject( CBaseCombatWeapon *pPhysCannon, CBaseEntity *pObject );
*/

#endif // PLAYER_PICKUP_CONTROLLER_H