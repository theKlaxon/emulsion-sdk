#pragma once
#include "gamemovement.h"
#include "igamemovement.h"
#include "vphysics_interface.h"
#include "game/shared/portal2/paint_enum.h"
#include "emulsion_shareddefs.h"

static bool g_bInStickTransition = false;

class CEmulsionGameMovement : public CGameMovement {
public:
	DECLARE_CLASS(CEmulsionGameMovement, CGameMovement);

	CEmulsionGameMovement();

	void ProcessMovement(CBasePlayer* pPlayer, CMoveData* pMove);
	void CategorizePosition();
	void PlayerMove();
	void FullWalkMove();
	void WalkMove();
	void AirMove();
	void Friction();
	void CheckParameters();
	void CheckFalling();

	void StayOnGround();
	void StartGravity();
	void FinishGravity();
	void SetGroundEntity(trace_t* pm);

	bool CheckJumpButton();
	int	TryPlayerMove(Vector* pFirstDest = NULL, trace_t* pFirstTrace = NULL);

	// paint specifics
	void ProcessPowerUpdate();
	void BouncePlayer(cplane_t plane);
	void StickPlayer(PaintInfo_t info);
	void UnStickPlayer();

	Vector GetGravityDir() { return m_vecGravity; }
	void SetGravityDir(Vector dir) { m_vecGravity = dir; }

	void PlayPaintEntrySound(PaintPowerType type, bool force = false);
	void PlayPaintExitSound(PaintPowerType type);
	void DetermineExitSound(PaintPowerType type);

	void CalculateStickAngles();

	friend class CEmulsionPlayer;
	friend class C_EmulsionPlayer;

protected:

	void PlaySoundInternal(const char* sound);

	Vector m_vecGravity;
	Vector m_vecPrevGravity;
	PaintInfo_t m_tCurPaintInfo;											// the player's current paint power;

	VMatrix m_mGravityTransform;
	matrix3x4_t m_mStickMatrix;

	Quaternion m_qRotFrom;
	Quaternion m_qRotTo;

	QAngle m_angDefaultAngles;
	QAngle m_angStickAngles;

	Vector m_vecStickForward;
	Vector m_vecStickUp;

	bool m_bIsTouchingStickParent;

#ifdef GAME_DLL
	PaintInfo_t CheckPaintedSurface();										// get the paint(s) the player is touching
	PaintPowerType GetHighestPriorityPaint(CUtlVector<unsigned char>* paints);	// get the paint with the highest priority

private:
	float m_fCurPaintFrameDelay;
#endif
};

