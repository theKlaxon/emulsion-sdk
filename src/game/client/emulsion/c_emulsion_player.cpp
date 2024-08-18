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
RecvPropVector(RECVINFO(m_vecPrevUp)),
RecvPropVector(RECVINFO(m_vecStickRight)),
RecvPropVector(RECVINFO(m_vecSurfaceForward)),
RecvPropVector(RECVINFO(m_vecPrevOrigin)),
RecvPropVector(RECVINFO(m_vecUp)),
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

	// decompose the angle and get it's directions
	Vector vecForward, vecUp, vecRight;
	AngleVectors(eyeAngles, &vecForward, &vecRight, &vecUp);

	Vector absForward, absUp, absRight;
	AngleVectors(eyeAngles, &vecForward, &vecRight, &vecUp);



	// stick camera calculation
	{
		Vector newForward = m_vecPrevUp - ( ( DotProduct(m_vecPrevUp, m_vecUp) / DotProduct(m_vecUp, m_vecUp) ) * m_vecUp );
		
		QAngle newAng;
		VectorAngles(newForward.Normalized(), newAng);

		// new right
		Vector newRight = CrossProduct(newForward, m_vecUp);

		float angforward = acos(DotProduct(vecForward, -1 * m_vecUp));
		float angup = acos(DotProduct(vecForward, m_vecPrevUp));

		matrix3x4_t omat;
		omat.m_flMatVal[0][0] = newRight.x; omat.m_flMatVal[0][1] = m_vecUp.x; omat.m_flMatVal[0][2] = newForward.x; omat.m_flMatVal[0][3] = eyeOrigin.x;
		omat.m_flMatVal[1][0] = newRight.y; omat.m_flMatVal[1][1] = m_vecUp.y; omat.m_flMatVal[1][2] = newForward.y; omat.m_flMatVal[1][3] = eyeOrigin.y;
		omat.m_flMatVal[2][0] = newRight.z; omat.m_flMatVal[2][1] = m_vecUp.z; omat.m_flMatVal[2][2] = newForward.z; omat.m_flMatVal[2][3] = eyeOrigin.z;

		

		// ignore this (unrelated to the cam math above),
		// fixes inverted mouse while on the ceiling
		GetPaintInput()->SetUseStickMouseFix(m_vecGravity == g_RoofStickNormal);

		// get the camera offset using the new up vector as the offset direction
		float totalOffset = m_vecViewOffset.Length();
		eyeOrigin = GetAbsOrigin() + (m_vecUp * totalOffset);
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

/*
	{
		float flAngleOfRotation = RAD2DEG( acos( DotProduct(m_vecUp, m_vecPrevUp) ) );	// get the amount we need to rotate

		Vector axof = CrossProduct(m_vecPrevUp, m_vecUp);
		VMatrix anglemat; // the matrix getting built below
		MatrixBuildRotationAboutAxis(anglemat, axof, flAngleOfRotation); // build the rotation mat around
																					 // an axis perpendicular to the direction we're rotating

		VectorRotate(vecForward, anglemat.As3x4(), vecForward); // rotate the forward vector
		VectorRotate(vecUp, anglemat.As3x4(), vecUp);			// rotate the up vector
		VectorAngles(vecForward, vecUp, eyeAngles);				// create the new eyeAngles (camera view direction) from forward and up

		//if (flAngleOfRotation > 0.0f) {
		//	Msg("vecForward: (%f, %f, %f)\n", vecForward.x, vecForward.y, vecForward.z);
		//	Msg("m_vecStickRight: (%f, %f, %f)\n", m_vecStickRight.x, m_vecStickRight.y, m_vecStickRight.z);
		//}

		// ignore this (unrelated to the cam math above),
		// fixes inverted mouse while on the ceiling
		GetPaintInput()->SetUseStickMouseFix(m_vecGravity == g_RoofStickNormal);

		// get the camera offset using the new up vector as the offset direction
		float totalOffset = m_vecViewOffset.Length();
		eyeOrigin = GetAbsOrigin() + ( m_vecUp * totalOffset );
	}
*/

/*
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
}*/