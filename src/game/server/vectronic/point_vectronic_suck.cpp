//=========== Copyright © 2015, rHetorical, All rights reserved. =============
//
// Purpose:
//		
//=============================================================================

#include "cbase.h"
#include "point_vectronic_suck.h"
#include "prop_vectronic_projectile.h"
#include "prop_vecbox.h"

LINK_ENTITY_TO_CLASS(point_vectronic_suck, CPointSuck);
LINK_ENTITY_TO_CLASS(point_vecball_target, CPointSuck);

BEGIN_DATADESC(CPointSuck)

	DEFINE_THINKFUNC(SuckThink),

	DEFINE_KEYFIELD(m_bEnabled, FIELD_BOOLEAN, "enabled"),
	DEFINE_FIELD(m_bFoundPlayer, FIELD_BOOLEAN),
	DEFINE_KEYFIELD(m_intBallType, FIELD_INTEGER, "BallType"),
	DEFINE_KEYFIELD(m_flMagnitude, FIELD_FLOAT, "magnitude"),
	DEFINE_KEYFIELD(m_flRadius, FIELD_FLOAT, "radius"),
	DEFINE_KEYFIELD(m_flInnerRadius, FIELD_FLOAT, "inner_radius"),
	DEFINE_KEYFIELD(m_flConeOfInfluence, FIELD_FLOAT, "influence_cone"),

	DEFINE_INPUTFUNC(FIELD_VOID, "Enable", InputEnable),
	DEFINE_INPUTFUNC(FIELD_VOID, "Disable", InputDisable),

	DEFINE_INPUTFUNC(FIELD_FLOAT, "SetMagitude", InputSetMagitude),
	DEFINE_INPUTFUNC(FIELD_FLOAT, "SetRadius", InputSetRadius),

END_DATADESC();

// Spawnflags

// To add spawnflags, AddSpawnFlags(FLAG);
#define	SF_PUSH_TEST_LOS			0x0001
#define SF_PUSH_NO_FALLOFF			0x0004

void CPointSuck::DrawDebugGeometryOverlays(void)
{
	// Draw our center
	NDebugOverlay::Box(GetAbsOrigin(), -Vector(2, 2, 2), Vector(2, 2, 2), 0, 255, 0, 16, 0.05f);

	// Draw our radius
	NDebugOverlay::Sphere(GetAbsOrigin(), vec3_angle, m_flRadius, 0, 255, 0, 0, false, 0.05f);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPointSuck::Activate(void)
{
	if (m_bEnabled)
	{
		SetThink(&CPointSuck::SuckThink);
		SetNextThink(gpGlobals->curtime + 0.05f);
	}

	BaseClass::Activate();
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pTarget - 
//-----------------------------------------------------------------------------
void CPointSuck::SuckEntity(CBaseEntity *pTarget, const Vector &vecPushPoint)
{
	//if (m_bFoundPlayer)
		//return;

	Vector vecPushDir;
	vecPushDir = (pTarget->BodyTarget(vecPushPoint, false) - vecPushPoint);

	float dist = VectorNormalize(vecPushDir);

	float flFalloff = (HasSpawnFlags(SF_PUSH_NO_FALLOFF)) ? 1.0f : RemapValClamped(dist, m_flRadius, m_flRadius*0.25f, 0.0f, 1.0f);

	switch (pTarget->GetMoveType())
	{
	case MOVETYPE_NONE:
	case MOVETYPE_PUSH:
	case MOVETYPE_STEP:
	case MOVETYPE_NOCLIP:
		break;

	case MOVETYPE_VPHYSICS:
	{
		IPhysicsObject *pPhys = pTarget->VPhysicsGetObject();
		if (pPhys)
		{
			// UNDONE: Assume the velocity is for a 100kg object, scale with mass
			pPhys->ApplyForceCenter((m_flMagnitude * -1) * flFalloff * 100.0f * vecPushDir * pPhys->GetMass() * gpGlobals->frametime);
			return;
		}

		CPropParticleBall *pBall = dynamic_cast<CPropParticleBall*>(pTarget);
		if (pBall)
		{
			pBall->SetSpeed(0.0f);
			return;
		}

	}
	break;

	default:
	{
		Vector vecPush = ((m_flMagnitude * -1) * vecPushDir * flFalloff);
		if (pTarget->GetFlags() & FL_BASEVELOCITY)
		{
			vecPush = vecPush + pTarget->GetBaseVelocity();
		}
		if (vecPush.z > 0 && (pTarget->GetFlags() & FL_ONGROUND))
		{
			pTarget->SetGroundEntity(NULL);
			Vector origin = pTarget->GetAbsOrigin();
			origin.z += 1.0f;
			pTarget->SetAbsOrigin(origin);
		}

		pTarget->SetBaseVelocity(vecPush);
		pTarget->AddFlag(FL_BASEVELOCITY);
	}
	break;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPointSuck::SuckThink(void)
{
	// Get a collection of entities in a radius around us
	CBaseEntity *pEnts[256];
	int numEnts = UTIL_EntitiesInSphere(pEnts, 256, GetAbsOrigin(), m_flRadius, 0);

	if (m_flMagnitude < 0)
	{
		m_flMagnitude = m_flMagnitude * -1;
	}

	for (int i = 0; i < numEnts; i++)
	{
		// NDebugOverlay::Box( pEnts[i]->GetAbsOrigin(), -Vector(2,2,2), Vector(2,2,2), 0, 255, 0, 16, 0.1f );

		// Must be solid
		if (pEnts[i]->IsSolid() == false)
			continue;

		// Cannot be parented (only push parents)
		if (pEnts[i]->GetMoveParent() != NULL)
			continue;

		// Must be moveable
		if (pEnts[i]->GetMoveType() != MOVETYPE_VPHYSICS &&
			pEnts[i]->GetMoveType() != MOVETYPE_WALK &&
			pEnts[i]->GetMoveType() != MOVETYPE_STEP)
			continue;

		/*
		CBaseAnimating *pAnim = pEnts[i]->GetBaseAnimating();
		if (!pAnim)
			return;

		CPropParticleBall *pBall = dynamic_cast<CPropParticleBall*>(pEnts[i]);

		if (pBall == NULL)
		{
			continue;
		}
		*/

		CVecBox *pBox = dynamic_cast<CVecBox*>(pEnts[i]);

		if (pBox == NULL || pBox->LastBallHit() != m_intBallType)
			continue;

		// If we don't want to push players, don't
		//if (pEnts[i]->IsPlayer() && HasSpawnFlags(SF_PUSH_PLAYER) == false)
		//	continue;

		// If we don't want to push physics, don't
		//if (pEnts[i]->GetMoveType() == MOVETYPE_VPHYSICS && HasSpawnFlags(SF_PUSH_PHYSICS) == false)
			//continue;

		// See if they're within our cone of influence
		if (m_flConeOfInfluence != 0.0f)
		{
			Vector vecEndPos = pEnts[i]->BodyTarget(GetAbsOrigin(), false);
			Vector vecDirToTarget = (vecEndPos - GetAbsOrigin());
			VectorNormalize(vecDirToTarget);

			Vector vecDirection;
			AngleVectors(GetAbsAngles(), &vecDirection);

			// Test if it's within our cone of influence
			float flDot = DotProduct(vecDirToTarget, vecDirection);
			if (flDot < cos(DEG2RAD(m_flConeOfInfluence)))
				continue;
		}

		// Test for LOS if asked to
		if (HasSpawnFlags(SF_PUSH_TEST_LOS))
		{
			Vector vecStartPos = GetAbsOrigin();
			Vector vecEndPos = pEnts[i]->BodyTarget(vecStartPos, false);

			if (m_flInnerRadius != 0.0f)
			{
				// Find a point on our inner radius sphere to begin from
				Vector vecDirToTarget = (vecEndPos - vecStartPos);
				VectorNormalize(vecDirToTarget);
				vecStartPos = GetAbsOrigin() + (vecDirToTarget * m_flInnerRadius);
			}

			trace_t tr;
			UTIL_TraceLine(vecStartPos,
				pEnts[i]->BodyTarget(vecStartPos, false),
				MASK_SOLID_BRUSHONLY,
				this,
				COLLISION_GROUP_NONE,
				&tr);

			// Shielded
			if (tr.fraction < 1.0f && tr.m_pEnt != pEnts[i])
				continue;
		}

		// If the player is in the target radius, disable. This targeting system should not be obvious!
		/*
		if (pEnts[i]->IsPlayer())
		{
			if (!m_bFoundPlayer)
			{
				m_bFoundPlayer = true;
				DevMsg("Found Player~\n");
			}
		}
		else
		{
			if (m_bFoundPlayer)
			{
				m_bFoundPlayer = false;
				DevMsg("Lost Player~\n");
			}

			continue;
		}
		*/

		// Push it along
		SuckEntity(pEnts[i], GetAbsOrigin());
	}


	// Set us up for the next think
	SetNextThink(gpGlobals->curtime + 0.05f);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPointSuck::InputEnable(inputdata_t &inputdata)
{
	DevMsg("CPointSuck: Enabled\n");
	m_bEnabled = true;
	SetThink(&CPointSuck::SuckThink);
	SetNextThink(gpGlobals->curtime + 0.05f);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPointSuck::InputDisable(inputdata_t &inputdata)
{
	DevMsg("CPointSuck: Disabled\n");
	m_bEnabled = false;
	SetThink(NULL);
	SetNextThink(gpGlobals->curtime);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPointSuck::InputSetMagitude(inputdata_t &inputdata)
{
	SetMagitude(inputdata.value.Float());
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPointSuck::InputSetRadius(inputdata_t &inputdata)
{
	SetRadius(inputdata.value.Float());
}