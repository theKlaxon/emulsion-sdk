// ======== Emulsion - weapon_placement.cpp ========
// The Klaxon Method - Example
// TODO: add polish
// =================================================
#include "cbase.h"
#include "basecombatweapon.h"

#define EX_PROP_MDL "models/props/metal_box.mdl"

// TODO: pack the data needed for prop placement into 
// a struct if needed
struct PlacementData_t {

};

// this is just an example, so if used in the furture then this code should only 
// be use as a base class. ex. ents like prop_weighted_cube would be placeable props
class CPropPlaceable : public CBaseAnimating {
	DECLARE_CLASS(CPropPlaceable, CBaseAnimating)
public:

	void Spawn();
	void Precache();

	bool SuggestOrigin(Vector origin, cplane_t plane);
	bool IsPlaceable() { return m_bIsPlaceable && m_bIsGhost; }
	void SetGhost(bool isGhost);

private:

	bool m_bIsGhost; // controls wheteher or not this ent is in placement mode
	bool m_bIsPlaceable; 
	float m_flNormalGravity;

};

LINK_ENTITY_TO_CLASS(prop_placeable, CPropPlaceable)

void CPropPlaceable::Spawn() {

	Precache();

	SetModel(EX_PROP_MDL);
	SetAbsOrigin(vec3_origin);
	SetSolid(SOLID_VPHYSICS);
	VPhysicsInitNormal(SOLID_VPHYSICS, 0, false);

	SetCollisionGroup(COLLISION_GROUP_PLAYER_HELD);

	SetGhost(true); // this is only starting true since this is purely example code.
	m_flNormalGravity = GetGravity();

	m_bIsPlaceable = false;
}

void CPropPlaceable::Precache() {
	PrecacheModel(EX_PROP_MDL);
}

const Vector dirs[6] = {
	Vector(1, 0, 0),
	Vector(-1, 0, 0),
	Vector(0, 1, 0),
	Vector(0, -1, 0),
	Vector(0, 0, 1),
	Vector(0, 0, -1)
};

bool CPropPlaceable::SuggestOrigin(Vector origin, cplane_t plane) {
	
	Vector maxs = ScriptGetBoundingMaxs();
	float brad = maxs[maxs.LargestComponent()];// *GetModelScale();

	Vector audited = origin;
	audited += plane.normal * brad;

	// check if we're colliding with any other surfaces
	trace_t tr;
	for (int i = 0; i < 6; i++) {
		UTIL_TraceLine(audited, audited + (dirs[i] * brad), MASK_SOLID, this, COLLISION_GROUP_PLAYER_MOVEMENT, &tr);

		if (tr.DidHit())
			audited -= dirs[i] * brad;
	}

	QAngle newForward;
	VectorAngles(plane.normal, newForward);

	SetAbsOrigin(audited);
	SetAbsAngles(newForward);
	DrawBBoxOverlay();

	m_bIsPlaceable = true;
	return true;
}

void CPropPlaceable::SetGhost(bool bIsGhost) {

	m_bIsGhost = bIsGhost;

	if (bIsGhost) {
		SetGravity(0);
		SetSolid(SOLID_NONE);
		return;
	} 

	SetGravity(m_flNormalGravity);
	SetSolid(SOLID_VPHYSICS);
}

class CWeaponPlacement : public CBaseCombatWeapon {
	DECLARE_CLASS(CWeaponPlacement, CBaseCombatWeapon)
public:

	void Spawn();
	void Precache();

	void PrimaryAttack() { PlaceObject(); }
	void SecondaryAttack() { m_pCurObject->SetModelScale(2.0f); }
	
private:

	void PlacementThink();
	void PlaceObject();

	CPropPlaceable* m_pCurObject;

};

LINK_ENTITY_TO_CLASS(weapon_placement, CWeaponPlacement)

void CWeaponPlacement::Spawn() {
	
	Precache();

	// setup the think
	SetThink(&CWeaponPlacement::PlacementThink);
	SetNextThink(gpGlobals->curtime + 0.0001f);

	// for the example code only
	m_pCurObject = (CPropPlaceable*)CreateEntityByName("prop_placeable");
	m_pCurObject->Spawn();
	m_pCurObject->SetGhost(1); // ensure we have a ghost
}

void CWeaponPlacement::Precache() {

}

void CWeaponPlacement::PlacementThink() {

	CBasePlayer* pPlayer = UTIL_GetLocalPlayer();

	// trace a ray, suggest the end point as the origin of the prop we're holding
	Vector forward;
	AngleVectors(pPlayer->EyeAngles(), &forward);

	trace_t tr;
	UTIL_TraceLine(pPlayer->Weapon_ShootPosition(), forward * MAX_TRACE_LENGTH, MASK_SOLID_BRUSHONLY, pPlayer, COLLISION_GROUP_PLAYER_MOVEMENT, &tr);

	if (tr.DidHitWorld() && m_pCurObject)
		if (m_pCurObject->SuggestOrigin(tr.endpos, tr.plane)) {
			m_pCurObject->SetRenderAlpha(125);
			m_pCurObject->SetGhost(0);
		}
		else {
			m_pCurObject->SetRenderAlpha(0);
			m_pCurObject->SetGhost(1);

			m_pCurObject->SetAbsOrigin(vec3_origin);
		}

	SetNextThink(gpGlobals->curtime + 0.0001f);
}

void CWeaponPlacement::PlaceObject() {

	if (!m_pCurObject && m_pCurObject->IsPlaceable())
		return;

	m_pCurObject->SetRenderAlpha(255);
	m_pCurObject->SetGhost(0);

}