#include "cbase.h"
#include "basecombatweapon.h"
#include "player.h"
#include "in_buttons.h"
#include "weapon_paintgun.h"
#include "emulsion_player.h"
#include "..\public\game\shared\portal2\paint_enum.h"
#include "paintblob_manager.h"

// -- paint colours
ConVar bounce_paint_color("bounce_paint_color", "0 165 255 255", FCVAR_REPLICATED);
ConVar speed_paint_color("speed_paint_color", "255 106 0 255", FCVAR_REPLICATED);
ConVar portal_paint_color("portal_paint_color", "0 200 33 255", FCVAR_REPLICATED); // p2ce's green (i was allowed to use it) -Klax
//ConVar portal_paint_color("portal_paint_color", "d15 252 11 255", FCVAR_REPLICATED); // i like greemn -Klax
//ConVar portal_paint_color("portal_paint_color", "140 0 255 255", FCVAR_REPLICATED); // using this for stick paint

ConVar erase_color("erase_color", "0 0 0 255", FCVAR_REPLICATED);
ConVar erase_visual_color("erase_visual_color", "0 0 0 255", FCVAR_REPLICATED);
ConVar paint_color_max_diff("paint_color_max_diff", "32", FCVAR_REPLICATED, "The maximum difference between two colors for matching.");
// ---

ConVar paintgun_fire_blobs("paintgun_fire_blobs", "1", FCVAR_NOTIFY, "Blobs are experimental!");

// Paintgun power commands
void Paintgun_NextPower();
void Paintgun_PrevPower();

CWeaponPaintgun* g_playerPaintgun = nullptr;
PaintPowerType g_CurPaintgunPower = BOUNCE_POWER;

ConCommand paintgun_next("paintgun_next", Paintgun_NextPower);
ConCommand paintgun_prev("paintgun_prev", Paintgun_PrevPower);

ConVar paintgun_rad("paintgun_rad", "64", FCVAR_REPLICATED);
ConVar paintgun_strength("paintgun_strength", "5", FCVAR_REPLICATED);
ConVar paintgun_timing("paintgun_timing", "0.00125f", FCVAR_REPLICATED, "0.015 is the MINIMUM you should go.");
ConVar paintgun_cone("paintgun_cone", "15.0f", FCVAR_REPLICATED, "Paintblob streak cone in degrees");
ConVar paintgun_fire_offset_z("paintgun_fire_offset_z", "-20.0f", FCVAR_REPLICATED);
ConVar paintgun_fire_offset_x("paintgun_fire_offset_x", "60.5f", FCVAR_REPLICATED);

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
	m_flCurPaintDelay = 0.0f;
	//g_playerPaintgun = this;
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
}

void CWeaponPaintgun::Drop(const Vector& vecVelocity) {
	BaseClass::Drop(vecVelocity);
	//g_playerPaintgun = nullptr;
}

int GetStreamIndex(PaintPowerType type) {
	return PaintBlobManager()->GetStreamIndex(type);
}

struct BlobPattern_t {

	BlobPattern_t(float velMod, Vector offsets, float radius, int count = 1) {
		m_flVelMod = velMod;

		m_vecOffsets = offsets;
		m_flRadius = radius;
		m_nCount = count;
	}

	float m_flVelMod;
	Vector m_vecOffsets;
	float m_flRadius;
	int m_nCount;
};

#define BLOB_PATTERN_COUNT 6
//float g_flOffsets0[3] = { 50, 0, 75 };
//float g_flOffsets1[2] = { 0, 40 };
//float g_flOffsets2[1] = { 10 };
//float g_flOffsets3[2] = { 5, 60 };
//float g_flOffsets4[3] = { 0, 50, 30 };
//float g_flOffsets5[2] = { 45, 0 };

Vector g_vecOffsets0 = Vector(0, 5, 0);
Vector g_vecOffsets1 = Vector(3, 0, 2);
Vector g_vecOffsets2 = Vector(0, 1, 3);
Vector g_vecOffsets3 = Vector(2, 4, 3);
Vector g_vecOffsets4 = Vector(0, 5, 1);
Vector g_vecOffsets5 = Vector(1, 2, 2);

//BlobPattern_t g_BlobPatterns[BLOB_PATTERN_COUNT] = {
//	BlobPattern_t(1, g_flOffsets0, 0.5f),
//	BlobPattern_t(2, g_flOffsets1, 0.4f),
//	BlobPattern_t(2, g_flOffsets2, 0.6f),
//	BlobPattern_t(1, g_flOffsets3, 0.8f),
//	BlobPattern_t(2, g_flOffsets4, 0.6f),
//	BlobPattern_t(1, g_flOffsets5, 0.3f)
//};

BlobPattern_t g_BlobPatterns[BLOB_PATTERN_COUNT] = {
	BlobPattern_t(50, g_vecOffsets0, 0.5f, 1),
	BlobPattern_t(-35, g_vecOffsets1, 0.4f, 2),
	BlobPattern_t(100, g_vecOffsets2, 0.6f, 3),
	BlobPattern_t(-75, g_vecOffsets3, 0.8f, 2),
	BlobPattern_t(75, g_vecOffsets4, 0.6f, 1),
	BlobPattern_t(-10, g_vecOffsets5, 0.3f, 3)
};


int g_nBlobCycle = 0;

void CWeaponPaintgun::FirePaint(bool erase) {

	//if (gpGlobals->curtime < m_flCurPaintDelay)
	//	return;

	CEmulsionPlayer* pPlayer = ToEmulsionPlayer(UTIL_PlayerByIndex(1));
	Vector halfHeightOrigin = pPlayer->GetHalfHeight_Stick();

	Vector forward, right, up;
	AngleVectors(pPlayer->StickEyeAngles(), &forward, &right, &up);

	Vector eyePosition = pPlayer->StickEyeOrigin();
	eyePosition += Vector(0, 0, 1) * paintgun_fire_offset_z.GetFloat();

	if (paintgun_fire_blobs.GetBool()) {

		if (g_nBlobCycle >= BLOB_PATTERN_COUNT)
			g_nBlobCycle = 0;

		Vector blobVel = forward * (1000.0f + g_BlobPatterns[g_nBlobCycle].m_flVelMod);
		PaintPowerType type = erase ? NO_POWER : g_CurPaintgunPower;

		Vector offset = eyePosition + (g_BlobPatterns[g_nBlobCycle].m_vecOffsets * (forward + right + up));

		for (int i = 0; i < g_BlobPatterns[g_nBlobCycle].m_nCount; i++) {
			float randfor = RandomFloat(0.0f, 25.0f);
			float randri = RandomFloat(-10.0f, 10.0f);
			PaintBlobManager()->CreateBlob(offset + ((forward * (randfor + paintgun_fire_offset_x.GetFloat()))) + (right * randri), blobVel, GetStreamIndex(type), g_BlobPatterns[g_nBlobCycle].m_flVelMod);
		}
		
		g_nBlobCycle++;
	}
	else {
		trace_t tr;
		UTIL_TraceLine(halfHeightOrigin, pPlayer->GetForward_Stick() * MAX_TRACE_LENGTH, MASK_ALL, pPlayer, COLLISION_GROUP_PLAYER_MOVEMENT, &tr);

		if (tr.DidHit()) {

			if (tr.DidHitWorld())
				if (!erase) {
					engine->SpherePaintSurface(tr.m_pEnt->GetModel(), tr.endpos, g_CurPaintgunPower, paintgun_rad.GetInt(), paintgun_strength.GetInt());
				}
				else
					engine->SpherePaintSurface(tr.m_pEnt->GetModel(), tr.endpos, NO_POWER, paintgun_rad.GetInt(), paintgun_strength.GetInt()); // erase		
		}
	}

	//m_flCurPaintDelay = gpGlobals->curtime + paintgun_timing.GetFloat();
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
