#include "cbase.h"
#include "triggers.h"

class CTrigger_SolidField : public CTriggerMultiple {
	DECLARE_CLASS(CTrigger_SolidField, CTriggerMultiple)
public:

	void Spawn();

};

LINK_ENTITY_TO_CLASS(trigger_portal_cleanser, CTrigger_SolidField)

void CTrigger_SolidField::Spawn() {
	BaseClass::Spawn();

	RemoveEffects(EF_NODRAW);
}