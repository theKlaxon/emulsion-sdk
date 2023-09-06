#include "cbase.h"
#include "prop_button.h"

#define BUTTON_PEDESTAL_MDL "models/props/switch001.mdl"
#define BUTTON_PEDESTAL_DWN_SND "Portal.button_down"
#define BUTTON_PEDESTAL_UP_SND "Portal.button_up"
#define BUTTON_PEDESTAL_DEACT_DELAY 0.5f

enum BUTTON_PEDESTAL_SKIN {
	BUTTON_PEDESTAL_DEFAULT = 0,
	BUTTON_PEDESTAL_DELAPIDATED,
};

LINK_ENTITY_TO_CLASS(prop_button, CPropPedestalButton)

CPropPedestalButton::CPropPedestalButton() {
	m_bActive = false;
	m_bIsTimed = false;
	m_flDelayTime = -69;
	m_flTimerDuration = 69.0f;
}

void CPropPedestalButton::Precache() {
	PrecacheModel(BUTTON_PEDESTAL_MDL);
	PrecacheScriptSound(BUTTON_PEDESTAL_DWN_SND);
	PrecacheScriptSound(BUTTON_PEDESTAL_UP_SND);
}

void CPropPedestalButton::Spawn() {
	Precache();
	BaseClass::Spawn();

	SetModel(BUTTON_PEDESTAL_MDL);
	SetMoveType(MOVETYPE_NONE);
	SetSolid(SOLID_VPHYSICS);
	SetThink(&CPropPedestalButton::Think);

	m_seqUp = LookupSequence("up");
	m_seqDown = LookupSequence("down");
	m_seqIdle = LookupSequence("idle");
	m_seqIdleDown = LookupSequence("idle_down");
	ResetSequence(m_seqIdle);

	CreateVPhysics();
	SetNextThink(gpGlobals->curtime + 0.1f);
}

void CPropPedestalButton::Think() {
	StudioFrameAdvance();
	int m_nCurSeq = GetSequence(); // temp save since we're in for a few more checks now
	
	if (IsSequenceFinished())
		if (m_nCurSeq == m_seqDown)
			if (gpGlobals->curtime > m_flDelayTime) {
				SetSequence(m_seqUp);
				ButtonDeactivate(); // deactivate once time is up, not when anims are done
				m_flDelayTime = -69;
			}
		else if (m_nCurSeq == m_seqUp)
			ResetSequence(m_seqIdle);

	SetNextThink(gpGlobals->curtime + BUTTON_FRAME_ADV_TIME);
}

void CPropPedestalButton::Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value) {
	if (pActivator->IsPlayer())
		ButtonActivate();
}

int CPropPedestalButton::ObjectCaps() {
	int caps = BaseClass::ObjectCaps();
	return caps |= FCAP_IMPULSE_USE;
}

void CPropPedestalButton::ButtonActivate() {
	if (m_flDelayTime > gpGlobals->curtime)
		return;

	EmitSound(BUTTON_PEDESTAL_DWN_SND);
	SetSequence(m_seqDown);
	m_bActive = true;

	if (!m_bIsTimed)
		m_flDelayTime = gpGlobals->curtime + BUTTON_PEDESTAL_DEACT_DELAY;
	else
		m_flDelayTime = gpGlobals->curtime + m_flTimerDuration;
}

void CPropPedestalButton::ButtonDeactivate() {
	m_bActive = false;

	if (m_bIsTimed) {
		EmitSound(BUTTON_PEDESTAL_UP_SND);
		// TODO: fire output timer end
	}
}