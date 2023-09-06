#include "cbase.h"
#include "props.h"
#include "baseanimating.h"
#include "triggers.h"
#include "prop_floorbutton.h"
#include "prop_button.h"

#define BUTTON_FLOOR_MDL "models/props/portal_button.mdl"
#define BUTTON_FLOOR_MINS Vector(-20, -20, 0)
#define BUTTON_FLOOR_MAXS Vector(20, 20, 15)

enum BUTTON_FLOOR_SKIN {
	BUTTON_DEFAULT = 0,
	BUTTON_ACTIVE
};

// had to make this so only the top of the button does the thing
class CTriggerFloorButton : public CBaseTrigger {
	DECLARE_CLASS(CTriggerFloorButton, CBaseTrigger)
public:
	
	virtual void Spawn();
	virtual bool PassesTriggerFilters(CBaseEntity* pOther);
	virtual void StartTouch(CBaseEntity* pOther);
	virtual void EndTouch(CBaseEntity* pOther);
	
	CPropFloorButton* m_pOwnerButton;
};

LINK_ENTITY_TO_CLASS(trigger_floor_button, CTriggerFloorButton)

void CTriggerFloorButton::Spawn() {
	//BaseClass::Spawn();

	SetSolid(SOLID_BBOX);
	AddSolidFlags(FSOLID_TRIGGER | FSOLID_NOT_SOLID);
	AddSpawnFlags(SF_TRIGGER_ALLOW_PHYSICS | SF_TRIGGER_ALLOW_CLIENTS );
}

bool CTriggerFloorButton::PassesTriggerFilters(CBaseEntity* pOther)
{
	//bool bPassedFilter = BaseClass::PassesTriggerFilters(pOther);

	//// did I fail the baseclass filter check?
	//if (!bPassedFilter)
	//	return false;
	
	if (pOther->IsPlayer() || pOther->GetClassname() == "prop_weighted_cube")
		return true;
	
	// failed filter check
	return false;
}

void CTriggerFloorButton::StartTouch(CBaseEntity* pOther) {
	if (pOther->IsPlayer() || pOther->GetClassname() == "prop_weighted_cube") {

		if (m_pOwnerButton->touching < 1)
			if (m_pOwnerButton)
				m_pOwnerButton->ButtonActivate();

		m_pOwnerButton->touching++;
	}
		
}

void CTriggerFloorButton::EndTouch(CBaseEntity* pOther) {
	if (pOther->IsPlayer() || pOther->ClassMatches("prop_weighted_cube")) {

		m_pOwnerButton->touching--;

		if (m_pOwnerButton->touching < 1)
			if (m_pOwnerButton)
				m_pOwnerButton->ButtonDeactivate();

		
	}
}

LINK_ENTITY_TO_CLASS(prop_floor_button, CPropFloorButton)

void CPropFloorButton::Precache() {
	PrecacheModel(BUTTON_FLOOR_MDL);
}

void CPropFloorButton::Spawn() {
	Precache();
	BaseClass::Spawn();

	SetModel(BUTTON_FLOOR_MDL);
	SetSolid(SOLID_VPHYSICS);
	SetThink(&CPropFloorButton::Think);
	
	
	m_seqUp = LookupSequence("up");
	m_seqDown = LookupSequence("down");
	ResetSequence(m_seqUp);
	m_bActive = false;

	//VPhysicsInitNormal(,);
	AddSolidFlags(FSOLID_TRIGGER);
	CreateVPhysics();
	//PhysicsTouchTriggers();

	

	// create the trigger for this button
	//m_hButtonTrigger = (CTriggerFloorButton*)CreateEntityByName("trigger_floor_button");
	//DispatchSpawn(m_hButtonTrigger.Get());

	//m_hButtonTrigger.Get()->SetParent(this);
	//m_hButtonTrigger.Get()->m_pOwnerButton = this;
	//m_hButtonTrigger.Get()->SetAbsOrigin(GetAbsOrigin());
	//m_hButtonTrigger.Get()->SetAbsAngles(GetAbsAngles());
	//m_hButtonTrigger.Get()->SetSize(BUTTON_FLOOR_MINS, BUTTON_FLOOR_MAXS);
	//m_hButtonTrigger.Get()->Enable();

	SetNextThink(gpGlobals->curtime + 0.1f);
}

void CPropFloorButton::Think() {
	StudioFrameAdvance();
	m_BoneFollowerManager.UpdateBoneFollowers(this);

	// set next think to update anims again
	SetNextThink(gpGlobals->curtime + BUTTON_FRAME_ADV_TIME);
}

void CPropFloorButton::ButtonActivate() {

	m_nSkin = BUTTON_ACTIVE;
	SetSequence(m_seqDown);
	m_bActive = true;
}

void CPropFloorButton::ButtonDeactivate() {

	m_nSkin = BUTTON_DEFAULT;
	SetSequence(m_seqUp);
	m_bActive = false;
}

bool CPropFloorButton::PassesTriggerFilters(CBaseEntity* pOther)
{
	//bool bPassedFilter = BaseClass::PassesTriggerFilters(pOther);

	//// did I fail the baseclass filter check?
	//if (!bPassedFilter)
	//	return false;

	if (pOther->IsPlayer() || pOther->GetClassname() == "prop_weighted_cube")
		return true;

	// failed filter check
	return true;
}


void CPropFloorButton::StartTouch(CBaseEntity* pOther) {

	if (!pOther->IsPlayer() && !pOther->ClassMatches("prop_weighted_cube"))
		return;

	if (touching < 1)
		ButtonActivate();

	touching++;
}

void CPropFloorButton::EndTouch(CBaseEntity* pOther) {

	if (!pOther->IsPlayer() && !pOther->ClassMatches("prop_weighted_cube"))
		return;

	touching--;

	if (touching < 1)
		ButtonDeactivate();
}