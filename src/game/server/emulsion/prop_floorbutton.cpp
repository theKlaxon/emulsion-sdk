#include "cbase.h"
#include "props.h"
#include "baseanimating.h"
#include "triggers.h"
#include "prop_floorbutton.h"
#include "prop_button.h"
#include "prop_devcube.h"

#define BUTTON_FLOOR_MDL "models/props/portal_button.mdl"
#define BUTTON_FLOOR_MINS Vector(-20, -20, 0)
#define BUTTON_FLOOR_MAXS Vector(20, 20, 15)

enum BUTTON_FLOOR_SKIN {
	BUTTON_DEFAULT = 0,
	BUTTON_ACTIVE
};

LINK_ENTITY_TO_CLASS(trigger_floor_button, CTriggerFloorButton)

void CTriggerFloorButton::Spawn() {

	BaseClass::Spawn();

	SetSolid(SOLID_BBOX);
	AddSolidFlags(FSOLID_TRIGGER);
	AddSpawnFlags(SF_TRIGGER_ALLOW_ALL);
}

bool CTriggerFloorButton::PassesTriggerFilters(CBaseEntity* pOther)
{
	if (pOther->IsPlayer() || pOther->ClassMatches("prop_weighted_cube"))
		return true;
	
	return false;
}

void CTriggerFloorButton::StartTouch(CBaseEntity* pOther) {
	inputdata_t actor;
	actor.pActivator = pOther;
	if (PassesTriggerFilters(pOther)) {

		if (m_pOwnerButton->touching < 1)
			if (m_pOwnerButton)
				m_pOwnerButton->ButtonActivate(actor);

		if (pOther->ClassMatches("prop_weighted_cube"))
			((CPropDevCube*)pOther)->CubeActivate();

		m_pOwnerButton->touching++;
	}
}

void CTriggerFloorButton::EndTouch(CBaseEntity* pOther) {
	inputdata_t actor;
	actor.pActivator = pOther;
	if (PassesTriggerFilters(pOther)) {

		m_pOwnerButton->touching--;

		if (m_pOwnerButton->touching < 1)
			if (m_pOwnerButton)
				m_pOwnerButton->ButtonDeactivate(actor);

		if (pOther->ClassMatches("prop_weighted_cube"))
			((CPropDevCube*)pOther)->CubeDeactivate();
	}
}

LINK_ENTITY_TO_CLASS(prop_floor_button, CPropFloorButton)

BEGIN_DATADESC(CPropFloorButton)

DEFINE_INPUTFUNC(FIELD_VOID, "PressIn", ButtonActivate),
DEFINE_INPUTFUNC(FIELD_VOID, "PressOut", ButtonDeactivate),

DEFINE_OUTPUT(m_outButtonPressed, "OnPressed"),
DEFINE_OUTPUT(m_outUnused, "OnPressedOrange"),
DEFINE_OUTPUT(m_outUnused, "OnPressedBlue"),
DEFINE_OUTPUT(m_outButtonUnPressed, "OnUnPressed"),

END_DATADESC()

void CPropFloorButton::Precache() {
	PrecacheModel(BUTTON_FLOOR_MDL);
}

void CPropFloorButton::Spawn() {
	BaseClass::Spawn();
	Precache();
	
	AddEffects(EF_NOSHADOW);

	SetModel(BUTTON_FLOOR_MDL);
	SetSolid(SOLID_VPHYSICS);

	SetThink(&CPropFloorButton::Think);

	m_seqUp = LookupSequence("up");
	m_seqDown = LookupSequence("down");

	ResetSequence(m_seqUp);
	m_bActive = false;

	CreateVPhysics();
	
	// create the trigger for this button
	CTriggerFloorButton* pTrigger = (CTriggerFloorButton*)CreateEntityByName("trigger_floor_button");

	pTrigger->SetAbsOrigin(GetAbsOrigin());
	pTrigger->SetSize(BUTTON_FLOOR_MINS, BUTTON_FLOOR_MAXS);
	pTrigger->SetParent(this);
	
	pTrigger->m_pOwnerButton = this;
	//pTrigger->Enable();

	m_hButtonTrigger = pTrigger;
	DispatchSpawn(m_hButtonTrigger.Get());

	SetNextThink(gpGlobals->curtime + 0.1f);

	SetFadeDistance(-1.0f, 0.0f);
	SetGlobalFadeScale(0.0f);
}

void CPropFloorButton::Think() {
	StudioFrameAdvance();
	m_BoneFollowerManager.UpdateBoneFollowers(this);

	// set next think to update anims again
	SetNextThink(gpGlobals->curtime + BUTTON_FRAME_ADV_TIME);
}

void CPropFloorButton::ButtonActivate(inputdata_t& inputdata) {

	m_nSkin = BUTTON_ACTIVE;
	SetSequence(m_seqDown);
	m_bActive = true;

	m_outButtonPressed.FireOutput(inputdata.pActivator, this);
}

void CPropFloorButton::ButtonDeactivate(inputdata_t& inputdata) {

	m_nSkin = BUTTON_DEFAULT;
	SetSequence(m_seqUp);
	m_bActive = false;

	m_outButtonUnPressed.FireOutput(inputdata.pActivator, this);
}

bool CPropFloorButton::PassesTriggerFilters(CBaseEntity* pOther)
{
	if (pOther->IsPlayer() || pOther->GetClassname() == "prop_weighted_cube")
		return true;

	// failed filter check
	return true;
}


void CPropFloorButton::StartTouch(CBaseEntity* pOther) {

}

void CPropFloorButton::EndTouch(CBaseEntity* pOther) {

}