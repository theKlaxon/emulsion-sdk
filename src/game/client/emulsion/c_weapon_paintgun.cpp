#include "cbase.h"
#include "c_basecombatweapon.h"
#include "c_emulsion_player.h"

#define	HL2_BOB_CYCLE_MIN	1.0f
#define	HL2_BOB_CYCLE_MAX	0.45f
#define	HL2_BOB			0.002f
#define	HL2_BOB_UP		0.5f

static float	g_lateralBob;
static float	g_verticalBob;

#define	HL2_BOB_CYCLE_MIN	1.0f
#define	HL2_BOB_CYCLE_MAX	0.45f
#define	HL2_BOB			0.002f
#define	HL2_BOB_UP		0.5f

class C_WeaponPaintgun : public C_BaseCombatWeapon {
	DECLARE_CLASS(C_WeaponPaintgun, C_BaseCombatWeapon)
	DECLARE_NETWORKCLASS()
public:

	C_WeaponPaintgun();

	virtual void	AddViewmodelBob(CBaseViewModel* viewmodel, Vector& origin, QAngle& angles);
	virtual	float	CalcViewmodelBob();
	virtual bool	ShouldDrawCrosshair(void) { return true; }

private:

	float m_flVerticalBob;
	float m_flLateralBob;
};

//LINK_ENTITY_TO_CLASS(weapon_paintgun, C_WeaponPaintgun)

IMPLEMENT_CLIENTCLASS_DT(C_WeaponPaintgun, DT_WeaponPaintgun, CWeaponPaintgun)
END_RECV_TABLE()

C_WeaponPaintgun::C_WeaponPaintgun() {

}
float C_WeaponPaintgun::CalcViewmodelBob(void)
{
	static	float bobtime;
	static	float lastbobtime;
	float	cycle;

	CBasePlayer* player = ToBasePlayer(GetOwner());
	//Assert( player );

	//NOTENOTE: For now, let this cycle continue when in the air, because it snaps badly without it

	if ((!gpGlobals->frametime) || (player == NULL))
	{
		//NOTENOTE: We don't use this return value in our case (need to restructure the calculation function setup!)
		return 0.0f;// just use old value
	}

	// Note: we use paint code for this so when player move on speed paint, gun bob faster (Bank)
	//Find the speed of the player
	float speed = player->GetLocalVelocity().Length();

	speed = clamp(speed, -player->MaxSpeed(), player->MaxSpeed());

	float bob_offset = RemapVal(speed, 0, player->MaxSpeed(), 0.0f, 1.0f);

	bobtime += (gpGlobals->curtime - lastbobtime) * bob_offset;
	lastbobtime = gpGlobals->curtime;

	//Calculate the vertical bob
	cycle = bobtime - (int)(bobtime / HL2_BOB_CYCLE_MAX) * HL2_BOB_CYCLE_MAX;
	cycle /= HL2_BOB_CYCLE_MAX;

	if (cycle < HL2_BOB_UP)
	{
		cycle = M_PI * cycle / HL2_BOB_UP;
	}
	else
	{
		cycle = M_PI + M_PI * (cycle - HL2_BOB_UP) / (1.0 - HL2_BOB_UP);
	}

	g_verticalBob = speed * 0.005f;
	g_verticalBob = g_verticalBob * 0.3 + g_verticalBob * 0.7 * sin(cycle);

	g_verticalBob = clamp(g_verticalBob, -7.0f, 4.0f);

	//Calculate the lateral bob
	cycle = bobtime - (int)(bobtime / HL2_BOB_CYCLE_MAX * 2) * HL2_BOB_CYCLE_MAX * 2;
	cycle /= HL2_BOB_CYCLE_MAX * 2;

	if (cycle < HL2_BOB_UP)
	{
		cycle = M_PI * cycle / HL2_BOB_UP;
	}
	else
	{
		cycle = M_PI + M_PI * (cycle - HL2_BOB_UP) / (1.0 - HL2_BOB_UP);
	}

	g_lateralBob = speed * 0.005f;
	g_lateralBob = g_lateralBob * 0.3 + g_lateralBob * 0.7 * sin(cycle);
	g_lateralBob = clamp(g_lateralBob, -7.0f, 4.0f);

	//NOTENOTE: We don't use this return value in our case (need to restructure the calculation function setup!)
	return 0.0f;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : &origin - 
//			&angles - 
//			viewmodelindex - 
//-----------------------------------------------------------------------------
void C_WeaponPaintgun::AddViewmodelBob(CBaseViewModel* viewmodel, Vector& origin, QAngle& angles)
{
	Vector	forward, right, up;
	AngleVectors(angles, &forward, &right, &up);

	CalcViewmodelBob();

	// thanks Bank, but we're not in Kansas anymore
	// Note: we need to use paint code for gun bob so the gun bobs correctly when player sticks on walls (Bank)
	//C_Portal_Player* pPortalPlayer = ToPortalPlayer(GetOwner());
	//if (!pPortalPlayer)
	//	return;

	C_EmulsionPlayer* pPortalPlayer = (C_EmulsionPlayer*)(GetOwner());
	if (!pPortalPlayer)
		return;

	// Apply bob, but scaled down to 40%
	VectorMA(origin, g_verticalBob * 0.1f, forward, origin);

	// Z bob a bit more
	origin += g_verticalBob * 0.1f * pPortalPlayer->m_vecCurLerpUp;// pPortalPlayer->GetPortalPlayerLocalData().m_Up;

	//move left and right
	VectorMA(origin, g_lateralBob * 0.8f, right, origin);

	//roll, pitch, yaw
	float rollAngle = g_verticalBob * 0.5f;
	VMatrix rotMatrix;
	Vector rotAxis = CrossProduct(right, up).Normalized();

	MatrixBuildRotationAboutAxis(rotMatrix, rotAxis, rollAngle);
	up = rotMatrix * up;
	forward = rotMatrix * forward;
	right = rotMatrix * right;

	float pitchAngle = -g_verticalBob * 0.4f;
	rotAxis = right;
	MatrixBuildRotationAboutAxis(rotMatrix, rotAxis, pitchAngle);
	up = rotMatrix * up;
	forward = rotMatrix * forward;

	float yawAngle = -g_lateralBob * 0.3f;
	rotAxis = up;
	MatrixBuildRotationAboutAxis(rotMatrix, rotAxis, yawAngle);
	forward = rotMatrix * forward;

	VectorAngles(forward, up, angles);
}