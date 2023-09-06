#pragma once
#include "props.h"

#define BUTTON_FRAME_ADV_TIME 0.1f

class CPropPedestalButton : public CBaseAnimating {
	DECLARE_CLASS(CPropPedestalButton, CBaseAnimating)
public:

	CPropPedestalButton();

	void Precache();
	void Spawn();
	void Think();

	// player use
	void Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value);
	int ObjectCaps();

	// button specifics
	void ButtonActivate();
	void ButtonDeactivate();

	bool IsActivated() { return m_bActive; }

private:
	
	bool m_bIsTimed;
	bool m_bActive;
	int m_seqUp, m_seqDown, m_seqIdle, m_seqIdleDown;
	float m_flDelayTime;
	float m_flTimerDuration;

};