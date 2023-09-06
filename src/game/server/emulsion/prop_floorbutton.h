#pragma once
#include "props.h"

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
	friend class CTriggerFLoorButton;
	int touching;

private:

	bool m_bActive;
	int m_seqUp, m_seqDown;
	CHandle<CTriggerFloorButton> m_hButtonTrigger;

};