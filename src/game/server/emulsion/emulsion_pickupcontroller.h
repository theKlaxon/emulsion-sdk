#pragma once
#include "player.h"
#include "player_pickup.h"

// globals
void PlayerPickupObject(CBasePlayer* pPlayer, CBaseEntity* pObject);
float PlayerPickupGetHeldObjectMass(CBaseEntity* pPickupControllerEntity, IPhysicsObject* pHeldObject);
bool PlayerPickupControllerIsHoldingEntity(CBaseEntity* pPickupControllerEntity, CBaseEntity* pHeldEntity);

// derive from this so we can add save/load data to it
struct game_shadowcontrol_params_t : public hlshadowcontrol_params_t
{
	DECLARE_SIMPLE_DATADESC();
};

struct thrown_objects_t
{
	float				fTimeThrown;
	EHANDLE				hEntity;

	DECLARE_SIMPLE_DATADESC();
};

class CGrabController : public IMotionEvent
{
	DECLARE_SIMPLE_DATADESC();

public:

	CGrabController(void);
	~CGrabController(void);
	void AttachEntity(CBasePlayer* pPlayer, CBaseEntity* pEntity, IPhysicsObject* pPhys, bool bIsMegaPhysCannon, const Vector& vGrabPosition, bool bUseGrabPosition);
	void DetachEntity(bool bClearVelocity);
	void OnRestore();

	bool UpdateObject(CBasePlayer* pPlayer, float flError);

	void SetTargetPosition(const Vector& target, const QAngle& targetOrientation);
	float ComputeError();
	float GetLoadWeight(void) const { return m_flLoadWeight; }
	void SetAngleAlignment(float alignAngleCosine) { m_angleAlignment = alignAngleCosine; }
	void SetIgnorePitch(bool bIgnore) { m_bIgnoreRelativePitch = bIgnore; }
	QAngle TransformAnglesToPlayerSpace(const QAngle& anglesIn, CBasePlayer* pPlayer);
	QAngle TransformAnglesFromPlayerSpace(const QAngle& anglesIn, CBasePlayer* pPlayer);

	CBaseEntity* GetAttached() { return (CBaseEntity*)m_attachedEntity; }

	IMotionEvent::simresult_e Simulate(IPhysicsMotionController* pController, IPhysicsObject* pObject, float deltaTime, Vector& linear, AngularImpulse& angular);
	float GetSavedMass(IPhysicsObject* pObject);

	bool IsObjectAllowedOverhead(CBaseEntity* pEntity);

private:
	// Compute the max speed for an attached object
	void ComputeMaxSpeed(CBaseEntity* pEntity, IPhysicsObject* pPhysics);

	game_shadowcontrol_params_t	m_shadow;
	float			m_timeToArrive;
	float			m_errorTime;
	float			m_error;
	float			m_contactAmount;
	float			m_angleAlignment;
	bool			m_bCarriedEntityBlocksLOS;
	bool			m_bIgnoreRelativePitch;

	float			m_flLoadWeight;
	float			m_savedRotDamping[VPHYSICS_MAX_OBJECT_LIST_COUNT];
	float			m_savedMass[VPHYSICS_MAX_OBJECT_LIST_COUNT];
	EHANDLE			m_attachedEntity;
	QAngle			m_vecPreferredCarryAngles;
	bool			m_bHasPreferredCarryAngles;
	float			m_flDistanceOffset;

	QAngle			m_attachedAnglesPlayerSpace;
	Vector			m_attachedPositionObjectSpace;

	IPhysicsMotionController* m_controller;

	bool			m_bAllowObjectOverhead; // Can the player hold this object directly overhead? (Default is NO)

	// NVNT player controlling this grab controller
	CBasePlayer* m_pControllingPlayer;

	friend class CWeaponPhysCannon;
};

class CPlayerPickupController : public CBaseEntity
{
	DECLARE_DATADESC();
	DECLARE_CLASS(CPlayerPickupController, CBaseEntity);
public:
	void Init(CBasePlayer* pPlayer, CBaseEntity* pObject);
	void Shutdown(bool bThrown = false);
	bool OnControls(CBaseEntity* pControls) { return true; }
	void Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value);
	void OnRestore()
	{
		m_grabController.OnRestore();
	}
	void VPhysicsUpdate(IPhysicsObject* pPhysics) {}
	void VPhysicsShadowUpdate(IPhysicsObject* pPhysics) {}

	bool IsHoldingEntity(CBaseEntity* pEnt);
	CGrabController& GetGrabController() { return m_grabController; }

private:
	CGrabController		m_grabController;
	CBasePlayer* m_pPlayer;
};
