#include "cbase.h"
#include "predicted_viewmodel.h"
#include "player_pickup.h"
#include "emulsion_pickupcontroller.h"

class CEmulsionPlayer : public CBasePlayer {
public:
	DECLARE_CLASS(CEmulsionPlayer, CBasePlayer );
	DECLARE_NETWORKCLASS();

	CEmulsionPlayer();

	virtual void Precache();
	virtual void Spawn();
	virtual void Activate();

	virtual int UpdateTransmitState() override { return SetTransmitState(FL_EDICT_ALWAYS); }

	//void CalcPlayerView(Vector& eyeOrigin, QAngle& eyeAngles, float& fov);
	void CreateViewModel( int index );
	void PostThink();
	void FireBullets ( const FireBulletsInfo_t &info );

	virtual void	PlayerUse();
	virtual void	PickupObject(CBaseEntity* pObject, bool bLimitMassAndSize);
	virtual	bool	IsHoldingEntity(CBaseEntity* pEnt);
	virtual float	GetHeldObjectMass(IPhysicsObject* pHeldObject);
	
	void SetGravityDir(Vector axis);
	void RotBoundingBox(VMatrix mat);

	void SetStickParent(CBaseEntity* pParent);
	CBaseEntity* GetStickParent();
	
	friend class CEmulsionGameMovement;

protected:
	
	CBaseEntity* m_pStickParent;

	VMatrix m_mEyeRotationMat;
	VMatrix m_mGravityTransform;
	
	QAngle m_angInitialAngles;

	Vector m_vecGravity;
	Vector m_vecPrevGravity;
	Vector m_vecEyeAxisRot;

	float m_flEyeRotation;
	bool m_bPlayUseDenySound;

	CNetworkVar(bool, m_bPlayerPickedUpObject);
	CNetworkVar(int, m_iPaintPower);
};

inline CEmulsionPlayer* ToEmulsionPlayer(CBaseEntity* pEntity) {
	if (!pEntity || !pEntity->IsPlayer())
		return NULL;

#ifdef _DEBUG
	Assert(dynamic_cast<CEmulsionPlayer*>(pEntity) != 0);
#endif
	return static_cast<CEmulsionPlayer*>(pEntity);
}