#pragma once
#include "baseanimating.h"

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

class CPropDevCube : public CBaseAnimating {
	DECLARE_CLASS(CPropDevCube, CBaseAnimating);
	DECLARE_SERVERCLASS()
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
