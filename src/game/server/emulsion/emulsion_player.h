#include "cbase.h"
#include "predicted_viewmodel.h"
#include "player_pickup.h"
#include "emulsion_pickupcontroller.h"
#include "emulsion_gamemovement.h"
#include "igamemovement.h"

extern IGameMovement* g_pGameMovement;

class CEmulsionPlayer : public CBasePlayer {
public:
	DECLARE_CLASS(CEmulsionPlayer, CBasePlayer );
	DECLARE_NETWORKCLASS();

	CEmulsionPlayer();

	virtual void Precache();
	virtual void Spawn();
	virtual void Activate();

	virtual int		UpdateTransmitState() override { return SetTransmitState(FL_EDICT_ALWAYS); }

	void			CalcPlayerView(Vector& eyeOrigin, QAngle& eyeAngles, float& fov);
	void			CreateViewModel(int index);
	virtual void	Touch(CBaseEntity* pOther) override;
	virtual void	EndTouch(CBaseEntity* pOther) override;
	void			FireBullets(const FireBulletsInfo_t& info);

	virtual void	PlayerUse();
	virtual void	PickupObject(CBaseEntity* pObject, bool bLimitMassAndSize);
	virtual	bool	IsHoldingEntity(CBaseEntity* pEnt);
	virtual float	GetHeldObjectMass(IPhysicsObject* pHeldObject);

	void			SetGravityDir(Vector axis);
	void			SetStickParent(CBaseEntity* pParent);
	CBaseEntity*	GetStickParent();

	virtual void	PreThink(void) override;
	void			ProcessPowerUpdate();
	void			BouncePlayer(cplane_t plane);
	void			StickPlayer(PaintInfo_t info);
	void			UnStickPlayer();
	void			RotateBBox(Vector vecUp);

	Vector GetHalfHeight_Stick();
	Vector GetForward_Stick();
	
	friend class CEmulsionGameMovement;

protected:
	
	CEmulsionGameMovement* pMove;

	PaintInfo_t m_tCurPaintInfo;
	CBaseEntity* m_pStickParent;

	VMatrix m_mEyeRotationMat;
	VMatrix m_mGravityTransform;

	QAngle m_angInitialAngles;

	Vector m_vecCurLerpUp;
	//Vector m_vecGravity;
	Vector m_vecEyeAxisRot;

	float m_flEyeRotation;
	bool m_bPlayUseDenySound;
	bool m_bIsTouchingStickParent;

	CNetworkHandle(CBaseEntity, m_hStickParent);
	CNetworkVar(bool, m_bPlayerPickedUpObject);
	CNetworkVar(int, m_nPaintPower);
	CNetworkVector(m_vecGravity);
};

inline CEmulsionPlayer* ToEmulsionPlayer(CBaseEntity* pEntity) {
	if (!pEntity || !pEntity->IsPlayer())
		return NULL;

#ifdef _DEBUG
	Assert(dynamic_cast<CEmulsionPlayer*>(pEntity) != 0);
#endif
	return static_cast<CEmulsionPlayer*>(pEntity);
}