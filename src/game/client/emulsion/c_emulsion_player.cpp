#include "cbase.h"
#include "c_emulsion_player.h"
#include "prediction.h"
#include "iviewrender.h"
#include "ivieweffects.h"
#include "emulsion_shareddefs.h"
#include "emulsion_gamemovement.h"
#include "igamemovement.h"

extern CPrediction* prediction;
extern IViewRender* view;

LINK_ENTITY_TO_CLASS(player, C_EmulsionPlayer);

IMPLEMENT_CLIENTCLASS_DT(C_EmulsionPlayer, DT_EmulsionPlayer, CEmulsionPlayer)
	RecvPropVector(RECVINFO(m_vecVelocity)),
	RecvPropInt(RECVINFO(m_iPaintPower))
END_RECV_TABLE()

C_EmulsionPlayer::C_EmulsionPlayer() {
	//m_vecGravity = Vector(0, 0, -1);
	//m_vecPrevGravity = Vector(0, 0, -1);
}

extern IGameMovement* g_pGameMovement;

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

		// Tilting handled in CInput::AdjustAngles
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
	
	if (m_iPaintPower == PORTAL_POWER) {
		
		CEmulsionGameMovement* pMove = (CEmulsionGameMovement*)g_pGameMovement;
		Vector vecAxisOfRotation = CrossProduct(Vector(0, 0, 1), -1 * pMove->m_vecGravity);
		float fAngleOfRotation = RAD2DEG(acos(DotProduct(Vector(0, 0, 1), -1 * pMove->m_vecGravity)));

		Msg("(%f, %f, %f)\n", pMove->m_vecGravity.x, pMove->m_vecGravity.y, pMove->m_vecGravity.z);

		VMatrix eyerotmat;
		MatrixBuildRotationAboutAxis(eyerotmat, vecAxisOfRotation, fAngleOfRotation);

		Vector vecForward, vecUp;
		AngleVectors(eyeAngles, &vecForward, nullptr, &vecUp);
		VectorRotate(vecForward, eyerotmat.As3x4(), vecForward);
		VectorRotate(vecUp, eyerotmat.As3x4(), vecUp);

		VectorAngles(vecForward, vecUp, eyeAngles);
	}
}

/*	if (m_iPaintPower == PORTAL_POWER) {

		Vector axrot = CrossProduct(Vector(0, 0, 1), Vector(0, -1, 0));
		float angrot = RAD2DEG(acos(DotProduct(Vector(0, 0, 1), Vector(0, -1, 0))));

		VMatrix eyerotmat;
		MatrixBuildRotation(eyerotmat, Vector(0, 0, 1), Vector(0, -1, 0));

		QAngle nEyeAngles;
		MatrixAngles(eyerotmat.As3x4(), nEyeAngles);

		eyeAngles = nEyeAngles;
	}*/