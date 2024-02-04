#include "cbase.h"
#include "c_emulsion_player.h"
#include "prediction.h"
#include "iviewrender.h"
#include "ivieweffects.h"
#include "emulsion_shareddefs.h"
#include "emulsion_gamemovement.h"
#include "emulsion_in_main.h"
#include "igamemovement.h"

static ConVar pl_stickcameralerpspeed("pl_stickcameralerpspeed", "3.0f", FCVAR_REPLICATED | FCVAR_CHEAT | FCVAR_NOTIFY);
static ConVar pl_stickcamerauselerp("pl_stickcamerauselerp", "1", FCVAR_REPLICATED | FCVAR_CHEAT | FCVAR_NOTIFY);

extern CPrediction* prediction;
extern IViewRender* view;

LINK_ENTITY_TO_CLASS(player, C_EmulsionPlayer);

IMPLEMENT_CLIENTCLASS_DT(C_EmulsionPlayer, DT_EmulsionPlayer, CEmulsionPlayer)
RecvPropVector(RECVINFO(m_vecVelocity)),
RecvPropVector(RECVINFO(m_vecGravity)),
RecvPropEHandle(RECVINFO(m_hStickParent)),
RecvPropInt(RECVINFO(m_nPaintPower))
END_RECV_TABLE()

C_EmulsionPlayer::C_EmulsionPlayer() {
	m_vecCurLerpUp = Vector(0, 0, 1);
}

extern IGameMovement* g_pGameMovement;

static Vector g_RoofStickNormal = Vector(0, 0, 1);

void C_EmulsionPlayer::CalcPlayerView(Vector& eyeOrigin, QAngle& eyeAngles, float& fov)
{
#if defined( CLIENT_DLL )
	if (!prediction->InPrediction())
	{
		// FIXME: Move into prediction
		view->DriftPitch();
	}
#endif

	// TrackIR
	if (IsHeadTrackingEnabled())
	{
		VectorCopy(EyePosition() + GetEyeOffset(), eyeOrigin);
		VectorCopy(EyeAngles() + GetEyeAngleOffset(), eyeAngles);
	}
	else
	{
		VectorCopy(EyePosition(), eyeOrigin);
		VectorCopy(EyeAngles(), eyeAngles);
	}


#if defined( CLIENT_DLL )
	if (!prediction->InPrediction())
#endif
	{
		SmoothViewOnStairs(eyeOrigin);
	}

	// Snack off the origin before bob + water offset are applied
	Vector vecBaseEyePosition = eyeOrigin;
	QAngle baseEyeAngles = eyeAngles;

	if (pl_stickcamerauselerp.GetBool()) {
		m_vecCurLerpUp = Lerp<Vector>((gpGlobals->frametime * pl_stickcameralerpspeed.GetFloat()), m_vecCurLerpUp, -1 * m_vecGravity);
		if (m_vecGravity == g_RoofStickNormal)
			GetPaintInput()->SetUseStickMouseFix(true);
		else
			GetPaintInput()->SetUseStickMouseFix(false);
	}

	CEmulsionGameMovement* pMove = (CEmulsionGameMovement*)g_pGameMovement;
	{
		Vector initial = Vector(0, 0, 1);
		Vector axOf = pl_stickcamerauselerp.GetBool() ? m_vecCurLerpUp : -1 * m_vecGravity;

		Vector vecAxisOfRotation = CrossProduct(initial, axOf); // build around out new up vector
		float flAngleOfRotation = RAD2DEG(acos(DotProduct(initial, axOf))); // build around out new up vector

		// build rotation matrix for new eye angles
		VMatrix eyerotmat;
		MatrixBuildRotationAboutAxis(eyerotmat, vecAxisOfRotation, flAngleOfRotation);

		// rotate the eyeAngles we already have by the new matrix
		Vector vecForward, vecUp;
		AngleVectors(eyeAngles, &vecForward, nullptr, &vecUp);
		VectorRotate(vecForward, eyerotmat.As3x4(), vecForward);
		VectorRotate(vecUp, eyerotmat.As3x4(), vecUp);

		// set the eyeAngles to our new result
		VectorAngles(vecForward, vecUp, eyeAngles);

		Vector vecRotOffset = m_vecViewOffset;
		VectorRotate(vecRotOffset, eyerotmat.As3x4(), vecRotOffset);

		// take the normal view offset and apply it to a new axis
		float totalOffset = m_vecViewOffset.Length();
		eyeOrigin = GetAbsOrigin() + ((pl_stickcamerauselerp.GetBool() ? m_vecCurLerpUp.Normalized() : -1 * m_vecGravity) * totalOffset);
	}

	CalcViewBob(eyeOrigin);
	CalcViewRoll(eyeAngles);

	// Apply punch angle
	VectorAdd(eyeAngles, m_Local.m_vecPunchAngle, eyeAngles);

#if defined( CLIENT_DLL )
	if (!prediction->InPrediction())
	{
		// Shake it up baby!
		GetViewEffects()->CalcShake();
		GetViewEffects()->ApplyShake(eyeOrigin, eyeAngles, 1.0);
	}
#endif

#if defined( CLIENT_DLL )
	// Apply a smoothing offset to smooth out prediction errors.
	Vector vSmoothOffset;
	GetPredictionErrorSmoothingVector(vSmoothOffset);
	eyeOrigin += vSmoothOffset;
	m_flObserverChaseDistance = 0.0;
#endif

	// calc current FOV
	fov = GetFOV();
}

CBaseEntity* C_EmulsionPlayer::GetStickParent() {
	return EntityFromEntityHandle(m_hStickParent->Get());
}