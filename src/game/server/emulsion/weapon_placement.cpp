// ======== Emulsion - weapon_placement.cpp ========
// The Klaxon Method - Example
// TODO: add polish
// =================================================
#include "cbase.h"
#include "basecombatweapon.h"
#include "prop_placeable.h"
#include "scroll_controller.h"

#define EX_PROP_MDL "models/props/metal_box.mdl"

// TODO: pack the data needed for prop placement into 
// a struct if needed
struct PlacementData_t {

};

void Placement_ScaleUp();
void Placement_ScaleDn();

LINK_ENTITY_TO_CLASS(prop_placeable, CPropPlaceable)

void CPropPlaceable::Spawn() {

	Precache();

	SetModel(EX_PROP_MDL);
	//SetAbsOrigin(vec3_origin);
	//SetSolid(SOLID_VPHYSICS);
	SetSolid(SOLID_BBOX);
	//VPhysicsInitNormal(SOLID_VPHYSICS, 0, false);
	SetMoveType(MOVETYPE_VPHYSICS);

	SetCollisionGroup(COLLISION_GROUP_PLAYER_HELD);

	//m_flNormalGravity = GetGravity();
	SetGhost(true); // this is only starting true since this is purely example code.

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
	
	Vector maxs = ScriptGetBoundingMaxs() * GetModelScale();
	float brad = maxs[maxs.LargestComponent()];

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
		//VPhysicsGetObject()->EnableGravity(false);
		SetSolid(SOLID_NONE);
		return;
	}

	SetSolid(SOLID_BBOX);
	//AddSolidFlags(FSOLID_CUSTOMBOXTEST);
	
	//m_Collision.SetCollisionBounds(ScriptGetBoundingMins() * GetModelScale(), ScriptGetBoundingMaxs() * GetModelScale());

	//VPhysicsGetObject()->EnableGravity(true);
	//VPhysicsGetObject()->SetPosition(GetAbsOrigin(), GetAbsAngles(), true);
	//VPhysicsGetObject()->Wake();
}

#include "collisionutils.h"

//bool CPropPlaceable::TestCollision(const Ray_t& ray, unsigned int fContentsMask, trace_t& tr)
//{
//	Vector scriptmins = ScriptGetBoundingMins();
//	Vector scriptmaxs = ScriptGetBoundingMaxs();
//
//	// FIXME: This isn't a valid test, Jay needs to make it real
//	Vector vecMin = (scriptmins);// *GetModelScale());// -(ray.m_Extents / GetModelScale());
//	Vector vecMax = (scriptmaxs);// *GetModelScale());// +(ray.m_Extents / GetModelScale());
//
//	trace_t boxtrace;
//
//	tr.fraction = 1.0;
//
//	if (IntersectRayWithBox(GetAbsOrigin() - ray.m_Start, -ray.m_Delta, vecMin, vecMax, 0.0, &boxtrace))
//	{
//		if (boxtrace.fraction < tr.fraction)
//		{
//			tr = boxtrace;
//			tr.startpos = ray.m_Start;
//			tr.endpos = ray.m_Start + tr.fraction * ray.m_Delta;
//		}
//	}
//
//	if (tr.fraction < 1.0)
//	{
//		tr.contents = CONTENTS_SOLID;
//		tr.m_pEnt = this;
//		tr.hitbox = 0;
//		tr.hitgroup = HITGROUP_GENERIC;
//		tr.physicsbone = 0;
//	}
//
//	return true;
//}


class CWeaponPlacement : public CBaseCombatWeapon {
	DECLARE_CLASS(CWeaponPlacement, CBaseCombatWeapon)
public:

	virtual void Spawn();
	void Precache();

	void PrimaryAttack() { PlaceObject(); }
	void SecondaryAttack() { m_pCurObject->SetModelScale(2.0f); }
	
	virtual void Equip(CBaseCombatCharacter* pOwner);
	virtual void Drop(const Vector& vecVelocity);

	void ScaleObject(float flPace);
	const float CurObjectScale() {
		if (!m_pCurObject)
			return 0.0f;

		return m_pCurObject->GetModelScale();
	}

private:

	void Not_PlacementThink();
	void PlacementThink();
	void PlaceObject();

	CPropPlaceable* m_pCurObject;

};

LINK_ENTITY_TO_CLASS(weapon_placement, CWeaponPlacement)

float tempalpha;

ConVar sv_placement_scale_place("sv_placement_scale_pace", "0.1f");
CWeaponPlacement* g_pPlayerPlacement = nullptr;

void CWeaponPlacement::Spawn() {
	BaseClass::Spawn();
	Precache();

	// for the example code only
	m_pCurObject = (CPropPlaceable*)CreateEntityByName("prop_placeable");
	m_pCurObject->Spawn();
	tempalpha = m_pCurObject->GetRenderAlpha();
}

void CWeaponPlacement::Precache() {
	BaseClass::Precache();
}

void CWeaponPlacement::Not_PlacementThink() {

	SetNextThink(gpGlobals->curtime + 0.0001f);
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
			m_pCurObject->SetRenderAlpha(tempalpha * 0.5f);
			
		}
		else {
			m_pCurObject->SetRenderAlpha(tempalpha * 0.2f);
		}

	SetNextThink(gpGlobals->curtime + 0.0001f);
}

void CWeaponPlacement::PlaceObject() {

	if (!m_pCurObject || !m_pCurObject->IsPlaceable())
		return;

	m_pCurObject->SetRenderAlpha(255);
	m_pCurObject->SetGhost(0);

	SetThink(&CWeaponPlacement::Not_PlacementThink);

}

void CWeaponPlacement::Equip(CBaseCombatCharacter* pOwner) {
	BaseClass::Equip(pOwner);

	g_sScroller.SetUp(Placement_ScaleDn);
	g_sScroller.SetDn(Placement_ScaleUp);

	g_pPlayerPlacement = this;

	// setup the think
	SetThink(&CWeaponPlacement::PlacementThink);
	SetNextThink(gpGlobals->curtime + 0.0001f);
}

void CWeaponPlacement::Drop(const Vector& vecVelocity) {
	BaseClass::Drop(vecVelocity);
	g_sScroller.ClearBinds();
	g_pPlayerPlacement = nullptr;
}

void CWeaponPlacement::ScaleObject(float flPace) {

	m_pCurObject->SetModelScale(m_pCurObject->GetModelScale() + flPace);
	//m_pCurObject->VPhysicsGetObject()->SetSphereRadius(m_pCurObject->VPhysicsGetObject()->GetSphereRadius() * m_pCurObject->GetModelScale());

}

float g_flCurScaleDelay = 0.0f;

void Placement_DoScale(bool dn) {

	float mod = 1.0f;

	if (dn)
		mod = -1.0f;

	float scaleamt = mod * sv_placement_scale_place.GetFloat() * 10.0f;

	if (g_pPlayerPlacement->CurObjectScale() < 2.0f)
		scaleamt = mod * sv_placement_scale_place.GetFloat() * 2.0f;

	g_pPlayerPlacement->ScaleObject(scaleamt);
	g_flCurScaleDelay = gpGlobals->curtime + sv_placement_scale_place.GetFloat();

}

void Placement_ScaleDn() {

	if (g_flCurScaleDelay > gpGlobals->curtime || g_pPlayerPlacement->CurObjectScale() < 0.25f)
		return;

	Placement_DoScale(1);
}

void Placement_ScaleUp() {

	if (g_flCurScaleDelay > gpGlobals->curtime || g_pPlayerPlacement->CurObjectScale() > 7.0f)
		return;

	Placement_DoScale(0);
}