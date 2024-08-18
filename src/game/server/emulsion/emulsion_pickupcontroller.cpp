#include "cbase.h"
#include "props.h"
#include "in_buttons.h"
#include "player.h"
#include "emulsion_pickupcontroller.h"
#include "vphysics/friction.h"
#include "physics_saverestore.h"
#include "physics_prop_ragdoll.h"
#include "emulsion_player.h"

//-----------------------------------------------------------------------------
// Purpose: Computes a local matrix for the player clamped to valid carry ranges
//-----------------------------------------------------------------------------
#define PLAYER_HOLD_LEVEL_EYES	-8 // when looking level, hold bottom of object 8 inches below eye level
#define PLAYER_HOLD_DOWN_FEET	2 // when looking down, hold bottom of object 0 inches from feet
#define PLAYER_HOLD_UP_EYES		24 // when looking up, hold bottom of object 24 inches above eye level
#define PLAYER_LOOK_PITCH_RANGE	30 // use a +/-30 degree range for the entire range of motion of pitch
#define PLAYER_REACH_DOWN_DISTANCE	24 // player can reach down 2ft below his feet (otherwise he'll hold the object above the bottom)

#define MASS_SPEED_SCALE	100
#define MAX_MASS			128
#define SIGN(x) ( (x) < 0 ? -1 : 1 )

const float DEFAULT_MAX_ANGULAR = 360.0f * 10.0f;
const float REDUCED_CARRY_MASS = 1.0f;

extern ConVar pl_normspeed;
ConVar player_throwforce("player_throwforce", "1000");
ConVar physcannon_maxmass("player_maxpickupmass", "250");
ConVar g_debug_playerpickup("g_debug_playerpickup", "false");

// tables

BEGIN_SIMPLE_DATADESC(game_shadowcontrol_params_t)
	DEFINE_FIELD(targetPosition, FIELD_POSITION_VECTOR),
	DEFINE_FIELD(targetRotation, FIELD_VECTOR),
	DEFINE_FIELD(maxAngular, FIELD_FLOAT),
	DEFINE_FIELD(maxDampAngular, FIELD_FLOAT),
	DEFINE_FIELD(maxSpeed, FIELD_FLOAT),
	DEFINE_FIELD(maxDampSpeed, FIELD_FLOAT),
	DEFINE_FIELD(dampFactor, FIELD_FLOAT),
	DEFINE_FIELD(teleportDistance, FIELD_FLOAT),
END_DATADESC()

BEGIN_SIMPLE_DATADESC(thrown_objects_t)
	DEFINE_FIELD(fTimeThrown, FIELD_TIME),
	DEFINE_FIELD(hEntity, FIELD_EHANDLE),
END_DATADESC()

BEGIN_SIMPLE_DATADESC(CGrabController)
	DEFINE_EMBEDDED(m_shadow),
	DEFINE_FIELD(m_timeToArrive, FIELD_FLOAT),
	DEFINE_FIELD(m_errorTime, FIELD_FLOAT),
	DEFINE_FIELD(m_error, FIELD_FLOAT),
	DEFINE_FIELD(m_contactAmount, FIELD_FLOAT),
	DEFINE_AUTO_ARRAY(m_savedRotDamping, FIELD_FLOAT),
	DEFINE_AUTO_ARRAY(m_savedMass, FIELD_FLOAT),
	DEFINE_FIELD(m_flLoadWeight, FIELD_FLOAT),
	DEFINE_FIELD(m_bCarriedEntityBlocksLOS, FIELD_BOOLEAN),
	DEFINE_FIELD(m_bIgnoreRelativePitch, FIELD_BOOLEAN),
	DEFINE_FIELD(m_attachedEntity, FIELD_EHANDLE),
	DEFINE_FIELD(m_angleAlignment, FIELD_FLOAT),
	DEFINE_FIELD(m_vecPreferredCarryAngles, FIELD_VECTOR),
	DEFINE_FIELD(m_bHasPreferredCarryAngles, FIELD_BOOLEAN),
	DEFINE_FIELD(m_flDistanceOffset, FIELD_FLOAT),
	DEFINE_FIELD(m_attachedAnglesPlayerSpace, FIELD_VECTOR),
	DEFINE_FIELD(m_attachedPositionObjectSpace, FIELD_VECTOR),
	DEFINE_FIELD(m_bAllowObjectOverhead, FIELD_BOOLEAN),
END_DATADESC()

LINK_ENTITY_TO_CLASS(player_pickup, CPlayerPickupController);

BEGIN_DATADESC(CPlayerPickupController)
	DEFINE_EMBEDDED(m_grabController),
	DEFINE_PHYSPTR(m_grabController.m_controller),
	DEFINE_FIELD(m_pPlayer, FIELD_CLASSPTR),
END_DATADESC()

void PlayerPickupObject(CBasePlayer* pPlayer, CBaseEntity* pObject)
{
	//Don't pick up if we don't have a phys object.
	if (pObject->VPhysicsGetObject() == NULL)
		return;

	CPlayerPickupController* pController = (CPlayerPickupController*)CBaseEntity::Create("player_pickup", pObject->GetAbsOrigin(), vec3_angle, pPlayer);

	if (!pController)
		return;

	pController->Init(pPlayer, pObject);
}

float PlayerPickupGetHeldObjectMass(CBaseEntity* pPickupControllerEntity, IPhysicsObject* pHeldObject)
{
	float mass = 0.0f;
	CPlayerPickupController* pController = dynamic_cast<CPlayerPickupController*>(pPickupControllerEntity);
	if (pController)
	{
		CGrabController& grab = pController->GetGrabController();
		mass = grab.GetSavedMass(pHeldObject);
	}
	return mass;
}

bool PlayerPickupControllerIsHoldingEntity(CBaseEntity* pPickupControllerEntity, CBaseEntity* pHeldEntity)
{
	CPlayerPickupController* pController = dynamic_cast<CPlayerPickupController*>(pPickupControllerEntity);

	return pController ? pController->IsHoldingEntity(pHeldEntity) : false;
}

static bool InContactWithHeavyObject(IPhysicsObject* pObject, float heavyMass)
{
	bool contact = false;
	IPhysicsFrictionSnapshot* pSnapshot = pObject->CreateFrictionSnapshot();
	while (pSnapshot->IsValid())
	{
		IPhysicsObject* pOther = pSnapshot->GetObject(1);
		if (!pOther->IsMoveable() || pOther->GetMass() > heavyMass)
		{
			contact = true;
			break;
		}
		pSnapshot->NextFrictionData();
	}
	pObject->DestroyFrictionSnapshot(pSnapshot);
	return contact;
}

static void MatrixOrthogonalize(matrix3x4_t& matrix, int column)
{
	Vector columns[3];
	int i;

	for (i = 0; i < 3; i++)
	{
		MatrixGetColumn(matrix, i, columns[i]);
	}

	int index0 = column;
	int index1 = (column + 1) % 3;
	int index2 = (column + 2) % 3;

	columns[index2] = CrossProduct(columns[index0], columns[index1]);
	columns[index1] = CrossProduct(columns[index2], columns[index0]);
	VectorNormalize(columns[index2]);
	VectorNormalize(columns[index1]);
	MatrixSetColumn(columns[index1], index1, matrix);
	MatrixSetColumn(columns[index2], index2, matrix);
}

static QAngle AlignAngles(const QAngle& angles, float cosineAlignAngle)
{
	matrix3x4_t alignMatrix;
	AngleMatrix(angles, alignMatrix);

	// NOTE: Must align z first
	for (int j = 3; --j >= 0; )
	{
		Vector vec;
		MatrixGetColumn(alignMatrix, j, vec);
		for (int i = 0; i < 3; i++)
		{
			if (fabs(vec[i]) > cosineAlignAngle)
			{
				vec[i] = SIGN(vec[i]);
				vec[(i + 1) % 3] = 0;
				vec[(i + 2) % 3] = 0;
				MatrixSetColumn(vec, j, alignMatrix);
				MatrixOrthogonalize(alignMatrix, j);
				break;
			}
		}
	}

	QAngle out;
	MatrixAngles(alignMatrix, out);
	return out;
}

static void ComputePlayerMatrix(CBasePlayer* pPlayer, matrix3x4_t& out)
{
	if (!pPlayer)
		return;

	CEmulsionPlayer* pePlayer = ((CEmulsionPlayer*)pPlayer);

	QAngle angles = pePlayer->LocalEyeAngles();// pPlayer->EyeAngles();
	Vector origin = pePlayer->StickEyeOrigin();//EyePosition();

	// 0-360 / -180-180
	//angles.x = init ? 0 : AngleDistance( angles.x, 0 );
	//angles.x = clamp( angles.x, -PLAYER_LOOK_PITCH_RANGE, PLAYER_LOOK_PITCH_RANGE );
	angles.x = 0;

	float fakeZ = (pePlayer->GetAbsOrigin() * -pePlayer->StickGravity()).Length();

	float feet = /*pPlayer->GetAbsOrigin().z*/fakeZ + (pPlayer->WorldAlignMins() * (-pePlayer->StickGravity())).Length();//pPlayer->WorldAlignMins().z;
	float eyes = fakeZ;//origin.z;
	float zoffset = 0;
	// moving up (negative pitch is up)
	if (angles.x < 0)
	{
		zoffset = RemapVal(angles.x, 0, -PLAYER_LOOK_PITCH_RANGE, PLAYER_HOLD_LEVEL_EYES, PLAYER_HOLD_UP_EYES);
	}
	else
	{
		zoffset = RemapVal(angles.x, 0, PLAYER_LOOK_PITCH_RANGE, PLAYER_HOLD_LEVEL_EYES, PLAYER_HOLD_DOWN_FEET + (feet - eyes));
	}
	origin.z += zoffset;
	angles.x = 0;
	AngleMatrix(angles, origin, out);
}


//-----------------------------------------------------------------------------
// Purpose: Finds the nearest ragdoll sub-piece to a location and returns it
// Input  : *pTarget - entity that is the potential ragdoll
//			&position - position we're testing against
// Output : IPhysicsObject - sub-object (if any)
//-----------------------------------------------------------------------------
IPhysicsObject* GetRagdollChildAtPosition(CBaseEntity* pTarget, const Vector& position)
{
	// Check for a ragdoll
	if (dynamic_cast<CRagdollProp*>(pTarget) == NULL)
		return NULL;

	// Get the root
	IPhysicsObject* pList[VPHYSICS_MAX_OBJECT_LIST_COUNT];
	int count = pTarget->VPhysicsGetObjectList(pList, ARRAYSIZE(pList));

	IPhysicsObject* pBestChild = NULL;
	float			flBestDist = 99999999.0f;
	float			flDist;
	Vector			vPos;

	// Find the nearest child to where we're looking
	for (int i = 0; i < count; i++)
	{
		pList[i]->GetPosition(&vPos, NULL);

		flDist = (position - vPos).LengthSqr();

		if (flDist < flBestDist)
		{
			pBestChild = pList[i];
			flBestDist = flDist;
		}
	}

	// Make this our base now
	pTarget->VPhysicsSwapObject(pBestChild);

	return pTarget->VPhysicsGetObject();
}

// CGrabController



CGrabController::CGrabController(void)
{
	m_shadow.dampFactor = 1.0;
	m_shadow.teleportDistance = 0;
	m_errorTime = 0;
	m_error = 0;
	// make this controller really stiff!
	m_shadow.maxSpeed = 1000;
	m_shadow.maxAngular = DEFAULT_MAX_ANGULAR;
	m_shadow.maxDampSpeed = m_shadow.maxSpeed * 2;
	m_shadow.maxDampAngular = m_shadow.maxAngular;
	m_attachedEntity = NULL;
	m_vecPreferredCarryAngles = vec3_angle;
	m_bHasPreferredCarryAngles = false;
	m_flDistanceOffset = 0;
	// NVNT constructing m_pControllingPlayer to NULL
	m_pControllingPlayer = NULL;
}

CGrabController::~CGrabController(void)
{
	DetachEntity(false);
}

void CGrabController::OnRestore()
{
	if (m_controller)
	{
		m_controller->SetEventHandler(this);
	}
}

void CGrabController::SetTargetPosition(const Vector& target, const QAngle& targetOrientation)
{
	m_shadow.targetPosition = target;
	m_shadow.targetRotation = targetOrientation;

	m_timeToArrive = gpGlobals->frametime;

	CBaseEntity* pAttached = GetAttached();
	if (pAttached)
	{
		IPhysicsObject* pObj = pAttached->VPhysicsGetObject();

		if (pObj != NULL)
		{
			pObj->Wake();
		}
		else
		{
			DetachEntity(false);
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : float
//-----------------------------------------------------------------------------
float CGrabController::ComputeError()
{
	if (m_errorTime <= 0)
		return 0;

	CBaseEntity* pAttached = GetAttached();
	if (pAttached)
	{
		Vector pos;
		IPhysicsObject* pObj = pAttached->VPhysicsGetObject();

		if (pObj)
		{
			pObj->GetShadowPosition(&pos, NULL);

			float error = (m_shadow.targetPosition - pos).Length();
			if (m_errorTime > 0)
			{
				if (m_errorTime > 1)
				{
					m_errorTime = 1;
				}
				float speed = error / m_errorTime;
				if (speed > m_shadow.maxSpeed)
				{
					error *= 0.5;
				}
				m_error = (1 - m_errorTime) * m_error + error * m_errorTime;
			}
		}
		else
		{
			DevMsg("Object attached to Physcannon has no physics object\n");
			DetachEntity(false);
			return 9999; // force detach
		}
	}

	if (pAttached->IsEFlagSet(EFL_IS_BEING_LIFTED_BY_BARNACLE))
	{
		m_error *= 3.0f;
	}

	m_errorTime = 0;

	return m_error;
}

void CGrabController::ComputeMaxSpeed(CBaseEntity* pEntity, IPhysicsObject* pPhysics)
{
	m_shadow.maxSpeed = 1000;
	m_shadow.maxAngular = DEFAULT_MAX_ANGULAR;

	// Compute total mass...
	float flMass = PhysGetEntityMass(pEntity);
	float flMaxMass = physcannon_maxmass.GetFloat();
	if (flMass <= flMaxMass)
		return;

	float flLerpFactor = clamp(flMass, flMaxMass, 500.0f);
	flLerpFactor = SimpleSplineRemapVal(flLerpFactor, flMaxMass, 500.0f, 0.0f, 1.0f);

	float invMass = pPhysics->GetInvMass();
	float invInertia = pPhysics->GetInvInertia().Length();

	float invMaxMass = 1.0f / MAX_MASS;
	float ratio = invMaxMass / invMass;
	invMass = invMaxMass;
	invInertia *= ratio;

	float maxSpeed = invMass * MASS_SPEED_SCALE * 200;
	float maxAngular = invInertia * MASS_SPEED_SCALE * 360;

	m_shadow.maxSpeed = Lerp(flLerpFactor, m_shadow.maxSpeed, maxSpeed);
	m_shadow.maxAngular = Lerp(flLerpFactor, m_shadow.maxAngular, maxAngular);
}


QAngle CGrabController::TransformAnglesToPlayerSpace(const QAngle& anglesIn, CBasePlayer* pPlayer)
{
	if (m_bIgnoreRelativePitch)
	{
		matrix3x4_t test;
		QAngle angleTest = ((CEmulsionPlayer*)pPlayer)->StickEyeAngles();//pPlayer->EyeAngles();
		angleTest.x = 0;
		AngleMatrix(angleTest, test);
		return TransformAnglesToLocalSpace(anglesIn, test);
	}
	return TransformAnglesToLocalSpace(anglesIn, pPlayer->EntityToWorldTransform());
}

QAngle CGrabController::TransformAnglesFromPlayerSpace(const QAngle& anglesIn, CBasePlayer* pPlayer)
{
	if (m_bIgnoreRelativePitch)
	{
		matrix3x4_t test;
		QAngle angleTest = ((CEmulsionPlayer*)pPlayer)->StickEyeAngles(); //pPlayer->EyeAngles();
		angleTest.x = 0;
		AngleMatrix(angleTest, test);
		return TransformAnglesToWorldSpace(anglesIn, test);
	}
	return TransformAnglesToWorldSpace(anglesIn, pPlayer->EntityToWorldTransform());
}


void CGrabController::AttachEntity(CBasePlayer* pPlayer, CBaseEntity* pEntity, IPhysicsObject* pPhys, bool bIsMegaPhysCannon, const Vector& vGrabPosition, bool bUseGrabPosition)
{
	// play the impact sound of the object hitting the player
	// used as feedback to let the player know he picked up the object
	int hitMaterial = pPhys->GetMaterialIndex();
	int playerMaterial = pPlayer->VPhysicsGetObject() ? pPlayer->VPhysicsGetObject()->GetMaterialIndex() : hitMaterial;
	PhysicsImpactSound(pPlayer, pPhys, CHAN_STATIC, hitMaterial, playerMaterial, 1.0, 64);
	Vector position;
	QAngle angles;
	pPhys->GetPosition(&position, &angles);
	// If it has a preferred orientation, use that instead.
	Pickup_GetPreferredCarryAngles(pEntity, pPlayer, pPlayer->EntityToWorldTransform(), angles);

	//	ComputeMaxSpeed( pEntity, pPhys );

		// If we haven't been killed by a grab, we allow the gun to grab the nearest part of a ragdoll
	if (bUseGrabPosition)
	{
		IPhysicsObject* pChild = GetRagdollChildAtPosition(pEntity, vGrabPosition);

		if (pChild)
		{
			pPhys = pChild;
		}
	}

	// Carried entities can never block LOS
	m_bCarriedEntityBlocksLOS = pEntity->BlocksLOS();
	pEntity->SetBlocksLOS(false);
	m_controller = physenv->CreateMotionController(this);
	m_controller->AttachObject(pPhys, true);
	// Don't do this, it's causing trouble with constraint solvers.
	//m_controller->SetPriority( IPhysicsMotionController::HIGH_PRIORITY );

	pPhys->Wake();
	PhysSetGameFlags(pPhys, FVPHYSICS_PLAYER_HELD);

	SetTargetPosition(position, angles);

	m_attachedEntity = pEntity;
	IPhysicsObject* pList[VPHYSICS_MAX_OBJECT_LIST_COUNT];
	int count = pEntity->VPhysicsGetObjectList(pList, ARRAYSIZE(pList));
	m_flLoadWeight = 0;
	float damping = 10;
	float flFactor = count / 7.5f;
	if (flFactor < 1.0f)
	{
		flFactor = 1.0f;
	}

	for (int i = 0; i < count; i++)
	{
		float mass = pList[i]->GetMass();
		pList[i]->GetDamping(NULL, &m_savedRotDamping[i]);
		m_flLoadWeight += mass;
		m_savedMass[i] = mass;

		// reduce the mass to prevent the player from adding crazy amounts of energy to the system
		pList[i]->SetMass(REDUCED_CARRY_MASS / flFactor);
		pList[i]->SetDamping(NULL, &damping);
	}

	// NVNT setting m_pControllingPlayer to the player attached
	m_pControllingPlayer = pPlayer;

	// Give extra mass to the phys object we're actually picking up
	pPhys->SetMass(REDUCED_CARRY_MASS);
	pPhys->EnableDrag(false);

	m_errorTime = bIsMegaPhysCannon ? -1.5f : -1.0f; // 1 seconds until error starts accumulating
	m_error = 0;
	m_contactAmount = 0;
	m_bIgnoreRelativePitch = true;
	m_attachedAnglesPlayerSpace = TransformAnglesToPlayerSpace(angles, pPlayer);
	if (m_angleAlignment != 0)
	{
		m_attachedAnglesPlayerSpace = AlignAngles(m_attachedAnglesPlayerSpace, m_angleAlignment);
	}

	// Ragdolls don't offset this way
	if (dynamic_cast<CRagdollProp*>(pEntity))
	{
		m_attachedPositionObjectSpace.Init();
	}
	else
	{
		VectorITransform(pEntity->WorldSpaceCenter(), pEntity->EntityToWorldTransform(), m_attachedPositionObjectSpace);
	}

	// If it's a prop, see if it has desired carry angles
	CPhysicsProp* pProp = dynamic_cast<CPhysicsProp*>(pEntity);
	if (pProp)
	{
		m_bHasPreferredCarryAngles = pProp->GetPropDataAngles("preferred_carryangles", m_vecPreferredCarryAngles);
		m_flDistanceOffset = pProp->GetCarryDistanceOffset();
	}
	else
	{
		m_bHasPreferredCarryAngles = false;
		m_flDistanceOffset = 0;
	}

	m_bAllowObjectOverhead = IsObjectAllowedOverhead(pEntity);

	pEntity->SetSolid(SOLID_NONE);
	pEntity->SetSolidFlags(FSOLID_NOT_SOLID);
	((CEmulsionPlayer*)pPlayer)->m_pGrabber = this;
	((CEmulsionPlayer*)pPlayer)->m_bIsHoldingObject = true;
}

static void ClampPhysicsVelocity(IPhysicsObject* pPhys, float linearLimit, float angularLimit)
{
	Vector vel;
	AngularImpulse angVel;
	pPhys->GetVelocity(&vel, &angVel);
	float speed = VectorNormalize(vel) - linearLimit;
	float angSpeed = VectorNormalize(angVel) - angularLimit;
	speed = speed < 0 ? 0 : -speed;
	angSpeed = angSpeed < 0 ? 0 : -angSpeed;
	vel *= speed;
	angVel *= angSpeed;
	pPhys->AddVelocity(&vel, &angVel);
}

void CGrabController::DetachEntity(bool bClearVelocity)
{
	Assert(!PhysIsInCallback());
	CBaseEntity* pEntity = GetAttached();
	if (pEntity)
	{
		// Restore the LS blocking state
		pEntity->SetBlocksLOS(m_bCarriedEntityBlocksLOS);
		IPhysicsObject* pList[VPHYSICS_MAX_OBJECT_LIST_COUNT];
		int count = pEntity->VPhysicsGetObjectList(pList, ARRAYSIZE(pList));
		for (int i = 0; i < count; i++)
		{
			IPhysicsObject* pPhys = pList[i];
			if (!pPhys)
				continue;

			// on the odd chance that it's gone to sleep while under anti-gravity
			pPhys->EnableDrag(true);
			pPhys->Wake();
			pPhys->SetMass(m_savedMass[i]);
			pPhys->SetDamping(NULL, &m_savedRotDamping[i]);
			PhysClearGameFlags(pPhys, FVPHYSICS_PLAYER_HELD);
			if (bClearVelocity)
			{
				PhysForceClearVelocity(pPhys);
			}
			else
			{
				ClampPhysicsVelocity(pPhys, pl_normspeed.GetFloat() * 1.5f, 2.0f * 360.0f);
			}

			CEmulsionPlayer* pPlayer = ((CEmulsionPlayer*)UTIL_GetLocalPlayer());
			pPlayer->m_bIsHoldingObject = false;
		}

		pEntity->SetSolid(SOLID_VPHYSICS);
		pEntity->RemoveSolidFlags(FSOLID_NOT_SOLID);
	}

	m_attachedEntity = NULL;
	physenv->DestroyMotionController(m_controller);
	m_controller = NULL;
}



IMotionEvent::simresult_e CGrabController::Simulate(IPhysicsMotionController* pController, IPhysicsObject* pObject, float deltaTime, Vector& linear, AngularImpulse& angular)
{
	game_shadowcontrol_params_t shadowParams = m_shadow;
	if (InContactWithHeavyObject(pObject, GetLoadWeight()))
	{
		m_contactAmount = Approach(0.1f, m_contactAmount, deltaTime * 2.0f);
	}
	else
	{
		m_contactAmount = Approach(1.0f, m_contactAmount, deltaTime * 2.0f);
	}
	shadowParams.maxAngular = m_shadow.maxAngular * m_contactAmount * m_contactAmount * m_contactAmount;
	m_timeToArrive = pObject->ComputeShadowControl(shadowParams, m_timeToArrive, deltaTime);

	// Slide along the current contact points to fix bouncing problems
	Vector velocity;
	AngularImpulse angVel;
	pObject->GetVelocity(&velocity, &angVel);
	PhysComputeSlideDirection(pObject, velocity, angVel, &velocity, &angVel, GetLoadWeight());
	pObject->SetVelocityInstantaneous(&velocity, NULL);

	linear.Init();
	angular.Init();
	m_errorTime += deltaTime;

	return SIM_LOCAL_ACCELERATION;
}

float CGrabController::GetSavedMass(IPhysicsObject* pObject)
{
	CBaseEntity* pHeld = m_attachedEntity;
	if (pHeld)
	{
		if (pObject->GetGameData() == (void*)pHeld)
		{
			IPhysicsObject* pList[VPHYSICS_MAX_OBJECT_LIST_COUNT];
			int count = pHeld->VPhysicsGetObjectList(pList, ARRAYSIZE(pList));
			for (int i = 0; i < count; i++)
			{
				if (pList[i] == pObject)
					return m_savedMass[i];
			}
		}
	}
	return 0.0f;
}

bool CGrabController::UpdateObject(CBasePlayer* pPlayer, float flError)
{
	CBaseEntity* pEntity = GetAttached();
	if (!pEntity || ComputeError() > flError || pPlayer->GetGroundEntity() == pEntity || !pEntity->VPhysicsGetObject())
	{
		return false;
	}

	//Adrian: Oops, our object became motion disabled, let go!
	IPhysicsObject* pPhys = pEntity->VPhysicsGetObject();
	if (pPhys && pPhys->IsMoveable() == false)
	{
		return false;
	}

	Vector forward, right, up;
	QAngle playerAngles = ((CEmulsionPlayer*)pPlayer)->StickEyeAngles();//pPlayer->EyeAngles();
	AngleVectors(playerAngles, &forward, &right, &up);

	forward = ((CEmulsionPlayer*)pPlayer)->GetForward_Stick();

	float pitch = AngleDistance(playerAngles.x, 0);

	//if (!m_bAllowObjectOverhead)
	//{
	//	playerAngles.x = clamp(pitch, -75, 75);
	//}
	//else
	//{
	//	playerAngles.x = clamp(pitch, -90, 75);
	//}

	// Now clamp a sphere of object radius at end to the player's bbox
	Vector radial = physcollision->CollideGetExtent(pPhys->GetCollide(), vec3_origin, pEntity->GetAbsAngles(), -forward);
	//Vector player2d = pPlayer->CollisionProp()->OBBMaxs();
	Vector player2d = pPlayer->GetPlayerMaxs();
	float playerRadius = player2d.Length2D();
	float radius = playerRadius + fabs(DotProduct(forward, radial));

	float distance = 24 + (radius * 2.0f);

	// Add the prop's distance offset
	distance += m_flDistanceOffset;

	Vector start = ((CEmulsionPlayer*)pPlayer)->StickEyeOrigin();
	Vector end = start + (forward * distance);

	trace_t	tr;
	CTraceFilterSkipTwoEntities traceFilter(pPlayer, pEntity, COLLISION_GROUP_NONE);
	Ray_t ray;
	ray.Init(start, end);
	enginetrace->TraceRay(ray, MASK_SOLID_BRUSHONLY, &traceFilter, &tr);

	if (tr.fraction < 0.5)
	{
		end = start + forward * (radius * 0.5f);
	}
	else if (tr.fraction <= 1.0f)
	{
		end = start + forward * (distance - radius);
	}

	Vector playerMins, playerMaxs, nearest;
	pPlayer->CollisionProp()->WorldSpaceAABB(&playerMins, &playerMaxs);

	//Vector playerLine = pPlayer->CollisionProp()->WorldSpaceCenter();
	Vector playerLine = ((CEmulsionPlayer*)pPlayer)->GetHalfHeight_Stick();

	int k = 0;
	Vector stickgrav = ((CEmulsionPlayer*)pPlayer)->StickGravity();
	
	if (stickgrav == Vector(0, 0, 1))
		stickgrav *= -0.99;
	else
		stickgrav *= -1;
	
	// TODO: investigate this -klax
	//CalcClosestPointOnLine(end, playerLine + Vector(0, 0, playerMins.z), playerLine + Vector(0, 0, playerMaxs.z), nearest, NULL);
	CalcClosestPointOnLine(end, playerLine + (stickgrav * playerMaxs), playerLine + stickgrav * playerMaxs, nearest, NULL);

	if (!m_bAllowObjectOverhead)
	{
		Vector delta = end - nearest;
		float len = VectorNormalize(delta);
		if (len < radius)
		{
			end = nearest + radius * delta;
		}
	}

	//Show overlays of radius
	if (g_debug_playerpickup.GetBool())
	{
		NDebugOverlay::Box(end, -Vector(2, 2, 2), Vector(2, 2, 2), 0, 255, 0, true, 0);

		NDebugOverlay::Box(GetAttached()->WorldSpaceCenter(),
			-Vector(radius, radius, radius),
			Vector(radius, radius, radius),
			255, 0, 0,
			true,
			0.0f);
	}

	QAngle angles = TransformAnglesFromPlayerSpace(m_attachedAnglesPlayerSpace, pPlayer);

	// If it has a preferred orientation, update to ensure we're still oriented correctly.
	Pickup_GetPreferredCarryAngles(pEntity, pPlayer, pPlayer->EntityToWorldTransform(), angles);

	// We may be holding a prop that has preferred carry angles
	if (m_bHasPreferredCarryAngles)
	{
		matrix3x4_t tmp;
		ComputePlayerMatrix(pPlayer, tmp);
		angles = TransformAnglesToWorldSpace(m_vecPreferredCarryAngles, tmp);
	}

	matrix3x4_t attachedToWorld;
	Vector offset;
	AngleMatrix(angles, attachedToWorld);
	VectorRotate(m_attachedPositionObjectSpace, attachedToWorld, offset);

	SetTargetPosition(end - offset, angles);

	return true;
}

//-----------------------------------------------------------------------------
// Is this an object that the player is allowed to lift to a position 
// directly overhead? The default behavior prevents lifting objects directly
// overhead, but there are exceptions for gameplay purposes.
//-----------------------------------------------------------------------------
bool CGrabController::IsObjectAllowedOverhead(CBaseEntity* pEntity)
{
	// Allow props that are specifically flagged as such
	CPhysicsProp* pPhysProp = dynamic_cast<CPhysicsProp*>(pEntity);
	if (pPhysProp != NULL && pPhysProp->HasInteraction(PROPINTER_PHYSGUN_ALLOW_OVERHEAD))
		return true;

	// String checks are fine here, we only run this code one time- when the object is picked up.
	if (pEntity->ClassMatches("grenade_helicopter"))
		return true;

	if (pEntity->ClassMatches("weapon_striderbuster"))
		return true;

	return false;
}

// CPlayerPickupController

void CPlayerPickupController::Init(CBasePlayer* pPlayer, CBaseEntity* pObject)
{
	// Holster player's weapon
	if (pPlayer->GetActiveWeapon())
	{
		if (!pPlayer->GetActiveWeapon()->CanHolster() || !pPlayer->GetActiveWeapon()->Holster())
		{
			Shutdown();
			return;
		}
	}

	// If the target is debris, convert it to non-debris
	if (pObject->GetCollisionGroup() == COLLISION_GROUP_DEBRIS)
	{
		// Interactive debris converts back to debris when it comes to rest
		pObject->SetCollisionGroup(COLLISION_GROUP_INTERACTIVE_DEBRIS);
	}

	// done so I'll go across level transitions with the player
	SetParent(pPlayer);
	m_grabController.SetIgnorePitch(true);
	m_grabController.SetAngleAlignment(DOT_30DEGREE);
	m_pPlayer = pPlayer;
	IPhysicsObject* pPhysics = pObject->VPhysicsGetObject();

	Pickup_OnPhysGunPickup(pObject, m_pPlayer, PICKED_UP_BY_PLAYER);

	m_grabController.AttachEntity(pPlayer, pObject, pPhysics, false, vec3_origin, false);

	m_pPlayer->m_Local.m_iHideHUD |= HIDEHUD_WEAPONSELECTION;
	m_pPlayer->SetUseEntity(this);
}


//-----------------------------------------------------------------------------
// Purpose: 
// Input  : bool - 
//-----------------------------------------------------------------------------
void CPlayerPickupController::Shutdown(bool bThrown)
{
	CBaseEntity* pObject = m_grabController.GetAttached();

	bool bClearVelocity = false;
	if (!bThrown && pObject && pObject->VPhysicsGetObject() && pObject->VPhysicsGetObject()->GetContactPoint(NULL, NULL))
	{
		bClearVelocity = true;
	}

	m_grabController.DetachEntity(bClearVelocity);
	
	if (pObject != NULL)
	{
		Pickup_OnPhysGunDrop(pObject, m_pPlayer, bThrown ? THROWN_BY_PLAYER : DROPPED_BY_PLAYER);
	}

	if (m_pPlayer)
	{
		m_pPlayer->SetUseEntity(NULL);
		if (m_pPlayer->GetActiveWeapon())
		{
			if (!m_pPlayer->GetActiveWeapon()->Deploy())
			{
				// We tried to restore the player's weapon, but we couldn't.
				// This usually happens when they're holding an empty weapon that doesn't
				// autoswitch away when out of ammo. Switch to next best weapon.
				m_pPlayer->SwitchToNextBestWeapon(NULL);
			}
		}

		m_pPlayer->m_Local.m_iHideHUD &= ~HIDEHUD_WEAPONSELECTION;
	}
	Remove();
}


void CPlayerPickupController::Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value)
{
	if (ToBasePlayer(pActivator) == m_pPlayer)
	{
		CBaseEntity* pAttached = m_grabController.GetAttached();

		// UNDONE: Use vphysics stress to decide to drop objects
		// UNDONE: Must fix case of forcing objects into the ground you're standing on (causes stress) before that will work
		if (!pAttached || useType == USE_OFF || (m_pPlayer->m_nButtons & IN_ATTACK2) || m_grabController.ComputeError() > 12)
		{
			Shutdown();
			return;
		}

		//Adrian: Oops, our object became motion disabled, let go!
		IPhysicsObject* pPhys = pAttached->VPhysicsGetObject();
		if (pPhys && pPhys->IsMoveable() == false)
		{
			Shutdown();
			return;
		}

#if STRESS_TEST
		vphysics_objectstress_t stress;
		CalculateObjectStress(pPhys, pAttached, &stress);
		if (stress.exertedStress > 250)
		{
			Shutdown();
			return;
		}
#endif
		// +ATTACK will throw phys objects
		if (m_pPlayer->m_nButtons & IN_ATTACK)
		{
			Shutdown(true);
			Vector vecLaunch;
			m_pPlayer->EyeVectors(&vecLaunch);
			// JAY: Scale this with mass because some small objects really go flying
			float massFactor = clamp(pPhys->GetMass(), 0.5, 15);
			massFactor = RemapVal(massFactor, 0.5, 15, 0.5, 4);
			vecLaunch *= player_throwforce.GetFloat() * massFactor;

			pPhys->ApplyForceCenter(vecLaunch);
			AngularImpulse aVel = RandomAngularImpulse(-10, 10) * massFactor;
			pPhys->ApplyTorqueCenter(aVel);
			return;
		}

		if (useType == USE_SET)
		{
			// update position
			m_grabController.UpdateObject(m_pPlayer, 12);
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pEnt - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CPlayerPickupController::IsHoldingEntity(CBaseEntity* pEnt)
{
	return (m_grabController.GetAttached() == pEnt);
}