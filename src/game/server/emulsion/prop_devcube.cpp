#include "cbase.h"
#include "props.h"
#include "lights.h"
#include "fire.h"
#include "emulsion_player.h"
#include "particle_parse.h"
#include "prop_floorbutton.h"

#define CUBE_WEIGHTED_MDL	"models/props/metal_box.mdl"
#define CUBE_LASER_MDL		"models/props/metal_box.mdl" // TODO
#define CUBE_EDGELESS_MDL	"models/props/metal_box.mdl" // TODO

enum CUBE_TYPE {
	CUBE_WEIGHTED = 0,
	CUBE_COMPANION,
	CUBE_LASER,
	CUBE_EDGELESS
};

enum CUBE_SKIN_WEIGHTED {
	CUBE_WEIGHTED_DEFAULT = 0,
	CUBE_WEIGHTED_COMPANION,
	CUBE_WEIGHTED_DEFAULT_ACTIVE,
	CUBE_WEIGHTED_DILAPIDATED,
	CUBE_WEIGHTED_COMPANION_ACTIVE,
	CUBE_WEIGHTED_DILAPIDATED_ACTIVE,
	CUBE_WEIGHTED_REPULSIVE,
	CUBE_WEIGHTED_PROPULSIVE,
	CUBE_WEIGHTED_ACTIVE_REPULSIVE,
	CUBE_WEIGHTED_ACTIVE_PROPULSIVE,
};

//enum CUBE_SKIN_LASER {
//	DEFAULT = 0,
//	DILAPIDATED,
//	REPULSIVE,
//	PROPULSIVE
//};
//
//enum CUBE_SKIN_EDGELESS {
//	DEFAULT = 0,
//	ACTIVATED,
//	REPULSIVE,
//	PROPULSIVE
//};

class CPropDevCube : public CBaseAnimating {
	DECLARE_CLASS(CPropDevCube, CBaseAnimating);
public:

	CPropDevCube();

	void Precache();
	void Spawn();
	void Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value);

	int ObjectCaps();

	// cube specifics
	void CubeChooseModel();
	void CubeActivate();
	void CubeDeactivate();

	// note: may be obsolete soon
	virtual bool IsWeightedCube() const { return true; }

	virtual void StartTouch(CBaseEntity* pOther);
	virtual void EndTouch(CBaseEntity* pOther);

protected:

	CUBE_TYPE m_tType;

};

LINK_ENTITY_TO_CLASS(prop_weighted_cube, CPropDevCube);

CPropDevCube::CPropDevCube() {
	m_tType = CUBE_TYPE::CUBE_WEIGHTED;
}

void CPropDevCube::Precache() {
	PrecacheModel(CUBE_WEIGHTED_MDL);
	PrecacheParticleSystem("debug_sc_position");
	BaseClass::Precache();
}

void CPropDevCube::Spawn() {
	Precache();

	CubeChooseModel();
	SetSolid(SOLID_VPHYSICS);
	VPhysicsInitNormal(SOLID_VPHYSICS, 0, false);
	
	SetUse(&CPropDevCube::Use);
	m_nSkin = (int)CUBE_WEIGHTED_DEFAULT;
	
	BaseClass::Spawn();

	//DispatchParticleEffect("debug_sc_position", GetAbsOrigin() + Vector(0, 0, 10), GetAbsAngles());
}

int CPropDevCube::ObjectCaps() {
	int caps = BaseClass::ObjectCaps();
	return caps |= FCAP_IMPULSE_USE;
}

void CPropDevCube::Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value) {
	CBasePlayer* pPlayer = ToBasePlayer(pActivator);

	if (!pPlayer)
		return;

	// TODO: enable and 'on use' output to be fired here
	pPlayer->PickupObject(this, false);
}

void CPropDevCube::CubeChooseModel() {
	switch (m_tType) {
		case CUBE_WEIGHTED:
			SetModel(CUBE_WEIGHTED_MDL);
			break;
		case CUBE_COMPANION:
			SetModel(CUBE_WEIGHTED_MDL);
			m_nSkin = (int)CUBE_WEIGHTED_COMPANION;
			break;
		case CUBE_LASER:
			SetModel(CUBE_LASER_MDL);
			break;
		case CUBE_EDGELESS:
			SetModel(CUBE_EDGELESS_MDL);
			break;
		default:
			break;
	}
}

void CPropDevCube::CubeActivate() {
	switch (m_tType) {
	case CUBE_WEIGHTED:
		m_nSkin = (int)CUBE_WEIGHTED_DEFAULT_ACTIVE;
		break;
	case CUBE_COMPANION:
		m_nSkin = (int)CUBE_WEIGHTED_COMPANION_ACTIVE;
		break;
	case CUBE_LASER:
		break;
	case CUBE_EDGELESS:
		break;
	default:
		break;
	}
}

void CPropDevCube::CubeDeactivate() {
	switch (m_tType) {
	case CUBE_WEIGHTED:
		m_nSkin = (int)CUBE_WEIGHTED_DEFAULT;
		break;
	case CUBE_COMPANION:
		m_nSkin = (int)CUBE_WEIGHTED_COMPANION;
		break;
	case CUBE_LASER:
		break;
	case CUBE_EDGELESS:
		break;
	default:
		break;
	}
}

#include "prop_floorbutton.h"

void CPropDevCube::StartTouch(CBaseEntity* pOther) {
	// HACK: I'm forching the touch code to go, bc apparently something with vphysics flags is broken
	//if(pOther->ClassMatches("trigger_floor_button"))
		//pOther->StartTouch(this);

	//CTriggerFloorButton* pTrigger = assert_cast<CTriggerFloorButton*>(pOther);
	//if (pTrigger)
	//	pTrigger->StartTouch(pOther);
}

void CPropDevCube::EndTouch(CBaseEntity* pOther) {
	// HACK: I'm forching the touch code to go, bc apparently something with vphysics flags is broken
	//if (pOther->ClassMatches("trigger_floor_button"))
		//pOther->EndTouch(this);

	//CTriggerFloorButton* pTrigger = assert_cast<CTriggerFloorButton*>(pOther);
	//if (pTrigger)
	//	pTrigger->EndTouch(pOther);
}