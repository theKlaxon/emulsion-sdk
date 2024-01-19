#include "cbase.h"
#include "basecombatweapon.h"
#include "player.h"
#include "in_buttons.h"
#include "weapon_paintgun.h"
#include "emulsion_player.h"
#include "..\public\game\shared\portal2\paint_enum.h"
#include "blobulator/blob_manager.h"

// -- paint colours 
//ConVar bounce_paint_color("bounce_paint_color", "15 252 11 255", FCVAR_REPLICATED); // i like greemn - Klax
ConVar bounce_paint_color("bounce_paint_color", "0 165 255 255", FCVAR_REPLICATED);
ConVar speed_paint_color("speed_paint_color", "255 106 0 255", FCVAR_REPLICATED);
ConVar portal_paint_color("portal_paint_color", "140 0 255 255", FCVAR_REPLICATED); // using this for stick paint

ConVar erase_color("erase_color", "0 0 0 255", FCVAR_REPLICATED);
ConVar erase_visual_color("erase_visual_color", "0 0 0 255", FCVAR_REPLICATED);
ConVar paint_color_max_diff("paint_color_max_diff", "32", FCVAR_REPLICATED, "The maximum difference between two colors for matching.");
// ---

// Paintgun power commands
void Paintgun_NextPower();
void Paintgun_PrevPower();

CWeaponPaintgun* g_playerPaintgun = nullptr;
PaintPowerType g_CurPaintgunPower = BOUNCE_POWER;

ConCommand paintgun_next("paintgun_next", Paintgun_NextPower);
ConCommand paintgun_prev("paintgun_prev", Paintgun_PrevPower);

ConVar paintgun_rad("paintgun_rad", "35", FCVAR_REPLICATED);
ConVar paintgun_strength("paintgun_strength", "5", FCVAR_REPLICATED);
ConVar paintgun_timing("paintgun_timing", "0.001f", FCVAR_REPLICATED);

void SetPaintDisplayColour(PaintPowerType power) {
	if (g_playerPaintgun == nullptr)
		return;

	switch (power) {
		case BOUNCE_POWER:
			g_playerPaintgun->SetRenderColor(bounce_paint_color.GetColor().r(), bounce_paint_color.GetColor().g(), bounce_paint_color.GetColor().b());
			break;
		case SPEED_POWER:
			g_playerPaintgun->SetRenderColor(speed_paint_color.GetColor().r(), speed_paint_color.GetColor().g(), speed_paint_color.GetColor().b());
			break;
		case PORTAL_POWER:
			g_playerPaintgun->SetRenderColor(portal_paint_color.GetColor().r(), portal_paint_color.GetColor().g(), portal_paint_color.GetColor().b());
			break;
		default:
			g_playerPaintgun->SetRenderColor(bounce_paint_color.GetColor().r(), bounce_paint_color.GetColor().g(), bounce_paint_color.GetColor().b());
			break;
	}

	
}

LINK_ENTITY_TO_CLASS(weapon_paintgun, CWeaponPaintgun);

IMPLEMENT_SERVERCLASS_ST(CWeaponPaintgun, DT_WeaponPaintgun)
END_SEND_TABLE()

CWeaponPaintgun::CWeaponPaintgun() {
	m_flCurPaintDelay = 0.0f;
	g_playerPaintgun = this;
}

int GetStreamIndex(PaintPowerType power) {

	int ret = 0;

	switch (power) {
	case BOUNCE_POWER:
		ret = 0;
		break;
	//case REFLECT_POWER:
	//	break;
	case SPEED_POWER:
		ret = 1;
		break;
	case PORTAL_POWER:
		ret = 2;
		break;
	}

	return ret;
}

void CWeaponPaintgun::FirePaint(bool erase) {

	if (gpGlobals->curtime < m_flCurPaintDelay)
		return;

	CEmulsionPlayer* pPlayer = ToEmulsionPlayer(UTIL_PlayerByIndex(1));
	Vector halfHeightOrigin = pPlayer->GetHalfHeight_Stick();// player->GetAbsOrigin() + Vector(0, 0, player->BoundingRadius() / 2);

	trace_t tr;
	UTIL_TraceLine(halfHeightOrigin, pPlayer->GetForward_Stick() * MAX_TRACE_LENGTH, MASK_ALL, pPlayer, COLLISION_GROUP_PLAYER_MOVEMENT, &tr);

	if (tr.DidHit()) {

		if (tr.DidHitWorld())
			if (!erase) {
				//engine->SpherePaintSurface(tr.m_pEnt->GetModel(), tr.endpos, g_CurPaintgunPower, paintgun_rad.GetInt(), paintgun_strength.GetInt());
				//Paint::CreateBlob(tr.startpos, pPlayer->GetForward_Stick() * 50.0f);
				((CBlobManager*)BlobulatorSystem())->CreateBlob(tr.startpos + (pPlayer->Forward() * 72.0f), pPlayer->GetForward_Stick().Normalized() * 500.0f, GetStreamIndex(g_CurPaintgunPower));
			}
			else
				engine->SpherePaintSurface(tr.m_pEnt->GetModel(), tr.endpos, NO_POWER, paintgun_rad.GetInt(), paintgun_strength.GetInt()); // erase 

		m_flCurPaintDelay = gpGlobals->curtime + paintgun_timing.GetFloat();
	}
}

// TODO: maybe make this stuff into it's own class, make an interface 
// in case a multiplayer version needs to be used at any point
static const float paintSwitchDelay = 0.15f;
float curPaintSwitchTime = 0.0f;

void Paintgun_NextPower() {

	if (gpGlobals->curtime < curPaintSwitchTime)
		return;

	switch (g_CurPaintgunPower) {
		case BOUNCE_POWER:
			g_CurPaintgunPower = SPEED_POWER;
			SetPaintDisplayColour(SPEED_POWER);
			break;
		case SPEED_POWER:
			g_CurPaintgunPower = PORTAL_POWER;
			SetPaintDisplayColour(PORTAL_POWER);
			break;
		case PORTAL_POWER:
			g_CurPaintgunPower = BOUNCE_POWER;
			SetPaintDisplayColour(BOUNCE_POWER);
			break;
		default:
			g_CurPaintgunPower = BOUNCE_POWER;
			SetPaintDisplayColour(BOUNCE_POWER);
			break;
	}

	curPaintSwitchTime = gpGlobals->curtime + paintSwitchDelay;
}

void Paintgun_PrevPower() {

	if (gpGlobals->curtime < curPaintSwitchTime)
		return;

	switch (g_CurPaintgunPower) {
		case BOUNCE_POWER:
			g_CurPaintgunPower = PORTAL_POWER;
			SetPaintDisplayColour(PORTAL_POWER);
			break;
		case SPEED_POWER:
			g_CurPaintgunPower = BOUNCE_POWER;
			SetPaintDisplayColour(BOUNCE_POWER);
			break;
		case PORTAL_POWER:
			g_CurPaintgunPower = SPEED_POWER;
			SetPaintDisplayColour(SPEED_POWER);
			break;
		default:
			g_CurPaintgunPower = BOUNCE_POWER;
			SetPaintDisplayColour(BOUNCE_POWER);
			break;
	}

	curPaintSwitchTime = gpGlobals->curtime + paintSwitchDelay;
}
