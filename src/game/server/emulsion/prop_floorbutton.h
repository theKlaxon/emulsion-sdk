#pragma once
#include "props.h"
#include "triggers.h"

// had to make this so only the top of the button does the thing
class CTriggerFloorButton;

class CPropFloorButton : public CDynamicProp {
	DECLARE_CLASS(CPropFloorButton, CDynamicProp)
	DECLARE_DATADESC()

	friend class CTriggerFloorButton;

public:

	void Precache();
	void Spawn();
	void Think();

	// button specifics
	void ButtonActivate(inputdata_t& inputdata);
	void ButtonDeactivate(inputdata_t& inputdata);

	bool IsActivated() { return m_bActive; }

	virtual bool PassesTriggerFilters(CBaseEntity* pOther);

	virtual void StartTouch(CBaseEntity* pOther);
	virtual void EndTouch(CBaseEntity* pOther);

protected:
	friend class CTriggerFloorButton;
	int touching;

private:

	COutputEvent m_outButtonPressed;
	COutputEvent m_outButtonUnPressed;
	COutputEvent m_outUnused;

	CHandle<CTriggerFloorButton> m_hButtonTrigger;

	bool m_bActive;
	int m_seqUp, m_seqDown;
};

class CTriggerFloorButton : public CBaseTrigger {
	DECLARE_CLASS(CTriggerFloorButton, CBaseTrigger)
public:

	virtual void Spawn();
	virtual bool PassesTriggerFilters(CBaseEntity* pOther);
	virtual void StartTouch(CBaseEntity* pOther);
	virtual void EndTouch(CBaseEntity* pOther);

	CPropFloorButton* m_pOwnerButton;
};
