#include "cbase.h"
#include "paint_stream.h"
#include "saverestore_utlvector.h"
#include "dt_utlvector_send.h"
#include "collisionutils.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ConVar paintblob_stream_radius("paintblob_stream_radius", "8", FCVAR_REPLICATED | FCVAR_DEVELOPMENTONLY);
ConVar paintblob_stream_max_blobs("paintblob_stream_max_blobs", "350", FCVAR_REPLICATED | FCVAR_DEVELOPMENTONLY);

LINK_ENTITY_TO_CLASS(env_paint_stream, CPaintBlobStream)

IMPLEMENT_SERVERCLASS_ST(CPaintBlobStream, DT_PaintBlobStream)
	
	SendPropUtlVector(
		SENDINFO_UTLVECTOR(m_vecSurfacePositions),
		paintblob_stream_max_blobs.GetInt(), // max elements4
		SendPropVector(NULL, 0, sizeof(Vector), 32, SPROP_COORD)), // WAS SPROP_COORD
	SendPropUtlVector(
		SENDINFO_UTLVECTOR(m_vecSurfaceVs),
		paintblob_stream_max_blobs.GetInt(), // max elements
		SendPropFloat(NULL, 0, sizeof(float), 6, 0, 0.0, 1.0)),
	SendPropUtlVector(
		SENDINFO_UTLVECTOR(m_vecSurfaceRs),
		paintblob_stream_max_blobs.GetInt(), // max elements
		SendPropFloat(NULL, 0, sizeof(float), 6, 0, 0.0, 2.0)),
	SendPropUtlVector(
		SENDINFO_UTLVECTOR(m_vecRadii),
		paintblob_stream_max_blobs.GetInt(), // max elements
		SendPropFloat(NULL, 0, sizeof(float), 6, 0, 0.0, 2.0)),

	SendPropInt(SENDINFO(m_nActiveParticles), 12, SPROP_UNSIGNED),
	SendPropInt(SENDINFO(m_nPaintType), 12, SPROP_UNSIGNED),
END_SEND_TABLE()

BEGIN_DATADESC(CPaintBlobStream)

DEFINE_UTLVECTOR(m_vecSurfacePositions, FIELD_POSITION_VECTOR),
DEFINE_UTLVECTOR(m_vecSurfaceVs, FIELD_FLOAT),
DEFINE_UTLVECTOR(m_vecSurfaceRs, FIELD_FLOAT),

END_DATADESC()

CPaintBlobStream::CPaintBlobStream() {
	m_nActiveParticlesInternal = 0;
	m_nNewParticles = 0;
}

void CPaintBlobStream::Precache() {
	PrecacheModel("models/props/metal_box.mdl");
	BaseClass::Precache();
}

void CPaintBlobStream::Spawn() {
	
	Precache();
	BaseClass::Spawn();

	SetModel("models/props/metal_box.mdl");

	SetThink(&CPaintBlobStream::Think);

	SetHullType(HULL_SMALL_CENTERED);
	SetHullSizeNormal();
	
	SetSolid(SOLID_BBOX);
	AddSolidFlags(FSOLID_FORCE_WORLD_ALIGNED | FSOLID_NOT_STANDABLE);

	SetMoveType(MOVETYPE_VPHYSICS);
	AddSolidFlags(FSOLID_CUSTOMRAYTEST | FSOLID_CUSTOMBOXTEST);

	AddEFlags(EFL_NO_DISSOLVE);
	SetBloodColor(BLOOD_COLOR_YELLOW);
	ClearEffects();
	m_iHealth = 200;
	m_flFieldOfView = -1.0;
	m_NPCState = NPC_STATE_IDLE;

	m_vecStart = GetAbsOrigin();

	SetAbsAngles(QAngle(0, 0, 0));

	m_vecSurfaceVs.EnsureCount(paintblob_stream_max_blobs.GetInt());
	m_vecSurfaceRs.EnsureCount(paintblob_stream_max_blobs.GetInt());
	
	m_vecSurfaceVs[0] = 0.0;
	m_vecSurfaceRs[0] = 1.0;
	
	for (int i = 1; i < paintblob_stream_max_blobs.GetInt(); i++)
	{
		m_vecSurfaceVs[i] = m_vecSurfaceVs[i - 1];
		m_vecSurfaceRs[i] = m_vecSurfaceRs[i - 1];
	}
	
	NPCInit();
}

void CPaintBlobStream::VPhysicsCollision(int index, gamevcollisionevent_t* pEvent) {
	BaseClass::VPhysicsCollision(index, pEvent);
	
	// do parented_func callback here for any blob
	// that decides it wants to kiss a wall
	
	Vector contact, position;
	pEvent->pInternalData->GetContactPoint(contact);
	pEvent->pObjects[index]->GetPosition(&position, nullptr);
	

	trace_t tr;
	UTIL_TraceLine(position, pEvent->preVelocity[1].Normalized() * 2.0f, MASK_ALL, this, COLLISION_GROUP_PLAYER_MOVEMENT, &tr);
	
	if (tr.m_pEnt) {

		if (tr.DidHitWorld()) {
			model_t* mdl = tr.m_pEnt->GetModel();
			engine->SpherePaintSurface(mdl, position, m_nPaintType, 35.0f, 5);
		}
	}

}

bool CPaintBlobStream::CreateVPhysics() {

	objectparams_t params = g_PhysDefaultObjectParams;
	params.pGameData = static_cast<void*>(this);

	int surfaceIndex = physprops->GetSurfaceIndex("water");

	for (int i = 0; i < m_nActiveParticles; i++) {
		m_vecParticles.AddToTail();
		m_vecParticles[i] = physenv->CreateSphereObject(paintblob_stream_radius.GetFloat(), surfaceIndex, m_vecSurfacePositions[i], GetAbsAngles(), &params, false);
		
		if (m_vecParticles[i]) {
			Vector vVelocity = Vector(RandomFloat(-1, 1), RandomFloat(-1, 1), RandomFloat(1, 2)) * 10.0f;
			m_vecParticles[i]->SetVelocity(&vVelocity, NULL);
			PhysSetGameFlags(m_vecParticles[i], FVPHYSICS_NO_SELF_COLLISIONS | FVPHYSICS_MULTIOBJECT_ENTITY); // call collisionruleschanged if this changes dynamically
			m_vecParticles[i]->SetGameIndex(i);

			m_vecParticles[i]->SetMass(10.0f);
			m_vecParticles[i]->EnableGravity(true);
			m_vecParticles[i]->EnableDrag(true);

			float flDamping = 0.5f;
			float flAngDamping = 0.5f;
			m_vecParticles[i]->SetDamping(&flDamping, &flAngDamping);
		}
	}

	return true;
}

int CPaintBlobStream::VPhysicsGetObjectList(IPhysicsObject** pList, int listMax) {

	int count = 0;
	for (int i = 0; i < listMax && i < m_vecParticles.Count(); i++)
	{
		if (m_vecSurfaceRs[i] > 0.0f)
		{
			pList[count++] = m_vecParticles[i];
		}
	}

	return count;
}

void CPaintBlobStream::Think() {
	// TODO: send particle positions??? this is bad code /: slow code D^=
	SetAbsOrigin(UTIL_PlayerByIndex(1)->GetAbsOrigin());

	for (int i = 0; i < m_vecParticles.Count(); i++) {

		QAngle ang;
		m_vecParticles[i]->GetPosition(&m_vecSurfacePositions[i], &ang); // TODO: try nullptr > ang?
		int k = 0;
	}

	m_nActiveParticles = m_vecParticles.Count();

	// without this, LATCH_SIMULATION_VAR will never be triggered
	SetSimulationTime(gpGlobals->curtime);
	SetNextThink(gpGlobals->curtime + 0.0001);
	BaseClass::Think();
}

void CPaintBlobStream::PhysicsSimulate() {
	BaseClass::PhysicsSimulate();

}

void CPaintBlobStream::AddParticle(Vector center, Vector velocity) {

	if (m_vecParticles.Count() >= paintblob_stream_max_blobs.GetInt())
		return;
	
	objectparams_t params = g_PhysDefaultObjectParams;
	params.pGameData = static_cast<void*>(this);

	int surfaceIndex = physprops->GetSurfaceIndex("water");
	int i = m_vecParticles.Count();
	
	m_vecSurfacePositions.AddToTail();
	m_vecSurfacePositions[i] = center;

	m_vecRadii.AddToTail();
	m_vecRadii[i] = RandomFloat(0.4f, 1.3f);

	m_vecParticles.AddToTail();
	m_vecParticles[i] = physenv->CreateSphereObject(paintblob_stream_radius.GetFloat(), surfaceIndex, m_vecSurfacePositions[i], GetAbsAngles(), &params, false);

	if (m_vecParticles[i]) {
		m_vecParticles[i]->SetVelocity(&velocity, NULL);
		PhysSetGameFlags(m_vecParticles[i], FVPHYSICS_NO_SELF_COLLISIONS | FVPHYSICS_MULTIOBJECT_ENTITY); // call collisionruleschanged if this changes dynamically
		m_vecParticles[i]->SetGameIndex(i);

		m_vecParticles[i]->SetMass(2.5f); // 10.0f
		m_vecParticles[i]->EnableGravity(true);
		m_vecParticles[i]->EnableDrag(false);
		
		float flDamping = 0.5f;
		float flAngDamping = 0.5f;
		m_vecParticles[i]->SetDamping(&flDamping, &flAngDamping);
	}
}

bool CPaintBlobStream::TestCollision(const Ray_t& ray, unsigned int fContentsMask, trace_t& tr)
{
	int nLastHit = -1;

	if (ray.m_IsRay)
	{
		float d1, d2;

		for (int i = 0; i < m_vecParticles.Count(); i++)
		{
			if (m_vecSurfaceRs[i] > 0.0f && IntersectRayWithSphere(ray.m_Start, ray.m_Delta, m_vecSurfacePositions[i], m_vecRadii[i], &d1, &d2))
			{
				if (d1 < tr.fraction)
				{
					NDebugOverlay::Box(m_vecSurfacePositions[i], Vector(-2, -2, -2), Vector(2, 2, 2), 255, 0, 0, 20, .1);
					nLastHit = i;
					tr.fraction = d1;
				}
			}
		}
		if (nLastHit != -1)
		{
			tr.m_pEnt = this;
			tr.startpos = ray.m_Start;
			tr.endpos = ray.m_Start + ray.m_Delta * tr.fraction;
			tr.contents = CONTENTS_SOLID;
			tr.hitbox = nLastHit;
			tr.hitgroup = HITGROUP_GENERIC;
			tr.plane.dist = tr.endpos.Length();
			tr.plane.normal = (tr.endpos - m_vecSurfacePositions[nLastHit]) * (1 / paintblob_stream_radius.GetFloat());
			tr.plane.type = 0;
			tr.physicsbone = nLastHit;
		}
	}
	else
	{
		float m_flRadius = paintblob_stream_radius.GetFloat();

		// FIXME: This isn't a valid test, Jay needs to make it real
		Vector vecMin = Vector(-m_flRadius, -m_flRadius, -m_flRadius) - ray.m_Extents;
		Vector vecMax = Vector(m_flRadius, m_flRadius, m_flRadius) + ray.m_Extents;

		trace_t boxtrace;

		tr.fraction = 1.0;

		for (int i = 0; i < m_nActiveParticles; i++)
		{
			if (m_vecSurfaceRs[i] > 0.0f && IntersectRayWithBox(m_vecSurfacePositions[i] - ray.m_Start, -ray.m_Delta, vecMin, vecMax, 0.0, &boxtrace))
			{
				if (boxtrace.fraction < tr.fraction)
				{
					if (tr.startsolid && !IsBoxIntersectingSphere(ray.m_Start - ray.m_Extents, ray.m_Start + ray.m_Extents, m_vecSurfacePositions[i], m_flRadius))
					{
						tr.startsolid = false;
						tr.allsolid = false;
					}
					else if (tr.allsolid && !IsBoxIntersectingSphere(ray.m_Start + ray.m_Delta - ray.m_Extents, ray.m_Start + ray.m_Delta + ray.m_Extents, m_vecSurfacePositions[i], m_flRadius))
					{
						tr.allsolid = false;
					}

					tr = boxtrace;
					tr.startpos = ray.m_Start;
					tr.endpos = ray.m_Start + tr.fraction * ray.m_Delta;
					nLastHit = i;
				}
			}
		}
		
		if (tr.fraction < 1.0)
		{
			tr.contents = CONTENTS_SOLID;
			tr.m_pEnt = this;
			tr.hitbox = nLastHit;
			tr.hitgroup = HITGROUP_GENERIC;
			tr.physicsbone = nLastHit;
		}
	}

	return true;
}