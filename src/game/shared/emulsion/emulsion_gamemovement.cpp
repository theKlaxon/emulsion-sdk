#include "cbase.h"
#include "gamemovement.h"
#include "vphysics_interface.h"
#include "in_buttons.h"
#include "coordsize.h"
#include "SoundEmitterSystem/isoundemittersystembase.h"
#include "game/shared/portal2/paint_enum.h"
#include "emulsion_shareddefs.h"
#include "emulsion_gamemovement.h"

#ifndef CLIENT_DLL
#include "recipientfilter.h"
#include "env_player_surface_trigger.h"
#include "emulsion_player.h"
#else
#include "c_emulsion_player.h"
#define CRecipientFilter C_RecipientFilter
#endif

#define	MAX_CLIP_PLANES	5
#define CheckV( tick, ctx, vel )

extern void TracePlayerBBoxForGround(ITraceListData* pTraceListData, const Vector& start, const Vector& end, const Vector& minsSrc,
	const Vector& maxsSrc, unsigned int fMask,
	ITraceFilter* filter, trace_t& pm, float minGroundNormalZ, bool overwriteEndpos, int* pCounter);

static const float m_fPaintFrameDelay = 0.001f;
bool m_bCancelNextExitSound = false;

extern bool g_bMovementOptimizations;
extern ConVar sv_gravity;
extern ConVar sv_accelerate;
extern ConVar sv_optimizedmovement;
extern ConVar sv_noclipspeed;
extern ConVar sv_noclipaccelerate;
extern ConVar sv_airaccelerate;
extern ConVar sv_friction;
extern ConVar sv_stopspeed;
extern ConVar sv_maxspeed;
extern ConVar sv_rollangle;
extern ConVar sv_rollspeed;
extern ConVar sv_bounce;

// replacing the surfaeprop thingy for a more uniform movement
ConVar sv_jumpFactor("sv_jumpfactor", "1.0f", FCVAR_REPLICATED | FCVAR_CHEAT);
ConVar pl_fallpunchthreshold("pl_fallpunchthreshold", "150", FCVAR_REPLICATED | FCVAR_CHEAT | FCVAR_NOTIFY);

// paint
ConVar pl_normspeed("pl_normspeed", "175.0f", FCVAR_REPLICATED | FCVAR_NOTIFY | FCVAR_CHEAT);
ConVar pl_paintTraceRadius("pl_paintTraceRadius", "26.0f", FCVAR_REPLICATED | FCVAR_CHEAT | FCVAR_NOTIFY); // 26.0f worked good for a while // 20.0f while making stick

ConVar pl_bouncePaintFactor("pl_bouncePaintFactor", "265.0f", FCVAR_REPLICATED | FCVAR_CHEAT | FCVAR_NOTIFY);
ConVar pl_bouncePaintWallFactor("pl_bouncePaintWallFactor", "150.0f", FCVAR_REPLICATED | FCVAR_CHEAT | FCVAR_NOTIFY);

ConVar pl_speedPaintMoveSpeed("pl_speedPaintMoveSpeed", "600", FCVAR_REPLICATED | FCVAR_CHEAT | FCVAR_NOTIFY);
ConVar pl_speedPaintAcceleration("pl_speedPaintAccelerate", "5", FCVAR_REPLICATED | FCVAR_CHEAT | FCVAR_NOTIFY);
ConVar pl_speedPaintFrictionDivisor("pl_speedPaintFrictionDivisor", "0.85f", FCVAR_REPLICATED | FCVAR_CHEAT | FCVAR_NOTIFY); // should be denominator
ConVar pl_stickPaintMoveSpeed("pl_stickpaintmovespeed", "150", FCVAR_REPLICATED | FCVAR_CHEAT | FCVAR_NOTIFY);

// paint debugging
ConVar pl_showPaintPower("pl_showpaintpower", "0", FCVAR_REPLICATED | FCVAR_DEVELOPMENTONLY);
ConVar pl_showBouncePowerNormal("pl_showbouncepowernormal", "1", FCVAR_REPLICATED | FCVAR_DEVELOPMENTONLY);
ConVar pl_showStickPowerNormal("pl_showstickpowernormal", "0", FCVAR_REPLICATED | FCVAR_DEVELOPMENTONLY);

static CEmulsionGameMovement g_GameMovement;
IGameMovement* g_pGameMovement = (IGameMovement*)&g_GameMovement;

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CEmulsionGameMovement, IGameMovement, INTERFACENAME_GAMEMOVEMENT, g_GameMovement);

CEmulsionGameMovement::CEmulsionGameMovement() {

	m_qRotFrom = Quaternion();
	m_qRotTo = Quaternion();

	m_vecGravity = Vector(0, 0, -1);
	m_vecPrevGravity = m_vecGravity;

	MatrixSetIdentity(m_mGravityTransform);

	m_bIsTouchingStickParent = false;

#ifdef GAME_DLL 
	m_fCurPaintFrameDelay = m_fPaintFrameDelay;
#endif
}

void VecNeg(Vector& v) {
	for (int i = 0; i < 3; i++)
		v[i] = v[i] == 0.0f ? 1 : v[i] - v[i];
}

void CEmulsionGameMovement::ProcessMovement(CBasePlayer* pPlayer, CMoveData* pMove) {

	//#ifdef GAME_DLL
	//	if (m_fCurPaintFrameDelay <= 0.0f) {
	//		ProcessPowerUpdate();
	//
	//		if (pl_showPaintPower.GetBool())
	//			Msg("PaintPower: %i\n", (int)m_tCurPaintInfo.type);
	//
	//		m_fCurPaintFrameDelay = m_fPaintFrameDelay;
	//	}
	//	else
	//		m_fCurPaintFrameDelay -= gpGlobals->frametime;
	//#endif

	m_nTraceCount = 0;

	Assert(pMove && pPlayer);

	float flStoreFrametime = gpGlobals->frametime;

	//!!HACK HACK: Adrian - slow down all player movement by this factor.
	//!!Blame Yahn for this one.
	gpGlobals->frametime *= pPlayer->GetLaggedMovementValue();

	ResetGetWaterContentsForPointCache();

	// Cropping movement speed scales mv->m_fForwardSpeed etc. globally
	// Once we crop, we don't want to recursively crop again, so we set the crop
	//  flag globally here once per usercmd cycle.
	m_bSpeedCropped = false;

	// StartTrackPredictionErrors should have set this
	Assert(pPlayer->IsBot() || player == pPlayer);
	player = pPlayer;

	mv = pMove;
	//if (m_tCurPaintInfo.type == SPEED_POWER)
	//	mv->m_flMaxSpeed = pl_speedPaintMoveSpeed.GetFloat();
	//else
	//	mv->m_flMaxSpeed = sv_maxspeed.GetFloat();

	switch (m_tCurPaintInfo.type) {
	case SPEED_POWER:
		mv->m_flMaxSpeed = pl_speedPaintMoveSpeed.GetFloat();
		break;
	case PORTAL_POWER:
		mv->m_flMaxSpeed = pl_stickPaintMoveSpeed.GetFloat();
		break;
	default:
		mv->m_flMaxSpeed = sv_maxspeed.GetFloat();
	}

	m_bProcessingMovement = true;
	m_bInStuckTest = false;

	DiffPrint("start %f %f %f", mv->GetAbsOrigin().x, mv->GetAbsOrigin().y, mv->GetAbsOrigin().z);

	// Run the command.
	PlayerMove();
	FinishMove();

	DiffPrint("end %f %f %f", mv->GetAbsOrigin().x, mv->GetAbsOrigin().y, mv->GetAbsOrigin().z);

	//This is probably not needed, but just in case.
	gpGlobals->frametime = flStoreFrametime;

	m_bProcessingMovement = false;

#if !defined( CLIENT_DLL )
	if (!player->IsBot())
	{
		VPROF_INCREMENT_COUNTER("PlayerMovementTraces", m_nTraceCount);
	}
#endif
}

void CEmulsionGameMovement::PlayerMove()
{
	VPROF("CGameMovement::PlayerMove");
	CheckParameters();

	// clear output applied velocity
	mv->m_outWishVel.Init();
	mv->m_outJumpVel.Init();

	MoveHelper()->ResetTouchList();                    // Assume we don't touch anything

	ReduceTimers();

#ifdef INFESTED_DLL		// ignore roll component for Alien Swarm, this is used for vertical aiming
	QAngle vecViewAngles = mv->m_vecViewAngles;
	vecViewAngles[ROLL] = 0;
	AngleVectors(vecViewAngles, &m_vecForward, &m_vecRight, &m_vecUp);  // Determine movement angles
#else
	AngleVectors(mv->m_vecViewAngles, &m_vecForward, &m_vecRight, &m_vecUp);  // Determine movement angles
#endif

	// Always try and unstick us unless we are using a couple of the movement modes
	MoveType_t moveType = player->GetMoveType();
	if (moveType != MOVETYPE_NOCLIP &&
		moveType != MOVETYPE_NONE &&
		moveType != MOVETYPE_ISOMETRIC &&
		moveType != MOVETYPE_OBSERVER &&
		!player->pl.deadflag)
	{
		if (CheckInterval(STUCK))
		{
			if (CheckStuck())
			{
				// Can't move, we're stuck
				return;
			}
		}
	}

	// Now that we are "unstuck", see where we are (player->GetWaterLevel() and type, player->GetGroundEntity()).
	if ((player->GetMoveType() != MOVETYPE_WALK && player->GetMoveType() != MOVETYPE_STICK) ||
		mv->m_bGameCodeMovedPlayer ||
		!sv_optimizedmovement.GetBool())
	{
		CategorizePosition();
	}
	else
	{
		//if (mv->m_vecVelocity.z > 250.0f)
		//{
		//	SetGroundEntity(NULL);
		//}
		if ((mv->m_vecVelocity * GetGravityDir()).Length() > 250.0f)
			SetGroundEntity(NULL);
	}

	// Store off the starting water level
	m_nOldWaterLevel = player->GetWaterLevel();

	// If we are not on ground, store off how fast we are moving down
	if (player->GetGroundEntity() == NULL)
	{
		//player->m_Local.m_flFallVelocity = -mv->m_vecVelocity[2];
		vec_t l = (mv->m_vecVelocity * GetGravityDir()).Length();
		player->m_Local.m_flFallVelocity = l;
	}

	m_nOnLadder = 0;

#ifdef GAME_DLL
	if (m_tCurPaintInfo.m_pEnt == player->GetGroundEntity() && m_tCurPaintInfo.m_pEnt != NULL)
		if (m_tCurPaintInfo.m_bIsPainted) {
			player->m_surfaceProps = physprops->GetSurfaceIndex("painted_surface");
			player->m_pSurfaceData = physprops->GetSurfaceData(player->m_surfaceProps);
			player->UpdateStepSound(player->m_pSurfaceData, mv->GetAbsOrigin(), mv->m_vecVelocity);
			goto C;
		}
B:
	player->UpdateStepSound(player->m_pSurfaceData, mv->GetAbsOrigin(), mv->m_vecVelocity);
C:
#endif

	UpdateDuckJumpEyeOffset();
	Duck();

	ProcessPowerUpdate();

	// Handle movement modes.
	switch (player->GetMoveType())
	{
	case MOVETYPE_NONE:
		break;

	case MOVETYPE_NOCLIP:
		FullNoClipMove(sv_noclipspeed.GetFloat(), sv_noclipaccelerate.GetFloat());
		break;

	case MOVETYPE_FLY:
	case MOVETYPE_FLYGRAVITY:
		FullTossMove();
		break;

	case MOVETYPE_LADDER:
		FullLadderMove();
		break;

	case MOVETYPE_WALK:
		FullWalkMove();
		break;

	case MOVETYPE_ISOMETRIC:
		//IsometricMove();
		// Could also try:  FullTossMove();
		FullWalkMove();
		break;

	case MOVETYPE_STICK:
		//FullStickMove();
		FullWalkMove();
		break;

	case MOVETYPE_OBSERVER:
		FullObserverMove(); // clips against world&players
		break;

	default:
		DevMsg(1, "Bogus pmove player movetype %i on (%i) 0=cl 1=sv\n", player->GetMoveType(), player->IsServer());
		break;
	}
}

void CEmulsionGameMovement::CategorizePosition(void)
{

	Vector point;
	trace_t pm;

	// Reset this each time we-recategorize, otherwise we have bogus friction when we jump into water and plunge downward really quickly
	player->m_surfaceFriction = 1.0f;

	// if the player hull point one unit down is solid, the player
	// is on ground

	// see if standing on something solid	

	// Doing this before we move may introduce a potential latency in water detection, but
	// doing it after can get us stuck on the bottom in water if the amount we move up
	// is less than the 1 pixel 'threshold' we're about to snap to.	Also, we'll call
	// this several times per frame, so we really need to avoid sticking to the bottom of
	// water on each call, and the converse case will correct itself if called twice.
	//CheckWater();

	// observers don't have a ground entity
	if (player->IsObserver())
		return;

	float flOffset = 2.0f;

	//point[0] = mv->GetAbsOrigin()[0];
	//point[1] = mv->GetAbsOrigin()[1];
	//point[2] = mv->GetAbsOrigin()[2] - flOffset;
	point = mv->GetAbsOrigin() + ((-1 * GetGravityDir()) * -flOffset);

	//point = mv->GetAbsOrigin();
	//point += GetGravityDir() * -flOffset;

	Vector bumpOrigin;
	bumpOrigin = mv->GetAbsOrigin();

	// Shooting up really fast.  Definitely not on ground.
	// On ladder moving up, so not on ground either
	// NOTE: 145 is a jump.
#define NON_JUMP_VELOCITY 140.0f

	//float zvel = mv->m_vecVelocity[2];
	float zvel = (mv->m_vecVelocity * (-1 * GetGravityDir())).Length();
	bool bMovingUp = zvel > 0.0f;
	bool bMovingUpRapidly = zvel > NON_JUMP_VELOCITY;
	float flGroundEntityVelZ = 0.0f;
	//if (bMovingUpRapidly)
	//{
	//	// Tracker 73219, 75878:  ywb 8/2/07
	//	// After save/restore (and maybe at other times), we can get a case where we were saved on a lift and 
	//	//  after restore we'll have a high local velocity due to the lift making our abs velocity appear high.  
	//	// We need to account for standing on a moving ground object in that case in order to determine if we really 
	//	//  are moving away from the object we are standing on at too rapid a speed.  Note that CheckJump already sets
	//	//  ground entity to NULL, so this wouldn't have any effect unless we are moving up rapidly not from the jump button.
	//	CBaseEntity* ground = player->GetGroundEntity();
	//	if (ground)
	//	{
	//		//flGroundEntityVelZ = ground->GetAbsVelocity().z;
	//		flGroundEntityVelZ = (ground->GetAbsVelocity() * GetGravityDir()).Length();
	//		bMovingUpRapidly = (zvel - flGroundEntityVelZ) > NON_JUMP_VELOCITY;
	//	}
	//}

	// NOTE YWB 7/5/07:  Since we're already doing a traceline here, we'll subsume the StayOnGround (stair debouncing) check into the main traceline we do here to see what we're standing on
	bool bUnderwater = (player->GetWaterLevel() >= WL_Eyes);
	bool bMoveToEndPos = false;

	if ((player->GetMoveType() == MOVETYPE_WALK || player->GetMoveType() == MOVETYPE_STICK) &&
		(player->GetGroundEntity() != NULL && !bUnderwater))
	{
		// if walking and still think we're on ground, we'll extend trace down by stepsize so we don't bounce down slopes
		bMoveToEndPos = true;
		//point.z -= player->m_Local.m_flStepSize;
		point += GetGravityDir() * player->m_Local.m_flStepSize;
	}

	// Was on ground, but now suddenly am not
	if (bMovingUpRapidly ||
		(bMovingUp && player->GetMoveType() == MOVETYPE_LADDER))
	{
		SetGroundEntity(NULL);
		bMoveToEndPos = false;
	}
	else
	{
		// Try and move down.
		TracePlayerBBox(bumpOrigin, point, PlayerSolidMask(), COLLISION_GROUP_PLAYER_MOVEMENT, pm);

		// Was on ground, but now suddenly am not.  If we hit a steep plane, we are not on ground
		float flStandableZ = 0.7;

		float slope = (pm.plane.normal * (-1 * GetGravityDir())).Length();
		if (!pm.m_pEnt || (slope < flStandableZ))
		{
			// Test four sub-boxes, to see if any of them would have found shallower slope we could actually stand on
			ITraceFilter* pFilter = LockTraceFilter(COLLISION_GROUP_PLAYER_MOVEMENT);
			TracePlayerBBoxForGround(m_pTraceListData, bumpOrigin, point, GetPlayerMins(),
				GetPlayerMaxs(), PlayerSolidMask(), pFilter, pm, flStandableZ, true, &m_nTraceCount);
			UnlockTraceFilter(pFilter);
			//if (!pm.m_pEnt || (pm.plane.normal[2] < flStandableZ))
			if (!pm.m_pEnt || (slope < flStandableZ))
			{
				SetGroundEntity(NULL);
				// probably want to add a check for a +z velocity too!
				//if ((mv->m_vecVelocity.z > 0.0f) &&
				if (((mv->m_vecVelocity * (-1 * GetGravityDir())).Length() > 0.0f) &&
					(player->GetMoveType() != MOVETYPE_NOCLIP))
				{
					player->m_surfaceFriction = 0.25f;
				}
				bMoveToEndPos = false;
			}
			else
			{
				SetGroundEntity(&pm);
			}
		}
		else
		{
			SetGroundEntity(&pm);  // Otherwise, point to index of ent under us.
		}

#ifndef CLIENT_DLL

		//Adrian: vehicle code handles for us.
		if (player->IsInAVehicle() == false)
		{
			// If our gamematerial has changed, tell any player surface triggers that are watching
			IPhysicsSurfaceProps* physprops = MoveHelper()->GetSurfaceProps();
			surfacedata_t* pSurfaceProp = physprops->GetSurfaceData(pm.surface.surfaceProps);
			char cCurrGameMaterial = pSurfaceProp->game.material;
			if (!player->GetGroundEntity())
			{
				cCurrGameMaterial = 0;
			}

			// Changed?
			if (player->m_chPreviousTextureType != cCurrGameMaterial)
			{
				CEnvPlayerSurfaceTrigger::SetPlayerSurface(player, cCurrGameMaterial);
			}

			player->m_chPreviousTextureType = cCurrGameMaterial;
		}
#endif
	}

	// YWB:  This logic block essentially lifted from StayOnGround implementation
	if (bMoveToEndPos &&
		!pm.startsolid &&				// not sure we need this check as fraction would == 0.0f?
		pm.fraction > 0.0f &&			// must go somewhere
		pm.fraction < 1.0f) 			// must hit something
	{
		mv->SetAbsOrigin(pm.endpos);
	}
}

void CEmulsionGameMovement::FullWalkMove()
{

	if (m_tCurPaintInfo.type == PORTAL_POWER)
		CalculateStickAngles();

	Vector vecInvGravity = m_vecGravity;
	VecNeg(vecInvGravity);

	if (!CheckWater())
	{
		StartGravity();
	}

	// If we are leaping out of the water, just update the counters.
	if (player->m_flWaterJumpTime)
	{
		WaterJump();
		TryPlayerMove();
		// See if we are still in water?
		CheckWater();
		return;
	}

	// If we are swimming in the water, see if we are nudging against a place we can jump up out
	//  of, and, if so, start out jump.  Otherwise, if we are not moving up, then reset jump timer to 0
	if (player->GetWaterLevel() >= WL_Waist)
	{
		if (player->GetWaterLevel() == WL_Waist)
		{
			CheckWaterJump();
		}

		// If we are falling again, then we must not trying to jump out of water any more.
		if (mv->m_vecVelocity[2] < 0 &&
			player->m_flWaterJumpTime)
		{
			player->m_flWaterJumpTime = 0;
		}

		// Was jump button pressed?
		if (mv->m_nButtons & IN_JUMP)
		{
			CheckJumpButton();
		}
		else
		{
			mv->m_nOldButtons &= ~IN_JUMP;
		}

		// Perform regular water movement
		WaterMove();

		// Redetermine position vars
		CategorizePosition();

		// If we are on ground, no downward velocity.
		if (player->GetGroundEntity() != NULL)
		{
			mv->m_vecVelocity[2] = 0;
		}
	}
	else
		// Not fully underwater
	{
		// Was jump button pressed?
		if (mv->m_nButtons & IN_JUMP)
		{
			CheckJumpButton();
		}
		else
		{
			mv->m_nOldButtons &= ~IN_JUMP;
		}

		// Fricion is handled before we add in any base velocity. That way, if we are on a conveyor, 
		//  we don't slow when standing still, relative to the conveyor.
		if (player->GetGroundEntity() != NULL || m_bIsTouchingStickParent)
		{
			//mv->m_vecVelocity[2] = 0.0;
			//mv->m_vecVelocity *= vecInvGravity;

			//player->m_Local.m_flFallVelocity = 0.0f;
			Friction();
		}

		// Make sure velocity is valid.
		CheckVelocity();

		if (player->GetGroundEntity() != NULL || (m_tCurPaintInfo.type == PORTAL_POWER && m_bIsTouchingStickParent))
		{
			WalkMove();
		}
		else
		{
			AirMove();  // Take into account movement when in air.
		}

		// Set final flags.
		CategorizePosition();

		// Make sure velocity is valid.
		CheckVelocity();

		// Add any remaining gravitational component.
		if (!CheckWater())
		{
			FinishGravity();
		}

		// If we are on ground, no downward velocity.
		if (player->GetGroundEntity() != NULL || (m_tCurPaintInfo.type == PORTAL_POWER && m_bIsTouchingStickParent))
		{
			//mv->m_vecVelocity[2] = 0; // TODO: adapt this for stick paint axis (invert it, ex: V(0, 0, 1) -> V(1, 1, 0) * m_vecVel to keep our velocity)
			mv->m_vecVelocity *= vecInvGravity;
		}
		CheckFalling();
	}

	if ((m_nOldWaterLevel == WL_NotInWater && player->GetWaterLevel() != WL_NotInWater) ||
		(m_nOldWaterLevel != WL_NotInWater && player->GetWaterLevel() == WL_NotInWater))
	{
		PlaySwimSound();
#if !defined( CLIENT_DLL )
		player->Splash();
#endif
	}
}

void CEmulsionGameMovement::WalkMove()
{
	int i;

	Vector wishvel;
	float spd;
	float fmove, smove;
	Vector wishdir;
	float wishspeed;

	// setting vars early here to reduce if() statements later on
	float accel = sv_accelerate.GetFloat();
	float maxSpeed = mv->m_flMaxSpeed;

	Vector vecInvGravity = m_vecGravity;
	Vector dest;
	trace_t pm;
	Vector forward, right, up;

	VecNeg(vecInvGravity);

	// TODO: adapt this to use an override with stick paint power
	//AngleVectors(mv->m_vecViewAngles, &forward, &right, &up);  // Determine movement angles
	if (m_tCurPaintInfo.type == PORTAL_POWER)
		AngleVectors(m_angStickAngles, &forward, &right, &up);  // Determine movement angles
	else
		AngleVectors(mv->m_vecViewAngles, &forward, &right, &up);  // Determine movement angles

	CHandle< CBaseEntity > oldground;
	oldground = player->GetGroundEntity();

	// Copy movement amounts
	fmove = mv->m_flForwardMove;
	smove = mv->m_flSideMove;

	// adjust vars for speed paint, uses 1 if() statement here instead of a bunch later
	// TODO: account for stick paint vars here when it's added
	if (m_tCurPaintInfo.type == SPEED_POWER) {
		accel = pl_speedPaintAcceleration.GetFloat();
		maxSpeed = pl_speedPaintMoveSpeed.GetFloat(); // this happens twice to ensure the proper speed (no bug)
	}

	// Zero out z components of movement vectors // TODO: adapt for stick paint
	{
		forward *= vecInvGravity;
		right *= vecInvGravity;

		VectorNormalize(forward);  // Normalize remainder of vectors.
		VectorNormalize(right);    // 
	}

	wishvel = (forward * fmove) + (right * smove);

	//wishvel[2] = 0;             // Zero out z part of velocity // TODO: adapt for stick paint
	wishvel *= vecInvGravity;     // Zero out z part of velocity // TODO: adapt for stick paint

	VectorCopy(wishvel, wishdir);   // Determine maginitude of speed of move
	wishspeed = VectorNormalize(wishdir);

	//
	// Clamp to server defined max speed
	//
	if ((wishspeed != 0.0f) && (wishspeed > maxSpeed))
	{
		VectorScale(wishvel, maxSpeed / wishspeed, wishvel);
		wishspeed = maxSpeed;
	}

	Accelerate(wishdir, wishspeed, accel);

	// Add in any base velocity to the current velocity.
	VectorAdd(mv->m_vecVelocity, player->GetBaseVelocity(), mv->m_vecVelocity);

	spd = VectorLength(mv->m_vecVelocity);

	if (spd < 1.0f)
	{
		mv->m_vecVelocity.Init();
		// Now pull the base velocity back out.   Base velocity is set if you are on a moving object, like a conveyor (or maybe another monster?)
		VectorSubtract(mv->m_vecVelocity, player->GetBaseVelocity(), mv->m_vecVelocity);
		return;
	}

	// first try just moving to the destination	
	dest = mv->GetAbsOrigin() + ((vecInvGravity * mv->m_vecVelocity) * gpGlobals->frametime);

	// first try moving directly to the next spot
	TracePlayerBBox(mv->GetAbsOrigin(), dest, PlayerSolidMask(), COLLISION_GROUP_PLAYER_MOVEMENT, pm);

	// If we made it all the way, then copy trace end as new player position.
	mv->m_outWishVel += wishdir * wishspeed;

	if (pm.fraction == 1)
	{
		mv->SetAbsOrigin(pm.endpos);
		// Now pull the base velocity back out.   Base velocity is set if you are on a moving object, like a conveyor (or maybe another monster?)
		VectorSubtract(mv->m_vecVelocity, player->GetBaseVelocity(), mv->m_vecVelocity);

		StayOnGround();
		return;
	}

	// Don't walk up stairs if not on ground.
	if (oldground == NULL && player->GetWaterLevel() == 0)
	{
		// Now pull the base velocity back out.   Base velocity is set if you are on a moving object, like a conveyor (or maybe another monster?)
		VectorSubtract(mv->m_vecVelocity, player->GetBaseVelocity(), mv->m_vecVelocity);
		return;
	}

	// If we are jumping out of water, don't do anything more.
	if (player->m_flWaterJumpTime)
	{
		// Now pull the base velocity back out.   Base velocity is set if you are on a moving object, like a conveyor (or maybe another monster?)
		VectorSubtract(mv->m_vecVelocity, player->GetBaseVelocity(), mv->m_vecVelocity);
		return;
	}

	StepMove(dest, pm);

	// Now pull the base velocity back out.   Base velocity is set if you are on a moving object, like a conveyor (or maybe another monster?)
	VectorSubtract(mv->m_vecVelocity, player->GetBaseVelocity(), mv->m_vecVelocity);

	StayOnGround();
}

extern float g_flCurStickTransitionTime;

void CEmulsionGameMovement::AirMove()
{

	int			i;
	Vector		wishvel;
	float		fmove, smove;
	Vector		wishdir;
	float		wishspeed;
	Vector forward, right, up;
	Vector nAxis = GetGravityDir();

	VecNeg(nAxis);
	AngleVectors(mv->m_vecViewAngles, &forward, &right, &up);  // Determine movement angles

	// Copy movement amounts
	fmove = mv->m_flForwardMove;
	smove = mv->m_flSideMove;

	// Zero out gravity axis components of movement vectors
	forward *= nAxis;
	right *= nAxis;

	VectorNormalize(forward);  // Normalize remainder of vectors
	VectorNormalize(right);    // 

	for (i = 0; i < 2; i++)       // Determine x and y parts of velocity
		wishvel[i] = forward[i] * fmove + right[i] * smove;
	//wishvel[2] = 0;             // Zero out z part of velocity

	wishvel *= nAxis;

	VectorCopy(wishvel, wishdir);   // Determine maginitude of speed of move
	wishspeed = VectorNormalize(wishdir);

	//
	// clamp to server defined max speed
	//
	if (wishspeed != 0 && (wishspeed > mv->m_flMaxSpeed))
	{
		VectorScale(wishvel, mv->m_flMaxSpeed / wishspeed, wishvel);
		wishspeed = mv->m_flMaxSpeed;
	}

	AirAccelerate(wishdir, wishspeed, sv_airaccelerate.GetFloat());

	// Add in any base velocity to the current velocity.
	VectorAdd(mv->m_vecVelocity, player->GetBaseVelocity(), mv->m_vecVelocity);

	TryPlayerMove();

	// Now pull the base velocity back out.   Base velocity is set if you are on a moving object, like a conveyor (or maybe another monster?)
	VectorSubtract(mv->m_vecVelocity, player->GetBaseVelocity(), mv->m_vecVelocity);
}

void CEmulsionGameMovement::Friction()
{
	float	speed, newspeed, control;
	float	friction;
	float	drop;

	// If we are in water jump cycle, don't apply friction
	if (player->m_flWaterJumpTime)
		return;

	// Calculate speed
	speed = VectorLength(mv->m_vecVelocity);

	// If too slow, return
	if (speed < 0.1f)
	{
		return;
	}

	drop = 0;

	// apply ground friction
	if (m_tCurPaintInfo.type == PORTAL_POWER && m_bIsTouchingStickParent) {
		friction = sv_friction.GetFloat() * 1.2f;// player->m_surfaceFriction;
		control = (speed < sv_stopspeed.GetFloat()) ? sv_stopspeed.GetFloat() : speed;

		// Add the amount to the drop amount.
		drop += control * friction * gpGlobals->frametime;
	}
	else if (player->GetGroundEntity() != NULL)  // On an entity that is the ground
	{
		if (m_tCurPaintInfo.type == SPEED_POWER)
			friction = (sv_friction.GetFloat() * (player->m_surfaceFriction * pl_speedPaintFrictionDivisor.GetFloat()));
		else
			friction = sv_friction.GetFloat() * player->m_surfaceFriction;

		// Bleed off some speed, but if we have less than the bleed
		//  threshold, bleed the threshold amount.
		control = (speed < sv_stopspeed.GetFloat()) ? sv_stopspeed.GetFloat() : speed;

		// Add the amount to the drop amount.
		drop += control * friction * gpGlobals->frametime;
	}

	// scale the velocity
	newspeed = speed - drop;
	if (newspeed < 0)
		newspeed = 0;

	if (newspeed != speed)
	{
		// Determine proportion of old speed we are using.
		newspeed /= speed;
		// Adjust velocity according to proportion.
		VectorScale(mv->m_vecVelocity, newspeed, mv->m_vecVelocity);
	}

	mv->m_outWishVel -= (1.f - newspeed) * mv->m_vecVelocity;
}

void CEmulsionGameMovement::CheckParameters()
{
	QAngle	v_angle;

	// TODO: isolate the bad math happening in here, if enabled this stops player movement
	if (player->GetMoveType() != MOVETYPE_ISOMETRIC &&
		player->GetMoveType() != MOVETYPE_NOCLIP &&
		player->GetMoveType() != MOVETYPE_OBSERVER &&
		player->GetMoveType() != MOVETYPE_STICK)
	{
		float spd;
		float maxspeed;

		spd = (mv->m_flForwardMove * mv->m_flForwardMove) +
			(mv->m_flSideMove * mv->m_flSideMove) +
			(mv->m_flUpMove * mv->m_flUpMove);

		if (spd > 0)
			int k = 0;

		if (m_tCurPaintInfo.type == SPEED_POWER) {
			maxspeed = pl_speedPaintMoveSpeed.GetFloat();
			if (maxspeed != 0.0)
			{
				mv->m_flMaxSpeed = MIN(pl_speedPaintMoveSpeed.GetFloat(), mv->m_flMaxSpeed);
			}
		}
		else {
			maxspeed = mv->m_flClientMaxSpeed;
			if (maxspeed != 0.0)
			{
				mv->m_flMaxSpeed = MIN(maxspeed, mv->m_flMaxSpeed);
			}
		}

		// Slow down by the speed factor
		float flSpeedFactor = 1.0f;

		surfacedata_t* pSurfaceData = player->m_pSurfaceData;


		if (pSurfaceData)
		{
			flSpeedFactor = pSurfaceData->game.maxSpeedFactor;

			if (flSpeedFactor == 0)
				flSpeedFactor = 1.0f;
		}

		// If we have a constraint, slow down because of that too.
		float flConstraintSpeedFactor = ComputeConstraintSpeedFactor();
		if (flConstraintSpeedFactor < flSpeedFactor)
			flSpeedFactor = flConstraintSpeedFactor;

		mv->m_flMaxSpeed *= flSpeedFactor;

		if (g_bMovementOptimizations)
		{
			// Same thing but only do the sqrt if we have to.
			if ((spd != 0.0) && (spd > mv->m_flMaxSpeed * mv->m_flMaxSpeed))
			{
				float fRatio = mv->m_flMaxSpeed / sqrt(spd);
				mv->m_flForwardMove *= fRatio;
				mv->m_flSideMove *= fRatio;
				mv->m_flUpMove *= fRatio;
			}
		}
		else
		{
			spd = sqrt(spd);
			if ((spd != 0.0) && (spd > mv->m_flMaxSpeed))
			{
				float fRatio = mv->m_flMaxSpeed / spd;
				mv->m_flForwardMove *= fRatio;
				mv->m_flSideMove *= fRatio;
				mv->m_flUpMove *= fRatio;
			}
		}
	}

	if (player->GetFlags() & FL_FROZEN ||
		player->GetFlags() & FL_ONTRAIN ||
		IsDead())
	{
		mv->m_flForwardMove = 0;
		mv->m_flSideMove = 0;
		mv->m_flUpMove = 0;
	}

	DecayPunchAngle();

	// Take angles from command.
	if (!IsDead())
	{
		v_angle = mv->m_vecAngles;
		v_angle = v_angle + player->m_Local.m_vecPunchAngle;

		// Now adjust roll angle
		if (player->GetMoveType() != MOVETYPE_ISOMETRIC &&
			player->GetMoveType() != MOVETYPE_NOCLIP)
		{
			mv->m_vecAngles[ROLL] = CalcRoll(v_angle, mv->m_vecVelocity, sv_rollangle.GetFloat(), sv_rollspeed.GetFloat());
		}
		else
		{
			mv->m_vecAngles[ROLL] = 0.0; // v_angle[ ROLL ];
		}
		mv->m_vecAngles[PITCH] = v_angle[PITCH];
		mv->m_vecAngles[YAW] = v_angle[YAW];
	}
	else
	{
		mv->m_vecAngles = mv->m_vecOldAngles;
	}

	// Set dead player view_offset
	if (IsDead())
	{
		player->SetViewOffset(VEC_DEAD_VIEWHEIGHT);
	}

	// Adjust client view angles to match values used on server.
	if (mv->m_vecAngles[YAW] > 180.0f)
	{
		mv->m_vecAngles[YAW] -= 360.0f;
	}
}

void CEmulsionGameMovement::CheckFalling()
{
	if (player->GetGroundEntity() != NULL && !IsDead())
	{
		bool bAlive = true;
		float fvol = 0.5;

#ifdef GAME_DLL
		//if(m_tCurPaintInfo.type != PORTAL_POWER)
		PaintInfo_t m_tTempPaintInfo = CheckPaintedSurface();
		
		switch (m_tTempPaintInfo.type) {
		case BOUNCE_POWER:
			if (player->m_nButtons & IN_DUCK)
				break;

			if (player->m_Local.m_flFallVelocity != 0.0f) {
				//m_tCurPaintInfo = m_tTempPaintInfo;
				//Msg("Fall Vel: %f", player->m_Local.m_flFallVelocity);
				//BouncePlayer(m_tTempPaintInfo.plane);
				
			}
			
			if (player->GetGroundEntity() != NULL)
				player->m_Local.m_flFallVelocity = 0;

			return;
			
		default:
			break;
		}
#endif

		if (!(player->m_Local.m_flFallVelocity >= pl_fallpunchthreshold.GetFloat()))
			return;

#ifdef GAME_DLL
		switch (m_tCurPaintInfo.type) {
		case SPEED_POWER:
			PlayPaintEntrySound(SPEED_POWER, true); // force the entry sound here
			break;
		case PORTAL_POWER:
			PlayPaintEntrySound(PORTAL_POWER, true); // force the entry sound here
			break;
		default:
			break;
		}
#endif

		if (player->GetWaterLevel() > 0)
		{
			// They landed in water.
		}
		else
		{
			// Scale it down if we landed on something that's floating...
			if (player->GetGroundEntity()->IsFloating())
				player->m_Local.m_flFallVelocity -= PLAYER_LAND_ON_FLOATING_OBJECT;

			//
			// They hit the ground.
			//

			// TODO: make this a 3 dim check for moving bounce paint walls / floors / ceilings
			if (player->GetGroundEntity()->GetAbsVelocity().z < 0.0f)
			{
				// Player landed on a descending object. Subtract the velocity of the ground entity.
				player->m_Local.m_flFallVelocity += player->GetGroundEntity()->GetAbsVelocity().z;
				player->m_Local.m_flFallVelocity = MAX(0.1f, player->m_Local.m_flFallVelocity);
			}
		}

		PlayerRoughLandingEffects(fvol);

		if (bAlive)
			MoveHelper()->PlayerSetAnimation(PLAYER_WALK);
	}
	else {
#ifdef GAME_DLL
		//if(m_tCurPaintInfo.type != PORTAL_POWER)
		PaintInfo_t m_tTempPaintInfo = CheckPaintedSurface();

		switch (m_tTempPaintInfo.type) {
		case BOUNCE_POWER:
			if (player->m_nButtons & IN_DUCK)
				break;

			if (player->m_Local.m_flFallVelocity != 0.0f) {
				m_tCurPaintInfo = m_tTempPaintInfo;
				//Msg("(%f, %f, %f)\n", mv->m_vecVelocity.x, mv->m_vecVelocity.y, mv->m_vecVelocity.z);
				float bounceMult = mv->m_vecVelocity.Length();
				BouncePlayer(m_tTempPaintInfo.plane, sqrt(bounceMult) * 0.25f);

			}

			if (player->GetGroundEntity() != NULL)
				player->m_Local.m_flFallVelocity = 0;

			return;

		default:
			break;
		}
#endif
	}
	
	//
	// Clear the fall velocity so the impact doesn't happen again.
	//
	if (player->GetGroundEntity() != NULL)
		player->m_Local.m_flFallVelocity = 0;
}

void CEmulsionGameMovement::StayOnGround()
{
	BaseClass::StayOnGround();
	return;

	trace_t trace;
	Vector start(mv->GetAbsOrigin());
	Vector end(mv->GetAbsOrigin());
	Vector nAxis = GetGravityDir();

	VecNeg(nAxis);

	//start.z += 2;
	//end.z -= player->GetStepSize();

	start += nAxis * 2;
	end -= (-1 * GetGravityDir()) * player->GetStepSize();

	// See how far up we can go without getting stuck

	TracePlayerBBox(mv->GetAbsOrigin(), start, PlayerSolidMask(), COLLISION_GROUP_PLAYER_MOVEMENT, trace);
	start = trace.endpos;

	// using trace.startsolid is unreliable here, it doesn't get set when
	// tracing bounding box vs. terrain
	
	// Now trace down from a known safe position
	TracePlayerBBox(start, end, PlayerSolidMask(), COLLISION_GROUP_PLAYER_MOVEMENT, trace);
	if (trace.fraction > 0.0f &&			// must go somewhere
		trace.fraction < 1.0f &&			// must hit something
		!trace.startsolid &&				// can't be embedded in a solid
		(trace.plane.normal * (-1 * GetGravityDir())).Length() >= 0.7)		// can't hit a steep slope that we can't stand on anyway
		//trace.plane.normal[2] >= 0.7)		// can't hit a steep slope that we can't stand on anyway
	{
		//float flDelta = fabs(mv->GetAbsOrigin().z - trace.endpos.z);
		float flDelta = fabs((mv->GetAbsOrigin() * GetGravityDir()).Length() - (trace.endpos * GetGravityDir()).Length());

		//This is incredibly hacky. The real problem is that trace returning that strange value we can't network over.
		if (flDelta > 0.5f * COORD_RESOLUTION)
		{
			mv->SetAbsOrigin(trace.endpos);
		}
	}
}

void CEmulsionGameMovement::StartGravity() {
	float pGrav = player->GetGravity();
	float ent_gravity = pGrav ? pGrav : 1.0f;

	// Add gravity so they'll be in the correct position during movement
	// yes, this 0.5 looks wrong, but it's not.
	mv->m_vecVelocity += GetGravityDir() * (ent_gravity * sv_gravity.GetFloat() * 0.5 * gpGlobals->frametime);
	mv->m_vecVelocity += (player->GetBaseVelocity() * GetGravityDir()) * gpGlobals->frametime;

	Vector temp = player->GetBaseVelocity() * GetGravityDir();
	player->SetBaseVelocity(temp);

	CheckVelocity();
}

void CEmulsionGameMovement::FinishGravity() {
	if (player->m_flWaterJumpTime)
		return;

	float pGrav = player->GetGravity();
	float ent_gravity = pGrav ? pGrav : 1.0f;

	// Get the correct velocity for the end of the dt 
	mv->m_vecVelocity += GetGravityDir() * (ent_gravity * sv_gravity.GetFloat() * gpGlobals->frametime * 0.5);

	CheckVelocity();
}

void CEmulsionGameMovement::SetGroundEntity(trace_t* pm)
{
	Vector nAxis = GetGravityDir();
	VecNeg(nAxis);

	CBaseEntity* newGround = pm ? pm->m_pEnt : NULL;

	CBaseEntity* oldGround = player->GetGroundEntity();
	Vector vecBaseVelocity = player->GetBaseVelocity();

	if (!oldGround && newGround)
	{
		// Subtract ground velocity at instant we hit ground jumping
		vecBaseVelocity -= newGround->GetAbsVelocity();
		vecBaseVelocity = (vecBaseVelocity * nAxis) + (GetGravityDir() * newGround->GetAbsVelocity());
	}
	else if (oldGround && !newGround)
	{
		// Add in ground velocity at instant we started jumping
		vecBaseVelocity += oldGround->GetAbsVelocity();
		vecBaseVelocity = (vecBaseVelocity * nAxis) + (GetGravityDir() * oldGround->GetAbsVelocity());
	}

	player->SetBaseVelocity(vecBaseVelocity);
	player->SetGroundEntity(newGround);

	// If we are on something...

	if (newGround)
	{
		CategorizeGroundSurface(*pm);

		// Then we are not in water jump sequence
		player->m_flWaterJumpTime = 0;

		// Standing on an entity other than the world, so signal that we are touching something.
		if (!pm->DidHitWorld())
		{
			MoveHelper()->AddToTouched(*pm, mv->m_vecVelocity);
		}

		if (player->GetMoveType() != MOVETYPE_NOCLIP)
			mv->m_vecVelocity *= nAxis;
	}
}

bool CEmulsionGameMovement::CheckJumpButton() {
	if (player->pl.deadflag)
	{
		mv->m_nOldButtons |= IN_JUMP;	// don't jump again until released
		return false;
	}

	// See if we are waterjumping.  If so, decrement count and return.
	if (player->m_flWaterJumpTime)
	{
		player->m_flWaterJumpTime -= gpGlobals->frametime;
		if (player->m_flWaterJumpTime < 0)
			player->m_flWaterJumpTime = 0;

		return false;
	}

	// If we are in the water most of the way...
	if (player->GetWaterLevel() >= 2)
	{
		// swimming, not jumping
		SetGroundEntity(NULL);

		if (player->GetWaterType() == CONTENTS_WATER)    // We move up a certain amount
			mv->m_vecVelocity[2] = 100;
		else if (player->GetWaterType() == CONTENTS_SLIME)
			mv->m_vecVelocity[2] = 80;

		// play swiming sound
		if (player->m_flSwimSoundTime <= 0)
		{
			// Don't play sound again for 1 second
			player->m_flSwimSoundTime = 1000;
			PlaySwimSound();
		}

		return false;
	}

	// No more effect
	if (player->GetGroundEntity() == NULL)
	{
		mv->m_nOldButtons |= IN_JUMP;
		return false;		// in air, so no effect
	}

	// Don't allow jumping when the player is in a stasis field.
	if (player->m_Local.m_bSlowMovement)
		return false;

	if (mv->m_nOldButtons & IN_JUMP)
		return false;		// don't pogo stick

	// Cannot jump will in the unduck transition.
	if (player->m_Local.m_bDucking && (player->GetFlags() & FL_DUCKING))
		return false;

	// Still updating the eye position.
	if (player->m_Local.m_nDuckJumpTimeMsecs > 0)
		return false;


	surfacedata_t* pSurfDat = player->m_pSurfaceData; // here for debugging

	// In the air now.
	SetGroundEntity(NULL);
	if (m_tCurPaintInfo.type != BOUNCE_POWER)
		player->PlayStepSound((Vector&)mv->GetAbsOrigin(), player->m_pSurfaceData, 1.0, true);
	MoveHelper()->PlayerSetAnimation(PLAYER_JUMP);

	float flGroundFactor = sv_jumpFactor.GetFloat();
	float flMul;
	if (player->m_pSurfaceData)
	{
		flGroundFactor = player->m_pSurfaceData->game.jumpFactor;
	}

	if (m_tCurPaintInfo.type == BOUNCE_POWER) {
		//flMul = sqrt(2 * sv_gravity.GetFloat() * pl_bouncePaintFactor.GetFloat());
		//PlaySoundInternal("Player.JumpPowerUse");
		BouncePlayer(m_tCurPaintInfo.plane);
		goto J; // it works, stay mad. -Klaxon
	}
	else {
		if (g_bMovementOptimizations)
			flMul = 268.3281572999747f;
		else
			flMul = sqrt(2 * sv_gravity.GetFloat() * GAMEMOVEMENT_JUMP_HEIGHT);
	}

	// Acclerate upward
	// If we are ducking...
	float startz = mv->m_vecVelocity[2];
	if ((player->m_Local.m_bDucking) || (player->GetFlags() & FL_DUCKING))
	{
		// d = 0.5 * g * t^2		- distance traveled with linear accel
		// t = sqrt(2.0 * 45 / g)	- how long to fall 45 units
		// v = g * t				- velocity at the end (just invert it to jump up that high)
		// v = g * sqrt(2.0 * 45 / g )
		// v^2 = g * g * 2.0 * 45 / g
		// v = sqrt( g * 2.0 * 45 )
		//mv->m_vecVelocity[2] = flGroundFactor * flMul;  // 2 * gravity * height
		mv->m_vecVelocity += (-1 * m_vecGravity) * flGroundFactor * flMul;  // 2 * gravity * height
	}
	else
	{
		if (m_tCurPaintInfo.type == BOUNCE_POWER)
			mv->m_vecVelocity += player->Forward() * mv->m_vecVelocity.Length() / (flGroundFactor * 2.5f);

		mv->m_vecVelocity += (-1 * m_vecGravity) * flGroundFactor * flMul;
	}

#ifdef GAME_DLL
	extern bool g_bStickPaintJumpRelease;
	if (m_tCurPaintInfo.type == PORTAL_POWER)
		g_bStickPaintJumpRelease = true;
#endif

	// ACC FORHOP

	bool bAllowBunnyHopperSpeedBoost = (gpGlobals->maxClients == 1);


	if (bAllowBunnyHopperSpeedBoost)
	{
		//CHLMoveData *pMoveData = ( CHLMoveData* )mv;
		Vector vecForward;
		AngleVectors(mv->m_vecViewAngles, &vecForward);
		vecForward.z = 0;
		VectorNormalize(vecForward);

		// We give a certain percentage of the current forward movement as a bonus to the jump speed.  That bonus is clipped
		// to not accumulate over time.
		float flSpeedBoostPerc = ( /*!pMoveData->m_bIsSprinting &&*/ !player->m_Local.m_bDucked) ? 0.5f : 0.1f;
		float flSpeedAddition = fabs(mv->m_flForwardMove * flSpeedBoostPerc);
		float flMaxSpeed = mv->m_flMaxSpeed + (mv->m_flMaxSpeed * flSpeedBoostPerc);
		float flNewSpeed = (flSpeedAddition + mv->m_vecVelocity.Length2D());

		// If we're over the maximum, we want to only boost as much as will get us to the goal speed
		if (flNewSpeed > flMaxSpeed)
		{
			flSpeedAddition -= flNewSpeed - flMaxSpeed;
		}

		if (mv->m_flForwardMove < 0.0f)
			flSpeedAddition *= -1.0f;

		// Add it on
		VectorAdd((vecForward * flSpeedAddition), mv->m_vecVelocity, mv->m_vecVelocity);
	}

	//

	FinishGravity();

	CheckV(player->CurrentCommandNumber(), "CheckJump", mv->m_vecVelocity);

	mv->m_outJumpVel.z += mv->m_vecVelocity[2] - startz;
	mv->m_outStepHeight += 0.15f;


	bool bSetDuckJump = (gpGlobals->maxClients == 1); //most games we only set duck jump if the game is single player


	// Set jump time.
	if (bSetDuckJump)
	{
		player->m_Local.m_nJumpTimeMsecs = GAMEMOVEMENT_JUMP_TIME;
		player->m_Local.m_bInDuckJump = true;
	}

J:
	// Flag that we jumped.
	mv->m_nOldButtons |= IN_JUMP;	// don't jump again until released
	return true;
}

short wallBounce = 0;

int CEmulsionGameMovement::TryPlayerMove(Vector* pFirstDest, trace_t* pFirstTrace)
{
	int			bumpcount, numbumps;
	Vector		dir;
	float		d;
	int			numplanes;
	Vector		planes[MAX_CLIP_PLANES];
	Vector		primal_velocity, original_velocity;
	Vector      new_velocity;
	int			i, j;
	trace_t	pm;
	Vector		end;
	float		time_left, allFraction;
	int			blocked;

	//float bounceMult = mv->m_vecVelocity.Length();

	//short doBounce = 0;

	numbumps = 4;           // Bump up to four times

	blocked = 0;           // Assume not blocked
	numplanes = 0;           //  and not sliding along any planes

	VectorCopy(mv->m_vecVelocity, original_velocity);  // Store original velocity
	VectorCopy(mv->m_vecVelocity, primal_velocity);

	allFraction = 0;
	time_left = gpGlobals->frametime;   // Total time for this movement operation.

	new_velocity.Init();

	for (bumpcount = 0; bumpcount < numbumps; bumpcount++)
	{
		if (mv->m_vecVelocity.Length() == 0.0)
			break;

		// Assume we can move all the way from the current origin to the
		//  end point.
		VectorMA(mv->GetAbsOrigin(), time_left, mv->m_vecVelocity, end);

		// See if we can make it from origin to end point.
		if (g_bMovementOptimizations)
		{
			// If their velocity Z is 0, then we can avoid an extra trace here during WalkMove.
			if (pFirstDest && (end == *pFirstDest))
			{
				pm = *pFirstTrace;
			}
			else
				TracePlayerBBox(mv->GetAbsOrigin(), end, PlayerSolidMask(), COLLISION_GROUP_PLAYER_MOVEMENT, pm);
		}
		else
			TracePlayerBBox(mv->GetAbsOrigin(), end, PlayerSolidMask(), COLLISION_GROUP_PLAYER_MOVEMENT, pm);

		allFraction += pm.fraction;

		// If we started in a solid object, or we were in solid space
		//  the whole way, zero out our velocity and return that we
		//  are blocked by floor and wall.
		if (pm.allsolid)
		{
			// entity is trapped in another solid
			VectorCopy(vec3_origin, mv->m_vecVelocity);
			return 4;
		}

		// If we moved some portion of the total distance, then
		//  copy the end position into the pmove.origin and 
		//  zero the plane counter.
		if (pm.fraction > 0)
		{
			if (numbumps > 0 && pm.fraction == 1)
			{
				// There's a precision issue with terrain tracing that can cause a swept box to successfully trace
				// when the end position is stuck in the triangle.  Re-run the test with an uswept box to catch that
				// case until the bug is fixed.
				// If we detect getting stuck, don't allow the movement
				trace_t stuck;
				TracePlayerBBox(pm.endpos, pm.endpos, PlayerSolidMask(), COLLISION_GROUP_PLAYER_MOVEMENT, stuck);
				if (stuck.startsolid || stuck.fraction != 1.0f)
				{
					//Msg( "Player will become stuck!!!\n" );
					VectorCopy(vec3_origin, mv->m_vecVelocity);
					break;
				}
			}

#if defined( PLAYER_GETTING_STUCK_TESTING )
			trace_t foo;
			TracePlayerBBox(pm.endpos, pm.endpos, PlayerSolidMask(), COLLISION_GROUP_PLAYER_MOVEMENT, foo);
			if (foo.startsolid || foo.fraction != 1.0f)
			{
				Msg("Player will become stuck!!!\n");
			}
#endif
			// actually covered some distance
			mv->SetAbsOrigin(pm.endpos);
			VectorCopy(mv->m_vecVelocity, original_velocity);
			numplanes = 0;
		}

		// If we covered the entire distance, we are done
		//  and can return.
		if (pm.fraction == 1)
		{
			break;		// moved the entire distance
		}

		// Save entity that blocked us (since fraction was < 1.0)
		//  for contact
		// Add it if it's not already in the list!!!
		MoveHelper()->AddToTouched(pm, mv->m_vecVelocity);

		// If the plane we hit has a high z component in the normal, then
		//  it's probably a floor
		if (pm.plane.normal[2] > 0.7)
		{
			blocked |= 1;		// floor	
		}
		// If the plane has a zero z component in the normal, then it's a 
		//  step or wall
		if (!pm.plane.normal[2])
		{		
			blocked |= 2;		// step / wall
		}

		// Reduce amount of m_flFrameTime left by total time left * fraction
		//  that we covered.
		time_left -= time_left * pm.fraction;

		// Did we run out of planes to clip against?
		if (numplanes >= MAX_CLIP_PLANES)
		{
			// this shouldn't really happen
			//  Stop our movement if so.
			VectorCopy(vec3_origin, mv->m_vecVelocity);
			break;
		}

		// Set up next clipping plane
		VectorCopy(pm.plane.normal, planes[numplanes]);
		numplanes++;

		// modify original_velocity so it parallels all of the clip planes
		//

		// reflect player velocity 
		// Only give this a try for first impact plane because you can get yourself stuck in an acute corner by jumping in place
		//  and pressing forward and nobody was really using this bounce/reflection feature anyway...
		if (numplanes == 1 &&
			player->GetMoveType() == MOVETYPE_WALK &&
			player->GetGroundEntity() == NULL)
		{
			for (i = 0; i < numplanes; i++)
			{
				if (planes[i][2] > 0.7)
				{
					// floor or slope
					ClipVelocity(original_velocity, planes[i], new_velocity, 1);
					VectorCopy(new_velocity, original_velocity);
				}
				else
					ClipVelocity(original_velocity, planes[i], new_velocity, 1.0 + sv_bounce.GetFloat() * (1 - player->m_surfaceFriction));
			}

			VectorCopy(new_velocity, mv->m_vecVelocity);
			VectorCopy(new_velocity, original_velocity);
		}
		else
		{
			for (i = 0; i < numplanes; i++)
			{
				ClipVelocity(original_velocity, planes[i], mv->m_vecVelocity, 1);

				for (j = 0; j < numplanes; j++)
				{
					if (j != i)
					{
						// Are we now moving against this plane?
						if (mv->m_vecVelocity.Dot(planes[j]) < 0)
							break;	// not ok
					}
				}

				if (j == numplanes)  // Didn't have to clip, so we're ok
					break;
			}

			// Did we go all the way through plane set
			if (i == numplanes)
			{	// go along the crease
				if (numplanes != 2)
				{
					VectorCopy(vec3_origin, mv->m_vecVelocity);
					break;
				}
				CrossProduct(planes[0], planes[1], dir);
				dir.NormalizeInPlace();
				d = dir.Dot(mv->m_vecVelocity);
				VectorScale(dir, d, mv->m_vecVelocity);
			}

			//
			// if original velocity is against the original velocity, stop dead
			// to avoid tiny occilations in sloping corners
			//
			d = mv->m_vecVelocity.Dot(primal_velocity);
			if (d <= 0)
			{
				//Con_DPrintf("Back\n");
				VectorCopy(vec3_origin, mv->m_vecVelocity);
				break;
			}
		}
	}

	if (allFraction == 0)
		VectorCopy(vec3_origin, mv->m_vecVelocity);

	// Check if they slammed into a wall
	float fSlamVol = 0.0f;
	float fLateralStoppingAmount = primal_velocity.Length2D() - mv->m_vecVelocity.Length2D();

	if (fLateralStoppingAmount > PLAYER_MAX_SAFE_FALL_SPEED * 2.0f)
		fSlamVol = 1.0f;
	else if (fLateralStoppingAmount > PLAYER_MAX_SAFE_FALL_SPEED)
		fSlamVol = 0.85f;

	//if (fSlamVol > 0.0f)
	//	PlayerRoughLandingEffects(fSlamVol);

	return blocked;
}

static const float g_flStickTransitionTime = 10.0f;
static float g_flCurStickTransitionTime = 0.0f;

void CEmulsionGameMovement::ProcessPowerUpdate() {
#ifdef GAME_DLL
	PaintInfo_t m_tNewInfo = CheckPaintedSurface();
	CEmulsionPlayer* pPlayer = ToEmulsionPlayer(player);
	
	switch (m_tNewInfo.type) {
	case BOUNCE_POWER:
		if (m_tCurPaintInfo.type == SPEED_POWER) {
			BouncePlayer(m_tNewInfo.plane);
			break;
		}
		else if (player->GetGroundEntity() != NULL) {
			PlayPaintEntrySound(m_tNewInfo.type);
		}
		break;
	case SPEED_POWER:
		if (player->GetGroundEntity() != NULL) {
			PlayPaintEntrySound(m_tNewInfo.type);
		}
		break;
	case PORTAL_POWER: // handled by the player
		//PlayPaintEntrySound(m_tNewInfo.type);
		break;
	default:
		//if (m_tCurPaintInfo.type == PORTAL_POWER && g_flCurStickTransitionTime <= 0.0f)
		//	pPlayer->UnStickPlayer();
		if(m_tCurPaintInfo.type != PORTAL_POWER)
			DetermineExitSound(m_tNewInfo.type);
		break;
	}

	m_tCurPaintInfo = m_tNewInfo;
	pPlayer->m_nPaintPower = (int)m_tCurPaintInfo.type;
#endif
}

// POWERS:

// halfway there bounce paint
//void CEmulsionGameMovement::BouncePlayer(cplane_t plane, float multiplier) {
//
//	Msg("Bounce mult: %f \n", multiplier);
//
//	if (multiplier < 2) multiplier = 2;
//	if (m_tCurPaintInfo.type == SPEED_POWER) multiplier + 1;
//
//	float flMul = sqrt(multiplier * sv_gravity.GetFloat() * pl_bouncePaintFactor.GetFloat());
//
//	Vector curForwardVel;
//	Vector curNegVel;
//	Vector result;
//
//	curNegVel = mv->m_vecVelocity * player->Forward();
//	if (curNegVel.LargestComponent() < 0) 
//		curNegVel *= -1;
//	
//	curForwardVel = player->Forward() * curNegVel;
//
//	if(m_tCurPaintInfo.type != SPEED_POWER)
//		result = curForwardVel.Normalized() * (flMul / 2);
//	result += (plane.normal) * flMul;
//
//	if (plane.normal[2] == 0)
//		result += (-1 * m_vecGravity) * (flMul);
//
//	if (m_tCurPaintInfo.type == SPEED_POWER)
//		mv->m_vecVelocity += result;
//	else
//		mv->m_vecVelocity = result;
//
//	wallBounce = 0;
//	m_bCancelNextExitSound = true;
//	PlaySoundInternal("Player.JumpPowerUse");
//
//	if (pl_showBouncePowerNormal.GetBool()) {
//		Msg("Result: (%f, %f, %f)\n", result.x, result.y, result.z);
//		Msg("Actual: (%f, %f, %f)\n", mv->m_vecVelocity.x, mv->m_vecVelocity.y, mv->m_vecVelocity.z);
//	}
//		
//
//	SetGroundEntity(NULL);
//}

/// <summary>
///  this should've been a thing already. wtf source
/// </summary>
/// <param name="in">: the original vector</param>
/// <param name="min">: new min</param>
/// <param name="max">: new max</param>
/// <returns>the clamped vector (duh)</returns>
//inline Vector ClampVector(Vector in, Vector min, Vector max) {
//
//}

Vector ClampMinVector(Vector in, Vector min) {

	Vector result = in;
	for (int i = 0; i < 3; i++)
		if (result[i] < min[i])
			result[i] = min[i];

	return result;
}

void CEmulsionGameMovement::BouncePlayer(cplane_t plane, float multiplier) {

	Msg("Bounce mult: %f \n", multiplier);

	if (multiplier < 2) multiplier = 2;
	if (m_tCurPaintInfo.type == SPEED_POWER) multiplier + 1;

	float flMul = sqrt(multiplier /** sv_gravity.GetFloat()*/ * (mv->m_vecVelocity.Length() * 0.5f)/*pl_bouncePaintFactor.GetFloat()*/);
	
	Vector min = plane.normal * 500.0f;
	Vector result = min + (plane.normal * flMul);// + addVel;

	Vector positive = mv->m_vecVelocity * plane.normal;
	positive[positive.LargestComponent()] *= -1; // gotta add back this velocity somehow
	
	Vector curNegVel = mv->m_vecVelocity * player->Forward();
	Vector forward = player->Forward() * curNegVel;
	forward *= 0.5f;
	forward[plane.normal.LargestComponent()] = 0; // zero out the main bounce axis

	mv->m_vecVelocity += result + positive + forward;

	wallBounce = 0;
	m_bCancelNextExitSound = true;
	PlaySoundInternal("Player.JumpPowerUse");

	if (pl_showBouncePowerNormal.GetBool()) {
		Msg("flMul: %f\n", flMul);
		Msg("positive: (%f, %f, %f)\n", positive.x, positive.y, positive.z);
		Msg("min: (%f, %f, %f)\n", min.x, min.y, min.z);
		Msg("Result: (%f, %f, %f)\n", result.x, result.y, result.z);
		Msg("Actual: (%f, %f, %f)\n", mv->m_vecVelocity.x, mv->m_vecVelocity.y, mv->m_vecVelocity.z);
	}

	SetGroundEntity(NULL);
}

void CEmulsionGameMovement::CalculateStickAngles() {

	if (m_angDefaultAngles == QAngle(0, 0, 0))
		m_angDefaultAngles = mv->m_vecViewAngles;

	QAngle stickAngles = player->EyeAngles();
	Vector vecAxisOfRotation = CrossProduct(Vector(0, 0, 1), -1 * m_vecGravity);
	float flAngleOfRotation = RAD2DEG(acos(DotProduct(Vector(0, 0, 1), -1 * m_vecGravity)));
	
	VMatrix eyerotmat;
	MatrixBuildRotationAboutAxis(eyerotmat, vecAxisOfRotation, flAngleOfRotation);

	Vector vecForward, vecUp;
	AngleVectors(stickAngles, &vecForward, nullptr, &vecUp);
	VectorRotate(vecForward, eyerotmat.As3x4(), vecForward);
	VectorRotate(vecUp, eyerotmat.As3x4(), vecUp);

	VectorAngles(vecForward, vecUp, m_angStickAngles);
	m_vecStickForward = vecForward;
	m_vecStickUp = vecUp;
}

void CEmulsionGameMovement::UnStickPlayer() {
	SetGravityDir(Vector(0, 0, -1));
	player->SetMoveType(MOVETYPE_WALK);
}

void CEmulsionGameMovement::PlayPaintEntrySound(PaintPowerType type, bool force) {
	const char* entrySound = "";

	if (m_tCurPaintInfo.type == type && !force)
		return;

	switch (type) {
	case BOUNCE_POWER:
		entrySound = "Player.EnterBouncePaint";
		break;
	case SPEED_POWER:
		entrySound = "Player.EnterSpeedPaint";
		break;
	case PORTAL_POWER:
		entrySound = "Player.EnterStickPaint";

		if (m_tCurPaintInfo.type == PORTAL_POWER)
			return;

		break;
	}

	PlaySoundInternal(entrySound);
}

void CEmulsionGameMovement::PlayPaintExitSound(PaintPowerType type) {
	const char* exitSound;

	if (type == BOUNCE_POWER && player->GetGroundEntity() == NULL)
		return;

	switch (type) {
	case BOUNCE_POWER:
		exitSound = "Player.ExitBouncePaint";
		break;
	case SPEED_POWER:
		exitSound = "Player.ExitSpeedPaint";
		break;
	case PORTAL_POWER:
		exitSound = "Player.ExitStickPaint";
		break;
	}

	PlaySoundInternal(exitSound);
}

void CEmulsionGameMovement::DetermineExitSound(PaintPowerType type) {

	if (m_bCancelNextExitSound) {
		m_bCancelNextExitSound = false;
		return;
	}

	if (m_tCurPaintInfo.type == BOUNCE_POWER && player->GetGroundEntity() == NULL)
		return;
	else if (m_tCurPaintInfo.type == SPEED_POWER && type == BOUNCE_POWER)
		return;
	else if (m_tCurPaintInfo.type < 4 && type > 3)
		PlayPaintExitSound(m_tCurPaintInfo.type);

}

void CEmulsionGameMovement::PlaySoundInternal(const char* sound) {

	CRecipientFilter filter;
	CSoundParameters params;
	filter.AddRecipientsByPAS(player->GetAbsOrigin());

#ifndef CLIENT_DLL
	// in MP, server removes all players in the vecOrigin's PVS, these players generate the footsteps client side
	if (gpGlobals->maxClients > 1)
	{
		filter.RemoveRecipientsByPVS(player->GetAbsOrigin());
	}
#endif

	if (!CBaseEntity::GetParametersForSound(sound, params, NULL))
		return;

	EmitSound_t ep;
	ep.m_nChannel = params.channel;
	ep.m_pSoundName = params.soundname;
	ep.m_flVolume = params.volume;
	ep.m_SoundLevel = params.soundlevel;
	ep.m_nFlags = 0;
	ep.m_nPitch = params.pitch;
	ep.m_pOrigin = &player->GetAbsOrigin();

	player->EmitSound(filter, player->entindex(), ep);
}

#ifdef GAME_DLL

// check if paint a is better than paint b
static bool IsPaintHigherPriority(PaintPowerType a, PaintPowerType b) {

	switch (a) {
	case BOUNCE_POWER:
		if (b == SPEED_POWER || b == PORTAL_POWER)
			return true;
		break;
	case SPEED_POWER:
		if (b == BOUNCE_POWER)
			return false;
		else if (b == PORTAL_POWER)
			return true;
		break;
	case PORTAL_POWER:
		if (b == BOUNCE_POWER || b == SPEED_POWER)
			return false;
		break;
	case NO_POWER:
		return true;
	default:
		return false;
	}

	return false;
}

PaintInfo_t CEmulsionGameMovement::CheckPaintedSurface() {
	
	PaintInfo_t info = PaintInfo_t();

	if (mv == NULL || !engine->HasPaintmap())
		return info;

	if (g_bInStickTransition)
		return m_tCurPaintInfo;

	CalculateStickAngles();

	CUtlVector<unsigned char>* m_pPowers = new CUtlVector<unsigned char>();
	CEmulsionPlayer* pPlayer = (CEmulsionPlayer*)UTIL_GetLocalPlayer();

	Vector vecVel = mv->m_vecVelocity;
	Vector vecStart = mv->GetAbsOrigin() + (m_tCurPaintInfo.plane.normal * player->GetViewOffset().Length()) * 0.75f;
	Vector vecEnd;
	trace_t tr;
	float reach = 26.0f; // 24.0f;

	VectorNormalize(vecVel);
	if (m_tCurPaintInfo.type == PORTAL_POWER)
		VectorMA(vecStart, reach, vecVel * reach, vecEnd);
	else
		VectorMA(vecStart, reach, vecVel, vecEnd);

	TracePlayerBBox(vecStart, vecEnd, PlayerSolidMask(), COLLISION_GROUP_PLAYER_MOVEMENT, tr);
	
	if (tr.m_pEnt && tr.m_pEnt->IsBSPModel())
		engine->SphereTracePaintSurface(tr.m_pEnt->GetModel(), tr.endpos, tr.plane.normal, pl_paintTraceRadius.GetFloat(), *m_pPowers);

	// check for paints under us if we're on the ground
	if (player->GetGroundEntity() != NULL || m_bIsTouchingStickParent) {
		TracePlayerBBox(vecStart, mv->GetAbsOrigin() + (reach * m_vecGravity), PlayerSolidMask(), COLLISION_GROUP_PLAYER_MOVEMENT, tr);

		if (tr.m_pEnt && tr.m_pEnt->IsBSPModel())
			engine->SphereTracePaintSurface(tr.m_pEnt->GetModel(), tr.endpos, tr.plane.normal, pl_paintTraceRadius.GetFloat(), *m_pPowers);
	}
	
	if (m_tCurPaintInfo.type == PORTAL_POWER) {
		trace_t str;
		Vector forward, right, up;
		CUtlVector<unsigned char>* m_pStickPowers = new CUtlVector<unsigned char>();

		// TODO: replace the EyeAngles bullshit here, it breaks the move check when on walls
		AngleVectors(m_angStickAngles, &forward, &right, &up);
		//UTIL_TraceLine(vecStart, pPlayer->GetForward_Stick() * 750, MASK_ALL, pPlayer, COLLISION_GROUP_PLAYER_MOVEMENT, &str); // no.
		TracePlayerBBox(vecStart, vecStart + (forward * reach), PlayerSolidMask(), COLLISION_GROUP_PLAYER_MOVEMENT, str);

		if (str.m_pEnt && str.m_pEnt->IsBSPModel())
			engine->SphereTracePaintSurface(str.m_pEnt->GetModel(), str.endpos, str.plane.normal, pl_paintTraceRadius.GetFloat(), *m_pStickPowers);


		if (m_pStickPowers->Count() > 0)
			if ((PaintPowerType)m_pStickPowers->Tail() == PORTAL_POWER) {
				info.type = PORTAL_POWER;
				info.tr = &str;
				info.m_pEnt = str.m_pEnt;
				info.plane = str.plane;
				info.pos = str.endpos;
				info.m_bIsPainted = true;

				return info;
			}
	}

	info.type = GetHighestPriorityPaint(m_pPowers);

	info.tr = &tr;
	info.m_pEnt = tr.m_pEnt;
	info.plane = tr.plane;
	info.pos = tr.endpos;

	if (m_pPowers->Count() > 0)
		info.m_bIsPainted = true;

	return info;
}

PaintPowerType CEmulsionGameMovement::GetHighestPriorityPaint(CUtlVector<unsigned char>* m_pPowers) {

	PaintPowerType curBest = NO_POWER;

	if (m_pPowers->Count() < 1)
		return curBest;

	for (int i = 0; i < m_pPowers->Count(); i++) {
		PaintPowerType temp = (PaintPowerType)m_pPowers->Element(i);
		if (!IsPaintHigherPriority((PaintPowerType)m_pPowers->Element(i), curBest)) {
			curBest = temp;
		}
	}

	return curBest;
}

#endif