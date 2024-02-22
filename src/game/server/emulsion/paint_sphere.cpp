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

	// get any brushes within our rad
	//float rad = 256.0f;
	//Vector vecRad = Vector(1.0f, 1.0f, 1.0f) * rad;
	//Vector worldSpacePaintPoint = GetAbsOrigin() + 0.1f * vec3_origin;

	
	//CBaseEntity* pEnts[16];
	//CDetailEnum pEnum(pEnts, 16);
	////int cnt = UTIL_EntitiesInSphere(GetAbsOrigin(), rad, &pEnum);//UTIL_EntitiesInSphere(pEnts, 16, GetAbsOrigin(), rad, 0);

	//partition->EnumerateElementsInSphere(PARTITION_ENGINE_NON_STATIC_EDICTS, GetAbsOrigin(), rad, false, &pEnum);
	//int cnt = pEnum.GetCount();

	//for (int i = 0; i < cnt; i++) {
	//	int k = 0;
	//}

	//CBrushEnum pList;
	//enginetrace->EnumerateEntities(worldSpacePaintPoint - vecRad, worldSpacePaintPoint + vecRad, &pList);

	//int k = 0;

	//CBrushQuery query;
	//enginetrace->GetBrushesInAABB(worldSpacePaintPoint - vecRad, worldSpacePaintPoint + vecRad, query);

	//for (int i = 0; i < query.m_iCount; i++) {
	//	
	//	CBaseEntity* pEnt = gEntList.GetBaseEntity(query.m_pBrushes[i]);

	//	if (!pEnt)
	//		continue;

	//	if (!pEnt->GetModel())
	//		continue;

	//	Vector entSpacePos;
	//	pEnt->WorldToEntitySpace(worldSpacePaintPoint, &entSpacePos);

	//	engine->SpherePaintSurface(pEnt->GetModel(), entSpacePos, BOUNCE_POWER, rad, 1.0f);

	//}



	//for (int i = 0; i < count; i++) {
	//	
	//	Vector entSpacePos;
	//	pList.m_Brushes[i]->WorldToEntitySpace(worldSpacePaintPoint, &entSpacePos);

	//	engine->SpherePaintSurface(pList.m_Brushes[i]->GetModel(), entSpacePos, BOUNCE_POWER, rad, 1.0f);

	//}

	//for (int i = 0; i < count; i++) {
	//	if (!pEnts[i]->IsBSPModel())
	//		continue;

	//	Vector entSpacePaintPoint;
	//	pEnts[i]->WorldToEntitySpace(worldSpacePaintPoint, &entSpacePaintPoint);

	//	engine->SpherePaintSurface(pEnts[i]->GetModel(), entSpacePaintPoint, BOUNCE_POWER, 128.0f, 1.0f);
	//}

	//engine->SpherePaintSurface(GetWorldEntity()->GetModel(), entSpacePaintPoint, BOUNCE_POWER, 128.0f, 1.0f);


	//CBrushEnum pList;
	//enginetrace->EnumerateEntities(worldSpacePaintPoint - vecRad, worldSpacePaintPoint + vecRad, &pList);

	//for (int i = 0; i < pList.m_Brushes.Count(); i++) {

	//	CBaseEntity* pEnt;
	//	Vector entSpacePaintPoint;
	//	pEnt = pList.m_Brushes[i];
	//	pEnt->WorldToEntitySpace(worldSpacePaintPoint, &entSpacePaintPoint);

	//	engine->SpherePaintSurface(pEnt->GetModel(), entSpacePaintPoint, BOUNCE_POWER, 128.0f, 1.0f);
	//}

	//engine->SpherePaintSurface(GetWorldEntity()->GetModel(), GetAbsOrigin(), BOUNCE_POWER, 256.0f, 1.0f);

}

void CPaintSphere::Paint() {
	
	//float rad = 256.0f;
	//Vector vecRad = Vector(1.0f, 1.0f, 1.0f) * rad;
	//Vector worldSpacePaintPoint = GetAbsOrigin() + 0.1f * vec3_origin;

	trace_t tr;
	for (int i = 0; i < 6; i++) {
		UTIL_TraceLine(GetAbsOrigin(), GetAbsOrigin() + (dirs[i] * m_flRad), MASK_ALL, this, COLLISION_GROUP_PLAYER_MOVEMENT, &tr);

		if (tr.DidHitWorld())
			if (tr.m_pEnt)
				engine->SpherePaintSurface(tr.m_pEnt->GetModel(), tr.endpos, (PaintPowerType)m_nPaintType, m_flRad, m_flHardness);

	}
}