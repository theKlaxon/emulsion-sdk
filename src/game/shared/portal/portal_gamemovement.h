#pragma once
#include "gamemovement.h"
#ifndef CLIENT_DLL
#include "portal_player.h"
#else
#include "c_portal_player.h"
#endif
#include "portal_util_shared.h"

//-----------------------------------------------------------------------------
// Purpose: Portal specific movement code
//-----------------------------------------------------------------------------
class CPortalGameMovement : public CGameMovement// CHL2GameMovement
{
	typedef CGameMovement BaseClass;
public:

	CPortalGameMovement();

	bool	m_bInPortalEnv;
	// Overrides
	virtual void ProcessMovement(CBasePlayer* pPlayer, CMoveData* pMove);
	virtual bool CheckJumpButton(void);

	void FunnelIntoPortal(CProp_Portal* pPortal, Vector& wishdir);

	virtual void AirAccelerate(Vector& wishdir, float wishspeed, float accel);
	virtual void AirMove(void);

	virtual void PlayerRoughLandingEffects(float fvol);

	virtual void CategorizePosition(void);

	// Traces the player bbox as it is swept from start to end
	virtual void TracePlayerBBox(const Vector& start, const Vector& end, unsigned int fMask, int collisionGroup, trace_t& pm);

	// Tests the player position
	virtual CBaseHandle	TestPlayerPosition(const Vector& pos, int collisionGroup, trace_t& pm);

	virtual void Duck(void);				// Check for a forced duck

	virtual int CheckStuck(void);

	virtual void SetGroundEntity(trace_t* pm);

private:
	
	CPortal_Player* GetPortalPlayer();
};