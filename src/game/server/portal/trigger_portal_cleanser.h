#pragma once
#include "triggers.h"
#include "portal_base2d.h"

//-----------------------------------------------------------------------------
// Purpose: Removes anything that touches it. If the trigger has a targetname,
//			firing it will toggle state.
//-----------------------------------------------------------------------------
class CTriggerPortalCleanser : public CBaseTrigger
{
public:
	DECLARE_CLASS(CTriggerPortalCleanser, CBaseTrigger);

	void Spawn(void);
	void Touch(CBaseEntity* pOther);

	bool m_bVisible;

	DECLARE_DATADESC();

	// Outputs
	COutputEvent m_OnDissolve;
	COutputEvent m_OnFizzle;
	COutputEvent m_OnDissolveBox;
};