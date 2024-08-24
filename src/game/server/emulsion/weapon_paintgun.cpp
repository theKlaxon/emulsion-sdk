#include "cbase.h"
#include "basecombatweapon.h"
#include "player.h"
#include "in_buttons.h"
#include "weapon_paintgun.h"
#include "emulsion_player.h"
#include "..\public\game\shared\portal2\paint_enum.h"
#include "paintblob_manager.h"
#include "scroll_controller.h"

// -- paint colours
//ConVar portal_paint_color("portal_paint_color", "15 252 11 255", FCVAR_REPLICATED); // i like greemn -Klax
//ConVar portal_paint_color("portal_paint_color", "140 0 255 255", FCVAR_REPLICATED); // using this for stick paint

ConVar bounce_paint_color("bounce_paint_color", "0 165 255 255", FCVAR_REPLICATED);
ConVar speed_paint_color("speed_paint_color", "255 106 0 255", FCVAR_REPLICATED);
ConVar portal_paint_color("portal_paint_color", "0 200 33 255", FCVAR_REPLICATED); // p2ce's green (i was allowed to use it) -Klax
ConVar fifth_paint_color("fifth_paint_color", "251 255 0 255", FCVAR_REPLICATED);

ConVar erase_color("erase_color", "0 0 0 255", FCVAR_REPLICATED);
ConVar erase_visual_color("erase_visual_color", "0 0 0 255", FCVAR_REPLICATED);
ConVar paint_color_max_diff("paint_color_max_diff", "32", FCVAR_REPLICATED, "The maximum difference between two colors for matching.");
// ---

ConVar paintgun_fire_blobs("paintgun_fire_blobs", "1", FCVAR_NOTIFY, "Blobs are experimental!");
ConVar paintgun_rad("paintgun_rad", "64", FCVAR_REPLICATED);
ConVar paintgun_strength("paintgun_strength", "5", FCVAR_REPLICATED);
ConVar paintgun_fire_offset_z("paintgun_fire_offset_z", "-20.0f", FCVAR_REPLICATED);
ConVar paintgun_fire_offset_y("paintgun_fire_offset_y", "64.0f", FCVAR_REPLICATED);
ConVar paintgun_fire_offset_x("paintgun_fire_offset_x", "20.0f", FCVAR_REPLICATED);
ConVar paintgun_blobs_max_speed("paintgun_blobs_max_speed", "1050", FCVAR_CHEAT);
ConVar paintgun_blobs_min_speed("paintgun_blobs_min_speed", "950", FCVAR_CHEAT);
ConVar paintgun_blobs_per_second("paintgun_blobs_per_second", "40", FCVAR_CHEAT);
ConVar paintgun_blobs_max_spread_angle("paintgun_blobs_max_spread_angle", "10.0f", FCVAR_CHEAT);

// Paintgun power commands
void Paintgun_NextPower();
void Paintgun_PrevPower();

CWeaponPaintgun* g_playerPaintgun = nullptr;
PaintPowerType g_CurPaintgunPower = BOUNCE_POWER;

// TODO: maybe make this stuff into it's own class, make an interface 
// in case a multiplayer version needs to be used at any point
static const float paintSwitchDelay = 0.15f;
float curPaintSwitchTime = 0.0f;

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
		case FIFTH_POWER:
			g_playerPaintgun->SetRenderColor(fifth_paint_color.GetColor().r(), fifth_paint_color.GetColor().g(), fifth_paint_color.GetColor().b());
			break;
		default:
			g_playerPaintgun->SetRenderColor(bounce_paint_color.GetColor().r(), bounce_paint_color.GetColor().g(), bounce_paint_color.GetColor().b());
			break;
	}
}

LINK_ENTITY_TO_CLASS(weapon_paintgun, CWeaponPaintgun);

IMPLEMENT_SERVERCLASS_ST(CWeaponPaintgun, DT_WeaponPaintgun)
	SendPropInt(SENDINFO(m_nPaintType), 12, SPROP_UNSIGNED)
END_SEND_TABLE()

CWeaponPaintgun::CWeaponPaintgun() {
	m_flNextFireTime = 0.0f;
}

void CWeaponPaintgun::Spawn() {
	BaseClass::Spawn();

	g_playerPaintgun = this;
	g_CurPaintgunPower = BOUNCE_POWER;
	curPaintSwitchTime = 0.0f;
}

void CWeaponPaintgun::Equip(CBaseCombatCharacter* pOwner) {
	BaseClass::Equip(pOwner);
	g_playerPaintgun = this;
	g_CurPaintgunPower = BOUNCE_POWER;
	curPaintSwitchTime = 0.0f;
	m_flNextFireTime = 0.0f;

	g_sScroller.SetUp(Paintgun_NextPower);
	g_sScroller.SetDn(Paintgun_PrevPower);
}

void CWeaponPaintgun::Drop(const Vector& vecVelocity) {
	BaseClass::Drop(vecVelocity);

	g_sScroller.ClearBinds();
}

float g_flFireDelay = 1.0f / paintgun_blobs_per_second.GetFloat();

// returns a randomised version of the vector given
void Vector_RandomiseInCone(Vector& vec, float flMaxAng) {

	Vector random = {};
	float rad = (flMaxAng * 0.01f);

	int randAx = RandomInt(1, 2);
	vec[randAx] += RandomFloat(-rad, rad);
}

void CWeaponPaintgun::FirePaint(bool erase) {

	CEmulsionPlayer* pPlayer = ToEmulsionPlayer(UTIL_PlayerByIndex(1));
	Vector halfHeightOrigin = pPlayer->GetHalfHeight_Stick();

	Vector forward, right, up;
	AngleVectors(pPlayer->StickEyeAngles(), &forward, &right, &up);

	Vector eyePosition = pPlayer->StickEyeOrigin();

	//trace_t tr;
	//UTIL_TraceLine(eyePosition, eyePosition + (forward * paintgun_fire_offset_y.GetFloat() / 4), MASK_ALL, pPlayer, COLLISION_GROUP_PLAYER_MOVEMENT, &tr);

	//if (!tr.DidHit())
		eyePosition += forward * paintgun_fire_offset_y.GetFloat();

	eyePosition += up * paintgun_fire_offset_z.GetFloat();
	eyePosition += right * paintgun_fire_offset_x.GetFloat();
	
	if (paintgun_fire_blobs.GetBool()) {
	
		float blobSpeed = RandomFloat(paintgun_blobs_min_speed.GetFloat(), paintgun_blobs_max_speed.GetFloat());
		Vector blobDir = forward;

		PaintPowerType blobType = erase ? NO_POWER : g_CurPaintgunPower;

		if (gpGlobals->curtime >= m_flNextFireTime) {

			Vector_RandomiseInCone(blobDir, paintgun_blobs_max_spread_angle.GetFloat());
			
			blobDir = blobDir.Normalized();
			blobDir *= RandomFloat(paintgun_blobs_min_speed.GetFloat(), paintgun_blobs_max_speed.GetFloat());

			PaintBlobManager()->CreateBlob(eyePosition, blobDir, blobType);

			m_flNextFireTime = gpGlobals->curtime + g_flFireDelay;
		}

	}
	//else {
	//	trace_t tr;
	//	UTIL_TraceLine(halfHeightOrigin, pPlayer->GetForward_Stick() * MAX_TRACE_LENGTH, MASK_ALL, pPlayer, COLLISION_GROUP_PLAYER_MOVEMENT, &tr);

	//	if (tr.DidHit()) {

	//		if (tr.DidHitWorld())
	//			if (!erase) {
	//				bool result = engine->SpherePaintSurface(tr.m_pEnt->GetModel(), tr.endpos, g_CurPaintgunPower, paintgun_rad.GetInt(), paintgun_strength.GetInt());
	//				Msg("Painted? %i\n", (int)result);
	//			}
	//			else
	//				engine->SpherePaintSurface(tr.m_pEnt->GetModel(), tr.endpos, NO_POWER, paintgun_rad.GetInt(), paintgun_strength.GetInt()); // erase		
	//	}
	//}
}

int CWeaponPaintgun::UpdateClientData(CBasePlayer* pPlayer) {
	m_nPaintType = g_CurPaintgunPower;

	return BaseClass::UpdateClientData(pPlayer);
}

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
			g_CurPaintgunPower = FIFTH_POWER;
			SetPaintDisplayColour(FIFTH_POWER);
			break;
		case FIFTH_POWER:
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
			g_CurPaintgunPower = FIFTH_POWER;
			SetPaintDisplayColour(FIFTH_POWER);
			break;
		case SPEED_POWER:
			g_CurPaintgunPower = BOUNCE_POWER;
			SetPaintDisplayColour(BOUNCE_POWER);
			break;
		case PORTAL_POWER:
			g_CurPaintgunPower = SPEED_POWER;
			SetPaintDisplayColour(SPEED_POWER);
			break;
		case FIFTH_POWER:
			g_CurPaintgunPower = PORTAL_POWER;
			SetPaintDisplayColour(PORTAL_POWER);
			break;
		default:
			g_CurPaintgunPower = BOUNCE_POWER;
			SetPaintDisplayColour(BOUNCE_POWER);
			break;
	}

	curPaintSwitchTime = gpGlobals->curtime + paintSwitchDelay;
}
