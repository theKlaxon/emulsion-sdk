#include "cbase.h"
#include "c_basecombatweapon.h"
#include "c_emulsion_player.h"
#include "game/shared/portal2/paint_enum.h"

// -- paint colours
ConVar bounce_paint_color("bounce_paint_color", "0 165 255 255", FCVAR_REPLICATED);
ConVar speed_paint_color("speed_paint_color", "255 106 0 255", FCVAR_REPLICATED);
ConVar portal_paint_color("portal_paint_color", "0 200 33 255", FCVAR_REPLICATED); // p2ce's green (i was allowed to use it) -Klax
//ConVar portal_paint_color("portal_paint_color", "15 252 11 255", FCVAR_REPLICATED); // i like greemn -Klax

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
	virtual void	CalcViewModelLag(Vector& origin, QAngle& angles, QAngle& original_angles);
	//virtual void	CalcViewModelView(CBasePlayer* owner, const Vector& eyePosition, const QAngle& eyeAngles);
	virtual bool	ShouldDrawCrosshair(void) { return true; }

	void PostDataUpdate(DataUpdateType_t updateType);

private:

	Vector m_vecLastFacing;

	float m_flVerticalBob;
	float m_flLateralBob;

	int m_nPaintType;
};

IMPLEMENT_CLIENTCLASS_DT(C_WeaponPaintgun, DT_WeaponPaintgun, CWeaponPaintgun)
	RecvPropInt(RECVINFO(m_nPaintType)),
END_RECV_TABLE()

C_WeaponPaintgun::C_WeaponPaintgun() {

}

float C_WeaponPaintgun::CalcViewmodelBob(void)
{
	static	float bobtime;
	static	float lastbobtime;
	float	cycle;

	CBasePlayer* player = ToBasePlayer(GetOwner());
	
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

	C_EmulsionPlayer* pPortalPlayer = (C_EmulsionPlayer*)(GetOwner());
	if (!pPortalPlayer)
		return;

	// Apply bob, but scaled down to 40%
	VectorMA(origin, g_verticalBob * 0.1f, forward, origin);

	// Z bob a bit more
	origin += g_verticalBob * 0.1f * pPortalPlayer->m_vecCurLerpUp;

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
	
	// calc the lag
	QAngle originalang = EyeAngles();
	CalcViewModelLag(origin, angles, originalang);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
float g_fMaxViewModelLag = 1.5f;

void C_WeaponPaintgun::CalcViewModelLag(Vector& origin, QAngle& angles, QAngle& original_angles)
{
	Vector vOriginalOrigin = origin;
	QAngle vOriginalAngles = angles;

	// Calculate our drift
	Vector	forward;
	AngleVectors(angles, &forward, NULL, NULL);

	if (gpGlobals->frametime != 0.0f)
	{
		Vector vDifference;
		VectorSubtract(forward, m_vecLastFacing, vDifference);

		float flSpeed = 5.0f;

		// If we start to lag too far behind, we'll increase the "catch up" speed.  Solves the problem with fast cl_yawspeed, m_yaw or joysticks
		//  rotating quickly.  The old code would slam lastfacing with origin causing the viewmodel to pop to a new position
		float flDiff = vDifference.Length();
		if ((flDiff > g_fMaxViewModelLag) && (g_fMaxViewModelLag > 0.0f))
		{
			float flScale = flDiff / g_fMaxViewModelLag;
			flSpeed *= flScale;
		}

		// FIXME:  Needs to be predictable?
		VectorMA(m_vecLastFacing, flSpeed * gpGlobals->frametime, vDifference, m_vecLastFacing);
		// Make sure it doesn't grow out of control!!!
		VectorNormalize(m_vecLastFacing);
		VectorMA(origin, 5.0f, vDifference * -1.0f, origin);

		Assert(m_vecLastFacing.IsValid());
	}

	Vector right, up;
	AngleVectors(original_angles, &forward, &right, &up);

	float pitch = original_angles[PITCH];
	if (pitch > 180.0f)
		pitch -= 360.0f;
	else if (pitch < -180.0f)
		pitch += 360.0f;

	if (g_fMaxViewModelLag == 0.0f)
	{
		origin = vOriginalOrigin;
		angles = vOriginalAngles;
	}

	//FIXME: These are the old settings that caused too many exposed polys on some models
	VectorMA(origin, -pitch * 0.035f, forward, origin);
	VectorMA(origin, -pitch * 0.03f, right, origin);
	VectorMA(origin, -pitch * 0.02f, up, origin);
}

void C_WeaponPaintgun::PostDataUpdate(DataUpdateType_t updateType) {

	C_EmulsionPlayer* pPlayer = (C_EmulsionPlayer*)UTIL_PlayerByIndex(1);

	switch (m_nPaintType) {
	case BOUNCE_POWER:
		pPlayer->GetViewModel(0)->SetRenderColor(bounce_paint_color.GetColor().r(), bounce_paint_color.GetColor().g(), bounce_paint_color.GetColor().b());
		break;
	case SPEED_POWER:
		pPlayer->GetViewModel(0)->SetRenderColor(speed_paint_color.GetColor().r(), speed_paint_color.GetColor().g(), speed_paint_color.GetColor().b());
		break;
	case PORTAL_POWER:		
		pPlayer->GetViewModel(0)->SetRenderColor(portal_paint_color.GetColor().r(), portal_paint_color.GetColor().g(), portal_paint_color.GetColor().b());
		break;
	default:
		break;
	}

	BaseClass::PostDataUpdate(updateType);
}