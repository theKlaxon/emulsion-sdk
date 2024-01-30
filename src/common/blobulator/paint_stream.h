#pragma once
#include "baseanimating.h"
#include "ipaintstream.h"
#include "player_pickup.h"
#include "ai_basenpc.h"

class CPaintBlobStream : public CAI_BaseNPC, public IPaintStream {
	DECLARE_CLASS(CPaintBlobStream, CAI_BaseNPC)
	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();
	DECLARE_PREDICTABLE();
public:

	CPaintBlobStream();

	void Precache();
	void Spawn();

	virtual void VPhysicsCollision(int index, gamevcollisionevent_t* pEvent) override;
	virtual bool CreateVPhysics() override;
	virtual int	 VPhysicsGetObjectList(IPhysicsObject** pList, int listMax) override;

	void Think();
	virtual void PhysicsSimulate() override;
	virtual void Simulate(IPhysicsMotionController* pController, IPhysicsObject* pObject, float deltaTime, Vector& linear, AngularImpulse& angular) {}
	
	virtual bool TestCollision(const Ray_t& ray, unsigned int fContentsMask, trace_t& tr) override;

	void AddParticle(Vector center, Vector velocity);

	Class_T Classify() { return CLASS_BARNACLE; }

	int UpdateTransmitState() { return SetTransmitState(FL_EDICT_ALWAYS); }

	void SetPaintType(int type) { m_nPaintType.Set(type); }

	Vector m_vecStart;

	// networked particle info
	CNetworkVar(int, m_nActiveParticles);
	CNetworkVar(int, m_nPaintType);

	CUtlVector<Vector> m_vecSurfacePositions;
	CUtlVector<float> m_vecSurfaceVs;
	CUtlVector<float> m_vecSurfaceRs;
	CUtlVector<float> m_vecRadii;

	CUtlVector<IPhysicsObject*> m_vecToWake;

protected:
	friend class CBlobManager;

	CUtlVector<IPhysicsObject*> m_vecParticles;

private:
	int m_nNewParticles;
	int m_nActiveParticlesInternal;

};