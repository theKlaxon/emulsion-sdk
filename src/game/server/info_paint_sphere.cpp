#include "cbase.h"
#include "gameinterface.h"
#include "game/shared/portal2/paint_enum.h"

#define MAX_PAINTSPHERE_ENTS 64

class CInfoPaintSphere : public CBaseEntity {
	DECLARE_CLASS(CInfoPaintSphere, CBaseEntity)
	DECLARE_DATADESC()
public:

	virtual void Spawn();

private:

	bool IsValidPower(PaintPowerType _type);

	int m_nRadius;
	int m_nPaintPower;
};

BEGIN_DATADESC(CInfoPaintSphere)
	DEFINE_FIELD(m_nRadius, FIELD_INTEGER),
	DEFINE_FIELD(m_nPaintPower, FIELD_INTEGER)
END_DATADESC()

LINK_ENTITY_TO_CLASS(paint_sphere, CInfoPaintSphere) 

void CInfoPaintSphere::Spawn() {

	// check if we have a valid power, reset to bounce if not.
	PaintPowerType type = (PaintPowerType)m_nPaintPower;
	if (!IsValidPower(type))
		type = BOUNCE_POWER;

	// run a sphere trace for all paintable ents within the given radius
	CBaseEntity* pEntList[MAX_PAINTSPHERE_ENTS];
	UTIL_EntitiesInSphere(pEntList, MAX_PAINTSPHERE_ENTS, GetAbsOrigin(), m_nRadius, MASK_SOLID);

	for (int i = 0; i < MAX_PAINTSPHERE_ENTS; i++) {

		if (pEntList[i] == NULL)
			continue;

	/*	if(pEntList[i]->GetModel())
			engine->SpherePaintSurface(pEntList[i]->GetModel(), )*/

	}
}

bool CInfoPaintSphere::IsValidPower(PaintPowerType _type) {
	
	if (_type == BOUNCE_POWER || _type == SPEED_POWER || _type == PORTAL_POWER)
		return true;

	return false;
}