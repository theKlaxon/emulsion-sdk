#include "cbase.h"
#include "baseentity.h"
#include "game/shared/portal2/paint_enum.h"
#include "world.h"
#include "engine/IEngineTrace.h"
#include "paint_sphere.h"
#include "paintblob_manager.h"

BEGIN_DATADESC(CPaintSphere)
DEFINE_KEYFIELD(m_nPaintType,	FIELD_INTEGER,	"paint_type"),
DEFINE_KEYFIELD(m_flRad,		FIELD_FLOAT,	"radius"),
DEFINE_KEYFIELD(m_flHardness,	FIELD_FLOAT,	"alpha_percent"),
DEFINE_INPUTFUNC(FIELD_VOID,	"Paint",		InputPaint),
END_DATADESC()

LINK_ENTITY_TO_CLASS(paint_sphere, CPaintSphere)

class CBrushEnum : public IEntityEnumerator {
public:

	CBrushEnum() { m_count = 0; }

	virtual bool EnumEntity(IHandleEntity* pHandle) override {

		CBaseEntity* pEntity;

		if (pHandle) {
			pEntity = gEntList.GetBaseEntity(pHandle->GetRefEHandle());

			if (pEntity) {
				m_Brushes.AddToTail(pEntity);
				m_count++;
			}

		}
		
		return true;
	}

	int GetCount() { return m_count; }

	CUtlVectorFixedGrowable<CBaseEntity*, 32> m_Brushes;
	int m_count;
};

const Vector dirs[6] = {
	Vector(1, 0, 0),
	Vector(-1, 0, 0),
	Vector(0, 1, 0),
	Vector(0, -1, 0),
	Vector(0, 0, 1),
	Vector(0, 0, -1)
};

class CDetailEnum : public IPartitionEnumerator {
public:

	CDetailEnum(CBaseEntity** pList, int nMax) { m_pList = pList; m_nMax = nMax; m_nCount = 0; }

	bool Add(CBaseEntity* pEnt) {
		if (m_nCount >= m_nMax)
			return false;

		m_pList[m_nCount] = pEnt;
		m_nCount++;

		return true;
	}

	virtual IterationRetval_t EnumElement(IHandleEntity* pHandle) {

		CBaseEntity* pEnt = gEntList.GetBaseEntity(pHandle->GetRefEHandle());

		if (!pEnt)
			return ITERATION_CONTINUE;

		if (!Add(pEnt))
			return ITERATION_STOP;

		return ITERATION_CONTINUE;
	}

	int GetCount() { return m_nCount; }

private:

	CBaseEntity** m_pList;
	int m_nMax;
	int m_nCount;

};

#include "util.h"

void CPaintSphere::Spawn() {
	
	PaintBlobManager()->QueuePaintSphere(this);

	BaseClass::Spawn();
}

void CPaintSphere::Paint() {
	
	trace_t tr;
	for (int i = 0; i < 6; i++) {
		UTIL_TraceLine(GetAbsOrigin(), GetAbsOrigin() + (dirs[i] * m_flRad), MASK_ALL, this, COLLISION_GROUP_PLAYER_MOVEMENT, &tr);

		if (tr.DidHitWorld())
			if (tr.m_pEnt)
				engine->SpherePaintSurface(tr.m_pEnt->GetModel(), tr.endpos, (PaintPowerType)m_nPaintType, m_flRad, m_flHardness);

	}
}