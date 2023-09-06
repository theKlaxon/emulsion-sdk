#include "cbase.h"
#include "emulsion_player.h"
#include "player_pickup.h"
#include "in_buttons.h"
#include "emulsion_pickupcontroller.h"
#include "ai_basenpc.h"
#include "..\server\ilagcompensationmanager.h"
#include "shareddefs.h"
#include "..\game\shared\portal2\paint_enum.h"

#define PLAYER_MDL "models/player.mdl"
#define PLAYER_MAX_LIFT_MASS 85
#define PLAYER_MAX_LIFT_SIZE 128

#define SOUND_HINT		"Hint.Display"
#define SOUND_USE_DENY	"Player.UseDeny"
#define SOUND_USE		"Player.Use"
#define SOUND_WHOOSH	"Player.FallWoosh2"

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

LINK_ENTITY_TO_CLASS( player, CEmulsionPlayer);

IMPLEMENT_SERVERCLASS_ST(CEmulsionPlayer, DT_EmulsionPlayer)
	SendPropVector(SENDINFO(m_vecVelocity), -1, SPROP_NOSCALE, 0.0f, HIGH_DEFAULT),
	SendPropInt(SENDINFO(m_iPaintPower))
END_SEND_TABLE()

CEmulsionPlayer::CEmulsionPlayer() {
	m_vecGravity = Vector(0, 0, -1);
	m_vecPrevGravity = Vector(0, 0, -1);
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

	engine->ClientCommand(edict(), "bind l create_flashlight");
	engine->ClientCommand(edict(), "bind n tog_noclip");
	engine->ClientCommand(edict(), "give weapon_paintgun");
	engine->ClientCommand(edict(), "bind 0 exit2");
	engine->ClientCommand(edict(), "bind 8 disconnect");
	engine->ClientCommand(edict(), "bind mwheeldown paintgun_next");
	engine->ClientCommand(edict(), "bind mwheelup paintgun_prev");
	engine->ClientCommand(edict(), "bind b create_blob");

	m_pStickParent = NULL;
	m_angInitialAngles = GetAbsAngles();

	if (!engine->HasPaintmap())
		Msg("No Paintmap :(\n");
	else
		Msg("Yes Paintmap :)\n");
}

void CEmulsionPlayer::Activate() {
	BaseClass::Activate();
}

void CEmulsionPlayer::CreateViewModel( int index ) {
	Assert( index >= 0 && index < MAX_VIEWMODELS );

	if ( GetViewModel(index) )
		return;

	CPredictedViewModel* vm = (CPredictedViewModel*)CreateEntityByName( "predicted_viewmodel" );
	if ( vm )
	{
		vm->SetAbsOrigin( GetAbsOrigin() );
		vm->SetOwner( this );
		vm->SetIndex( index );
		DispatchSpawn( vm );
		vm->FollowEntity( this, false );
		m_hViewModel.Set( index, vm );
	}
}

void CEmulsionPlayer::PostThink() {
	BaseClass::PostThink();

	// Keep the model upright; pose params will handle pitch aiming.
	QAngle angles = GetLocalAngles();
	angles[PITCH] = 0;
	SetLocalAngles(angles);

	NetworkStateChanged();
}

void CEmulsionPlayer::FireBullets ( const FireBulletsInfo_t &info ) {
	lagcompensation->StartLagCompensation( this, LAG_COMPENSATE_HITBOXES);

	BaseClass::FireBullets(info);

	lagcompensation->FinishLagCompensation( this );
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
	m_vecPrevGravity = m_vecGravity;
	m_vecGravity = axis;

	Vector m_vecEyeAxisRot = CrossProduct(m_vecPrevGravity, m_vecGravity);
	float m_flEyeRotation = RAD2DEG(acos(DotProduct(m_vecPrevGravity, m_vecGravity)));

	MatrixSetIdentity(m_mGravityTransform);
	MatrixBuildRotation(m_mGravityTransform, Vector(0, 0, 1), -1 * m_vecGravity);

	//QAngle angAngles = m_angInitialAngles;
	//MatrixAngles(m_mGravityTransform.As3x4(), angAngles);
	//SetAbsAngles(angAngles);

	//Msg("initial angles: %f, %f, %f\n", m_angInitialAngles.x, m_angInitialAngles.y, m_angInitialAngles.z);
	//Msg("new angles: %f, %f, %f\n", angAngles.x, angAngles.y, angAngles.z);

	//RotBoundingBox(m_mGravityTransform);
}

void CEmulsionPlayer::RotBoundingBox(VMatrix mat) {
	RotateAABB(mat.As3x4(),
		GetViewVectors()->m_vDuckHullMin, GetViewVectors()->m_vDuckHullMax,
		GetViewVectors()->m_vDuckHullMin, GetViewVectors()->m_vDuckHullMax);
	RotateAABB(mat.As3x4(),
		GetViewVectors()->m_vHullMin, GetViewVectors()->m_vHullMax,
		GetViewVectors()->m_vHullMin, GetViewVectors()->m_vHullMax);
}

void CEmulsionPlayer::SetStickParent(CBaseEntity* pParent) {
	m_pStickParent = pParent;
}

CBaseEntity* CEmulsionPlayer::GetStickParent() {
	return m_pStickParent;
}
