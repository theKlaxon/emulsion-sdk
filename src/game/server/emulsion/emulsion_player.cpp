#include "cbase.h"
#include "emulsion_player.h"
#include "emulsion_pickupcontroller.h"
#include "player_pickup.h"
#include "in_buttons.h"
#include "ai_basenpc.h"
#include "shareddefs.h"
#include "globalstate.h"
#include "blob_manager.h"
#include "..\server\ilagcompensationmanager.h"
#include "..\game\shared\portal2\paint_enum.h"

#define PLAYER_MDL "models/player.mdl"
#define PLAYER_MAX_LIFT_MASS 85
#define PLAYER_MAX_LIFT_SIZE 128

#define SOUND_HINT		"Hint.Display"
#define SOUND_USE_DENY	"Player.UseDeny"
#define SOUND_USE		"Player.Use"
#define SOUND_WHOOSH	"Player.FallWoosh2"
#define SMOOTHING_FACTOR 0.9

extern ConVar sv_regeneration_wait_time;
extern ConVar sv_maxspeed;
extern ConVar sv_debug_player_use;
extern ConVar pl_normspeed;

extern float IntervalDistance(float x, float x0, float x1);

static int hasNoclip = 0;
static edict_t* playerEdict;
static bool enableNoclipToggle = false;

void ToggleNoclip() {
	if (hasNoclip == 0) {
		engine->ClientCommand(playerEdict, "noclip 1");
		hasNoclip = 1;
		return;
	}

	engine->ClientCommand(playerEdict, "noclip 0");
	hasNoclip = 0;
}
static ConCommand togNoclip("tog_noclip", ToggleNoclip);

LINK_ENTITY_TO_CLASS(player, CEmulsionPlayer);

IMPLEMENT_SERVERCLASS_ST(CEmulsionPlayer, DT_EmulsionPlayer)
SendPropVector(SENDINFO(m_vecVelocity), -1, SPROP_NOSCALE, 0.0f, HIGH_DEFAULT),
SendPropVector(SENDINFO(m_vecGravity), -1, SPROP_NOSCALE, 0.0f, HIGH_DEFAULT),
SendPropEHandle(SENDINFO(m_hStickParent)),
SendPropInt(SENDINFO(m_nPaintPower))
END_SEND_TABLE()

CEmulsionPlayer::CEmulsionPlayer() {
	pMove = (CEmulsionGameMovement*)g_pGameMovement;
	m_vecGravity = Vector(0, 0, -1);
	m_bIsTouchingStickParent = false;
}

void CEmulsionPlayer::Precache() {
	PrecacheModel(PLAYER_MDL);

	PrecacheScriptSound("Player.EnterBouncePaint");
	PrecacheScriptSound("Player.EnterSpeedPaint");
	PrecacheScriptSound("Player.EnterStickPaint");

	PrecacheScriptSound("Player.ExitBouncePaint");
	PrecacheScriptSound("Player.ExitSpeedPaint");
	PrecacheScriptSound("Player.ExitStickPaint");

	PrecacheScriptSound("Player.JumpPowerUse");
}

void CEmulsionPlayer::Spawn() {
	Precache();
	BaseClass::Spawn();

	// Dying without a player model crashes the client
	SetModel(PLAYER_MDL);
	SetViewOffset(VEC_VIEW);

	// set the move speed
	SetMaxSpeed(pl_normspeed.GetFloat());

	// Temp development stuff
	// TODO: remove after player move fixed
	playerEdict = edict();
	enableNoclipToggle = true;
	//
	engine->ClientCommand(edict(), "bind n tog_noclip");
	engine->ClientCommand(edict(), "give weapon_paintgun");
	engine->ClientCommand(edict(), "bind mwheeldown paintgun_next");
	engine->ClientCommand(edict(), "bind mwheelup paintgun_prev");

	m_hStickParent = NULL;
	m_angInitialAngles = GetAbsAngles();

	if (!engine->HasPaintmap())
		Msg("No Paintmap :(\n");
	else
		Msg("Yes Paintmap :)\n");
}

void CEmulsionPlayer::Activate() {
	BaseClass::Activate();

}

void CEmulsionPlayer::CreateViewModel(int index) {
	Assert(index >= 0 && index < MAX_VIEWMODELS);

	if (GetViewModel(index))
		return;

	CPredictedViewModel* vm = (CPredictedViewModel*)CreateEntityByName("predicted_viewmodel");
	if (vm)
	{
		vm->SetAbsOrigin(GetAbsOrigin());
		vm->SetOwner(this);
		vm->SetIndex(index);
		DispatchSpawn(vm);
		vm->FollowEntity(this, false);
		m_hViewModel.Set(index, vm);
	}
}

void CEmulsionPlayer::Touch(CBaseEntity* pOther)
{
	if (pOther == m_pStickParent) {
		m_bIsTouchingStickParent = true;
		pMove->m_bIsTouchingStickParent = true;
		return;
	}

	BaseClass::Touch(pOther);
}

void CEmulsionPlayer::EndTouch(CBaseEntity* pOther) {
	if (pOther == m_pStickParent) {
		m_bIsTouchingStickParent = false;
		pMove->m_bIsTouchingStickParent = false;
		return;
	}

	BaseClass::EndTouch(pOther);
}

void CEmulsionPlayer::FireBullets(const FireBulletsInfo_t& info) {
	lagcompensation->StartLagCompensation(this, LAG_COMPENSATE_HITBOXES);

	BaseClass::FireBullets(info);

	lagcompensation->FinishLagCompensation(this);
}

// hl2 player pickup code

CBaseEntity* GetPlayerHeldEntity(CBasePlayer* pPlayer)
{
	CBaseEntity* pObject = NULL;
	CPlayerPickupController* pPlayerPickupController = (CPlayerPickupController*)(pPlayer->GetUseEntity());

	if (pPlayerPickupController)
	{
		pObject = pPlayerPickupController->GetGrabController().GetAttached();
	}

	return pObject;
}

void CEmulsionPlayer::PlayerUse() {
	// Was use pressed or released?
	if (!((m_nButtons | m_afButtonPressed | m_afButtonReleased) & IN_USE))
		return;

	if (m_afButtonPressed & IN_USE)
	{
		// Currently using a latched entity?
		if (ClearUseEntity())
		{
			if (m_bPlayerPickedUpObject)
			{
				m_bPlayerPickedUpObject = false;
			}
			return;
		}
		else
		{
			if (m_afPhysicsFlags & PFLAG_DIROVERRIDE)
			{
				m_afPhysicsFlags &= ~PFLAG_DIROVERRIDE;
				m_iTrain = TRAIN_NEW | TRAIN_OFF;
				return;
			}
		}

		// Tracker 3926:  We can't +USE something if we're climbing a ladder
		if (GetMoveType() == MOVETYPE_LADDER)
		{
			return;
		}
	}

	CBaseEntity* pUseEntity = FindUseEntity();

	bool usedSomething = false;

	// Found an object
	if (pUseEntity)
	{
		//!!!UNDONE: traceline here to prevent +USEing buttons through walls			
		int caps = pUseEntity->ObjectCaps();
		variant_t emptyVariant;

		if (m_afButtonPressed & IN_USE)
		{
			// Robin: Don't play sounds for NPCs, because NPCs will allow respond with speech.
			if (!pUseEntity->MyNPCPointer())
			{
				EmitSound(SOUND_USE);
			}
		}

		if (((m_nButtons & IN_USE) && (caps & FCAP_CONTINUOUS_USE)) ||
			((m_afButtonPressed & IN_USE) && (caps & (FCAP_IMPULSE_USE | FCAP_ONOFF_USE))))
		{
			if (caps & FCAP_CONTINUOUS_USE)
				m_afPhysicsFlags |= PFLAG_USING;

			pUseEntity->AcceptInput("Use", this, this, emptyVariant, USE_TOGGLE);

			usedSomething = true;
		}
		// UNDONE: Send different USE codes for ON/OFF.  Cache last ONOFF_USE object to send 'off' if you turn away
		else if ((m_afButtonReleased & IN_USE) && (pUseEntity->ObjectCaps() & FCAP_ONOFF_USE))	// BUGBUG This is an "off" use
		{
			pUseEntity->AcceptInput("Use", this, this, emptyVariant, USE_TOGGLE);

			usedSomething = true;
		}
	}

	else if (m_afButtonPressed & IN_USE)
	{
		// Signal that we want to play the deny sound, unless the user is +USEing on a ladder!
		// The sound is emitted in ItemPostFrame, since that occurs after GameMovement::ProcessMove which
		// lets the ladder code unset this flag.
		m_bPlayUseDenySound = true;
	}

	// Debounce the use key
	if (usedSomething && pUseEntity)
	{
		m_Local.m_nOldButtons |= IN_USE;
		m_afButtonPressed &= ~IN_USE;
	}
}

void CEmulsionPlayer::PickupObject(CBaseEntity* pObject, bool bLimitMassAndSize)
{
	// can't pick up what you're standing on
	if (GetGroundEntity() == pObject)
		return;

	if (bLimitMassAndSize == true)
	{
		if (CBasePlayer::CanPickupObject(pObject, 35, 128) == false)
			return;
	}

	// Can't be picked up if NPCs are on me
	if (pObject->HasNPCsOnIt())
		return;

	PlayerPickupObject(this, pObject);
}

bool CEmulsionPlayer::IsHoldingEntity(CBaseEntity* pEnt)
{
	return PlayerPickupControllerIsHoldingEntity(m_hUseEntity, pEnt);
}

float CEmulsionPlayer::GetHeldObjectMass(IPhysicsObject* pHeldObject)
{
	return  PlayerPickupGetHeldObjectMass(m_hUseEntity, pHeldObject);
}

extern CViewVectors* GetViewVectors();
extern void VecNeg(Vector& v);

void CEmulsionPlayer::SetGravityDir(Vector axis) {
	m_vecGravity = axis;

	Vector m_vecEyeAxisRot = CrossProduct(Vector(0, 0, -1), m_vecGravity.Get());
	float m_flEyeRotation = RAD2DEG(acos(DotProduct(Vector(0, 0, -1), m_vecGravity.Get())));

	MatrixSetIdentity(m_mGravityTransform);
	MatrixBuildRotation(m_mGravityTransform, Vector(0, 0, 1), -1 * m_vecGravity.Get());
}

void CEmulsionPlayer::SetStickParent(CBaseEntity* pParent) {
	m_pStickParent = pParent;
}

CBaseEntity* CEmulsionPlayer::GetStickParent() {
	return m_hStickParent;
}

void CEmulsionPlayer::PreThink() {
	BaseClass::PreThink();

	// do paint stuff here
	ProcessPowerUpdate();
}

static const float g_flStickTransitionTime = 10.0f;
static float g_flCurStickTransitionTime = 0.0f;

void CEmulsionPlayer::ProcessPowerUpdate() {
	PaintInfo_t m_tNewInfo = pMove->CheckPaintedSurface();

	//debugoverlay->AddBoxOverlay(GetAbsOrigin(), GetPlayerMins(), GetPlayerMaxs(), QAngle(0, 0, 1), 0, 255, 0, 150, 0);

	switch (m_tNewInfo.type) {
	case BOUNCE_POWER:
		if (m_tCurPaintInfo.type == SPEED_POWER) {
			BouncePlayer(m_tNewInfo.plane);
			break;
		}
		else if (GetGroundEntity() != NULL) {
			pMove->PlayPaintEntrySound(m_tNewInfo.type);
			m_nPaintPower = BOUNCE_POWER;
		}
		break;
	case SPEED_POWER:
		if (GetGroundEntity() != NULL) {
			pMove->PlayPaintEntrySound(m_tNewInfo.type);
			m_nPaintPower = SPEED_POWER;
		}
		break;
	case PORTAL_POWER:
		if (m_tCurPaintInfo.type != PORTAL_POWER || (m_tNewInfo.plane.normal != m_tCurPaintInfo.plane.normal)) {
			StickPlayer(m_tNewInfo);
			pMove->PlayPaintEntrySound(m_tNewInfo.type);
			m_nPaintPower = PORTAL_POWER;
		}
		break;
	default:
		if (m_tCurPaintInfo.type == PORTAL_POWER) {
			UnStickPlayer();
			m_nPaintPower = NO_POWER;
		}
		pMove->DetermineExitSound(m_tNewInfo.type);
		break;
	}

	m_tCurPaintInfo = m_tNewInfo;
	pMove->m_tCurPaintInfo = m_tNewInfo;
}

extern ConVar pl_bouncePaintWallFactor;
extern ConVar pl_bouncePaintFactor;
extern ConVar pl_showBouncePowerNormal;

extern bool m_bCancelNextExitSound;

void CEmulsionPlayer::BouncePlayer(cplane_t plane) {

	Vector modVel = (m_vecVelocity.Get().Normalized() * (-1 * m_vecGravity)) * (pl_bouncePaintWallFactor.GetFloat());
	modVel = (modVel.x + modVel.y + modVel.z) < 0 ? modVel * -1 : modVel;

	Vector result = (plane.normal * (pl_bouncePaintFactor.GetFloat() * sqrt(m_vecVelocity.Get().Length()))) + modVel;
	SetAbsVelocity(GetAbsVelocity() + result);

	m_bCancelNextExitSound = true;
	pMove->PlaySoundInternal("Player.JumpPowerUse");

	if (pl_showBouncePowerNormal.GetBool())
		Msg("(%f, %f, %f)\n", result.x, result.y, result.z);

	SetGroundEntity(NULL);
}

void CEmulsionPlayer::StickPlayer(PaintInfo_t info) {
	SetGravityDir(-1 * info.plane.normal);
	pMove->SetGravityDir(-1 * info.plane.normal);
	pMove->CalculateStickAngles();

	SetStickParent(info.m_pEnt);
	SetGroundEntity(NULL);

	SetAbsOrigin(info.pos);
	RotateBBox(info.plane.normal);
}

void CEmulsionPlayer::UnStickPlayer() {
	SetGravityDir(Vector(0, 0, -1));
	pMove->SetGravityDir(Vector(0, 0, -1));
	pMove->CalculateStickAngles();

	SetStickParent(NULL);
	SetGroundEntity(NULL);
	m_bIsTouchingStickParent = false;
	pMove->m_bIsTouchingStickParent = false;

	if (m_tCurPaintInfo.plane.normal != Vector(0, 0, 1))
		SetAbsOrigin(GetAbsOrigin() + (GetViewOffset().Length() * (m_tCurPaintInfo.plane.normal)) * 1.2f);

	RotateBBox(Vector(0, 0, 1));
}

static CViewVectors g_OriginalDefaultViewVectors(
	Vector(0, 0, 64),			//VEC_VIEW (m_vView)

	Vector(-16, -16, 0),		//VEC_HULL_MIN (m_vHullMin)
	Vector(16, 16, 72),		//VEC_HULL_MAX (m_vHullMax)

	Vector(-16, -16, 0),		//VEC_DUCK_HULL_MIN (m_vDuckHullMin)
	Vector(16, 16, 36),		//VEC_DUCK_HULL_MAX	(m_vDuckHullMax)
	Vector(0, 0, 28),			//VEC_DUCK_VIEW		(m_vDuckView)

	Vector(-10, -10, -10),		//VEC_OBS_HULL_MIN	(m_vObsHullMin)
	Vector(10, 10, 10),		//VEC_OBS_HULL_MAX	(m_vObsHullMax)

	Vector(0, 0, 14)			//VEC_DEAD_VIEWHEIGHT (m_vDeadViewHeight)
);

CViewVectors* GetOriginalViewVectors()
{
	return &g_OriginalDefaultViewVectors;
}

void CEmulsionPlayer::RotateBBox(Vector vecUp) {

	// rotate the bbox around the player origin,
	// to the new up position from the plane normal

	// build rotation matrix
	VMatrix upMat;
	Vector vecAxisOfRotation = CrossProduct(Vector(0, 0, 1), vecUp);
	float flAngleOfRotation = RAD2DEG(acos(DotProduct(Vector(0, 0, 1), vecUp)));

	MatrixBuildRotationAboutAxis(upMat, vecAxisOfRotation, flAngleOfRotation);

	// rotate the tall bbox
	RotateAABB(upMat.As3x4(), GetOriginalViewVectors()->m_vHullMin, GetOriginalViewVectors()->m_vHullMax,
		GetViewVectors()->m_vHullMin, GetViewVectors()->m_vHullMax);

	// rotate the small bbox (because i can)
	RotateAABB(upMat.As3x4(), GetOriginalViewVectors()->m_vDuckHullMin, GetOriginalViewVectors()->m_vDuckHullMax,
		GetViewVectors()->m_vDuckHullMin, GetViewVectors()->m_vDuckHullMax);
}

Vector CEmulsionPlayer::GetHalfHeight_Stick() {
	return GetAbsOrigin() + ((-1 * m_vecGravity) * (GetViewOffset().Length() * 0.75f));
}

Vector CEmulsionPlayer::GetForward_Stick() {
	return pMove->m_vecStickForward;
}