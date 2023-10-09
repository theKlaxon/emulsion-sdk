#pragma once
#include "props.h"
#include "triggers.h"

// had to make this so only the top of the button does the thing
class CTriggerFloorButton;

class CPropFloorButton : public CDynamicProp {
	DECLARE_CLASS(CPropFloorButton, CDynamicProp)

	friend class CTriggerFloorButton;

public:

	void Precache();
	void Spawn();
	void Think();

	// button specifics
	void ButtonActivate();
	void ButtonDeactivate();

	bool IsActivated() { return m_bActive; }

	virtual bool PassesTriggerFilters(CBaseEntity* pOther);

	virtual void StartTouch(CBaseEntity* pOther);
	virtual void EndTouch(CBaseEntity* pOther);

protected:
	friend class CTriggerFloorButton;
	int touching;

private:

	bool m_bActive;
	int m_seqUp, m_seqDown;
	CHandle<CTriggerFloorButton> m_hButtonTrigger;

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
