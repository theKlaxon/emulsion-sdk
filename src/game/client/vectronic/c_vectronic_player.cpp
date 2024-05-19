//=========== Copyright © 2014, rHetorical, All rights reserved. =============
//
// Purpose: 
//		
//=============================================================================

#include "cbase.h"
#include "c_vectronic_player.h"
//#include "c_basevectroniccombatweapon_shared.h"
#include "playerandobjectenumerator.h"
#include "engine/ivdebugoverlay.h"
#include "c_ai_basenpc.h"
#include "in_buttons.h"
#include "collisionutils.h"

#define AVOID_SPEED 2000.0f
extern ConVar cl_forwardspeed;
extern ConVar cl_backspeed;
extern ConVar cl_sidespeed;

#define DEATH_CC_LOOKUP_FILENAME "scripts/colorcorrection/cc_death.raw"
#define DEATH_CC_FADE_SPEED 0.05f

ConVar cl_npc_speedmod_intime( "cl_npc_speedmod_intime", "0.25", FCVAR_CLIENTDLL | FCVAR_ARCHIVE );
ConVar cl_npc_speedmod_outtime( "cl_npc_speedmod_outtime", "1.5", FCVAR_CLIENTDLL | FCVAR_ARCHIVE );

LINK_ENTITY_TO_CLASS( player, C_VectronicPlayer );

IMPLEMENT_CLIENTCLASS_DT(C_VectronicPlayer, DT_VectronicPlayer, CVectronicPlayer)
	RecvPropBool(  RECVINFO(m_bPlayerPickedUpObject) ),
	RecvPropInt( RECVINFO( m_iShotsFired ) ),
END_RECV_TABLE()

BEGIN_PREDICTION_DATA( C_VectronicPlayer )
	DEFINE_PRED_FIELD( m_flCycle, FIELD_FLOAT, FTYPEDESC_OVERRIDE | FTYPEDESC_PRIVATE | FTYPEDESC_NOERRORCHECK ),
	DEFINE_PRED_FIELD( m_nSequence, FIELD_INTEGER, FTYPEDESC_OVERRIDE | FTYPEDESC_PRIVATE | FTYPEDESC_NOERRORCHECK ),
	DEFINE_PRED_FIELD( m_flPlaybackRate, FIELD_FLOAT, FTYPEDESC_OVERRIDE | FTYPEDESC_PRIVATE | FTYPEDESC_NOERRORCHECK ),
	DEFINE_PRED_FIELD( m_nNewSequenceParity, FIELD_INTEGER, FTYPEDESC_OVERRIDE | FTYPEDESC_PRIVATE | FTYPEDESC_NOERRORCHECK ),
END_PREDICTION_DATA()

C_VectronicPlayer::C_VectronicPlayer()
{
	AddVar( &m_Local.m_vecPunchAngle, &m_Local.m_iv_vecPunchAngle, LATCH_SIMULATION_VAR );
	AddVar( &m_Local.m_vecPunchAngleVel, &m_Local.m_iv_vecPunchAngleVel, LATCH_SIMULATION_VAR );

	m_flSpeedMod		= cl_forwardspeed.GetFloat();

	ConVarRef scissor( "r_flashlightscissor" );
	scissor.SetValue( "0" );

	m_CCDeathHandle = INVALID_CLIENT_CCHANDLE;
}

C_VectronicPlayer::~C_VectronicPlayer( void )
{
	g_pColorCorrectionMgr->RemoveColorCorrection( m_CCDeathHandle );
}

void C_VectronicPlayer::ClientThink( void )
{
	// If dead, fade in death CC lookup
	if ( m_CCDeathHandle != INVALID_CLIENT_CCHANDLE )
	{
		if ( m_lifeState != LIFE_ALIVE )
		{
			if ( m_flDeathCCWeight < 1.0f )
			{
				m_flDeathCCWeight += DEATH_CC_FADE_SPEED;
				clamp( m_flDeathCCWeight, 0.0f, 1.0f );
			}
		}
		else 
		{
			m_flDeathCCWeight = 0.0f;
		}
		g_pColorCorrectionMgr->SetColorCorrectionWeight( m_CCDeathHandle, m_flDeathCCWeight );
	}

	BaseClass::ClientThink();
}

void C_VectronicPlayer::OnDataChanged( DataUpdateType_t type )
{
	if ( type == DATA_UPDATE_CREATED )
	{
		// Load color correction lookup for the death effect
		m_CCDeathHandle = g_pColorCorrectionMgr->AddColorCorrection( DEATH_CC_LOOKUP_FILENAME );
		if ( m_CCDeathHandle != INVALID_CLIENT_CCHANDLE )
		{
			g_pColorCorrectionMgr->SetColorCorrectionWeight( m_CCDeathHandle, 0.0f );
		}

		SetNextClientThink( CLIENT_THINK_ALWAYS );
	}

	BaseClass::OnDataChanged( type );
}

void C_VectronicPlayer::UpdateClientSideAnimation()
{
	int curSeq = GetSequence();
	
	Vector vel = GetLocalVelocity();
	//EstimateAbsVelocity( vel );

	int goalSeq = curSeq;

	if ( vel.LengthSqr() > 4 )
	{
		QAngle velAng;
		VectorAngles( vel, velAng );

		goalSeq = SelectWeightedSequence( ACT_RUN );

		float speed = vel.Length2D();
		float yaw = AngleNormalize( -(GetRenderAngles().y - velAng.y) );
		float seqspeed = 150.0f;
		float rate = speed / seqspeed;

		SetPoseParameter( LookupPoseParameter( "move_x" ), cos( DEG2RAD( yaw ) ) * rate );
		SetPoseParameter( LookupPoseParameter( "move_y" ), -sin( DEG2RAD( yaw ) ) * rate );

		SetPlaybackRate( clamp( rate * 0.6f, 1, 1.5f ) );
	}
	else
		goalSeq = SelectWeightedSequence( ACT_IDLE );

	if ( curSeq != goalSeq )
	{
		ResetSequence( goalSeq );
	}

	//m_flAnimTime = gpGlobals->curtime;
	//StudioFrameAdvance();

	if ( GetCycle() >= 1.0f )
		SetCycle( GetCycle() - 1.0f );
}

const QAngle &C_VectronicPlayer::GetRenderAngles()
{
	m_angRender = GetLocalAngles();
	m_angRender.x = 0;

	return m_angRender;
}

//-----------------------------------------------------------------------------
// Purpose: Determines if a player can be safely moved towards a point
// Input:   pos - position to test move to, fVertDist - how far to trace downwards to see if the player would fall,
//			radius - how close the player can be to the object, objPos - position of the object to avoid,
//			objDir - direction the object is travelling
//-----------------------------------------------------------------------------
bool C_VectronicPlayer::TestMove( const Vector &pos, float fVertDist, float radius, const Vector &objPos, const Vector &objDir )
{
	trace_t trUp;
	trace_t trOver;
	trace_t trDown;
	float flHit1, flHit2;
	
	UTIL_TraceHull( GetAbsOrigin(), pos, GetPlayerMins(), GetPlayerMaxs(), MASK_SOLID_BRUSHONLY, this, COLLISION_GROUP_NONE, &trOver );
	if ( trOver.fraction < 1.0f )
	{
		// check if the endpos intersects with the direction the object is travelling.  if it doesn't, this is a good direction to move.
		if ( objDir.IsZero() ||
			( IntersectInfiniteRayWithSphere( objPos, objDir, trOver.endpos, radius, &flHit1, &flHit2 ) && 
			( ( flHit1 >= 0.0f ) || ( flHit2 >= 0.0f ) ) )
			)
		{
			// our first trace failed, so see if we can go farther if we step up.

			// trace up to see if we have enough room.
			UTIL_TraceHull( GetAbsOrigin(), GetAbsOrigin() + Vector( 0, 0, m_Local.m_flStepSize ), 
				GetPlayerMins(), GetPlayerMaxs(), MASK_SOLID_BRUSHONLY, this, COLLISION_GROUP_NONE, &trUp );

			// do a trace from the stepped up height
			UTIL_TraceHull( trUp.endpos, pos + Vector( 0, 0, trUp.endpos.z - trUp.startpos.z ), 
				GetPlayerMins(), GetPlayerMaxs(), MASK_SOLID_BRUSHONLY, this, COLLISION_GROUP_NONE, &trOver );

			if ( trOver.fraction < 1.0f )
			{
				// check if the endpos intersects with the direction the object is travelling.  if it doesn't, this is a good direction to move.
				if ( objDir.IsZero() ||
					( IntersectInfiniteRayWithSphere( objPos, objDir, trOver.endpos, radius, &flHit1, &flHit2 ) && ( ( flHit1 >= 0.0f ) || ( flHit2 >= 0.0f ) ) ) )
				{
					return false;
				}
			}
		}
	}

	// trace down to see if this position is on the ground
	UTIL_TraceLine( trOver.endpos, trOver.endpos - Vector( 0, 0, fVertDist ), 
		MASK_SOLID_BRUSHONLY, NULL, COLLISION_GROUP_NONE, &trDown );

	if ( trDown.fraction == 1.0f ) 
		return false;

	return true;
}

//-----------------------------------------------------------------------------
// Client-side obstacle avoidance
//-----------------------------------------------------------------------------
void C_VectronicPlayer::PerformClientSideObstacleAvoidance( float flFrameTime, CUserCmd *pCmd )
{
	// Don't avoid if noclipping or in movetype none
	switch ( GetMoveType() )
	{
	case MOVETYPE_NOCLIP:
	case MOVETYPE_NONE:
	case MOVETYPE_OBSERVER:
		return;
	default:
		break;
	}

	// Try to steer away from any objects/players we might interpenetrate
	Vector size = WorldAlignSize();

	float radius = 0.7f * sqrt( size.x * size.x + size.y * size.y );
	float curspeed = GetLocalVelocity().Length2D();

	//int slot = 1;
	//engine->Con_NPrintf( slot++, "speed %f\n", curspeed );
	//engine->Con_NPrintf( slot++, "radius %f\n", radius );

	// If running, use a larger radius
	float factor = 1.0f;

	if ( curspeed > 150.0f )
	{
		curspeed = MIN( 2048.0f, curspeed );
		factor = ( 1.0f + ( curspeed - 150.0f ) / 150.0f );

		//engine->Con_NPrintf( slot++, "scaleup (%f) to radius %f\n", factor, radius * factor );

		radius = radius * factor;
	}

	Vector currentdir;
	Vector rightdir;

	QAngle vAngles = pCmd->viewangles;
	vAngles.x = 0;

	AngleVectors( vAngles, &currentdir, &rightdir, NULL );
		
	bool istryingtomove = false;
	bool ismovingforward = false;
	if ( fabs( pCmd->forwardmove ) > 0.0f || 
		fabs( pCmd->sidemove ) > 0.0f )
	{
		istryingtomove = true;
		if ( pCmd->forwardmove > 1.0f )
		{
			ismovingforward = true;
		}
	}

	if ( istryingtomove == true )
		 radius *= 1.3f;

	CPlayerAndObjectEnumerator avoid( radius );
	partition->EnumerateElementsInSphere( PARTITION_CLIENT_SOLID_EDICTS, GetAbsOrigin(), radius, false, &avoid );

	// Okay, decide how to avoid if there's anything close by
	int c = avoid.GetObjectCount();
	if ( c <= 0 )
		return;

	//engine->Con_NPrintf( slot++, "moving %s forward %s\n", istryingtomove ? "true" : "false", ismovingforward ? "true" : "false"  );

	float adjustforwardmove = 0.0f;
	float adjustsidemove	= 0.0f;

	for ( int i = 0; i < c; i++ )
	{
		C_AI_BaseNPC *obj = dynamic_cast< C_AI_BaseNPC *>(avoid.GetObject( i ));

		if( !obj )
			continue;

		Vector vecToObject = obj->GetAbsOrigin() - GetAbsOrigin();

		float flDist = vecToObject.Length2D();
		
		// Figure out a 2D radius for the object
		Vector vecWorldMins, vecWorldMaxs;
		obj->CollisionProp()->WorldSpaceAABB( &vecWorldMins, &vecWorldMaxs );
		Vector objSize = vecWorldMaxs - vecWorldMins;

		float objectradius = 0.5f * sqrt( objSize.x * objSize.x + objSize.y * objSize.y );

		//Don't run this code if the NPC is not moving UNLESS we are in stuck inside of them.
		if ( !obj->IsMoving() && flDist > objectradius )
			  continue;

		if ( flDist > objectradius && obj->IsEffectActive( EF_NODRAW ) )
		{
			obj->RemoveEffects( EF_NODRAW );
		}

		Vector vecNPCVelocity;
		obj->EstimateAbsVelocity( vecNPCVelocity );
		float flNPCSpeed = VectorNormalize( vecNPCVelocity );

		Vector vPlayerVel = GetAbsVelocity();
		VectorNormalize( vPlayerVel );

		float flHit1, flHit2;
		Vector vRayDir = vecToObject;
		VectorNormalize( vRayDir );

		float flVelProduct = DotProduct( vecNPCVelocity, vPlayerVel );
		float flDirProduct = DotProduct( vRayDir, vPlayerVel );

		if ( !IntersectInfiniteRayWithSphere(
				GetAbsOrigin(),
				vRayDir,
				obj->GetAbsOrigin(),
				radius,
				&flHit1,
				&flHit2 ) )
			continue;

        Vector dirToObject = -vecToObject;
		VectorNormalize( dirToObject );

		float fwd = 0;
		float rt = 0;

		float sidescale = 2.0f;
		float forwardscale = 1.0f;
		bool foundResult = false;

		Vector vMoveDir = vecNPCVelocity;
		if ( flNPCSpeed > 0.001f )
		{
			// This NPC is moving. First try deflecting the player left or right relative to the NPC's velocity.
			// Start with whatever side they're on relative to the NPC's velocity.
			Vector vecNPCTrajectoryRight = CrossProduct( vecNPCVelocity, Vector( 0, 0, 1) );
			int iDirection = ( vecNPCTrajectoryRight.Dot( dirToObject ) > 0 ) ? 1 : -1;
			for ( int nTries = 0; nTries < 2; nTries++ )
			{
				Vector vecTryMove = vecNPCTrajectoryRight * iDirection;
				VectorNormalize( vecTryMove );
				
				Vector vTestPosition = GetAbsOrigin() + vecTryMove * radius * 2;

				if ( TestMove( vTestPosition, size.z * 2, radius * 2, obj->GetAbsOrigin(), vMoveDir ) )
				{
					fwd = currentdir.Dot( vecTryMove );
					rt = rightdir.Dot( vecTryMove );
					
					//Msg( "PUSH DEFLECT fwd=%f, rt=%f\n", fwd, rt );
					
					foundResult = true;
					break;
				}
				else
				{
					// Try the other direction.
					iDirection *= -1;
				}
			}
		}
		else
		{
			// the object isn't moving, so try moving opposite the way it's facing
			Vector vecNPCForward;
			obj->GetVectors( &vecNPCForward, NULL, NULL );
			
			Vector vTestPosition = GetAbsOrigin() - vecNPCForward * radius * 2;
			if ( TestMove( vTestPosition, size.z * 2, radius * 2, obj->GetAbsOrigin(), vMoveDir ) )
			{
				fwd = currentdir.Dot( -vecNPCForward );
				rt = rightdir.Dot( -vecNPCForward );

				if ( flDist < objectradius )
				{
					obj->AddEffects( EF_NODRAW );
				}

				//Msg( "PUSH AWAY FACE fwd=%f, rt=%f\n", fwd, rt );

				foundResult = true;
			}
		}

		if ( !foundResult )
		{
			// test if we can move in the direction the object is moving
			Vector vTestPosition = GetAbsOrigin() + vMoveDir * radius * 2;
			if ( TestMove( vTestPosition, size.z * 2, radius * 2, obj->GetAbsOrigin(), vMoveDir ) )
			{
				fwd = currentdir.Dot( vMoveDir );
				rt = rightdir.Dot( vMoveDir );

				if ( flDist < objectradius )
				{
					obj->AddEffects( EF_NODRAW );
				}

				//Msg( "PUSH ALONG fwd=%f, rt=%f\n", fwd, rt );

				foundResult = true;
			}
			else
			{
				// try moving directly away from the object
				Vector vTestPosition = GetAbsOrigin() - dirToObject * radius * 2;
				if ( TestMove( vTestPosition, size.z * 2, radius * 2, obj->GetAbsOrigin(), vMoveDir ) )
				{
					fwd = currentdir.Dot( -dirToObject );
					rt = rightdir.Dot( -dirToObject );
					foundResult = true;

					//Msg( "PUSH AWAY fwd=%f, rt=%f\n", fwd, rt );
				}
			}
		}

		if ( !foundResult )
		{
			// test if we can move through the object
			Vector vTestPosition = GetAbsOrigin() - vMoveDir * radius * 2;
			fwd = currentdir.Dot( -vMoveDir );
			rt = rightdir.Dot( -vMoveDir );

			if ( flDist < objectradius )
			{
				obj->AddEffects( EF_NODRAW );
			}

			//Msg( "PUSH THROUGH fwd=%f, rt=%f\n", fwd, rt );

			foundResult = true;
		}

		// If running, then do a lot more sideways veer since we're not going to do anything to
		//  forward velocity
		if ( istryingtomove )
		{
			sidescale = 6.0f;
		}

		if ( flVelProduct > 0.0f && flDirProduct > 0.0f )
		{
			sidescale = 0.1f;
		}

		float force = 1.0f;
		float forward = forwardscale * fwd * force * AVOID_SPEED;
		float side = sidescale * rt * force * AVOID_SPEED;

		adjustforwardmove	+= forward;
		adjustsidemove		+= side;
	}

	pCmd->forwardmove	+= adjustforwardmove;
	pCmd->sidemove		+= adjustsidemove;
	
	// Clamp the move to within legal limits, preserving direction. This is a little
	// complicated because we have different limits for forward, back, and side

	//Msg( "PRECLAMP: forwardmove=%f, sidemove=%f\n", pCmd->forwardmove, pCmd->sidemove );

	float flForwardScale = 1.0f;
	if ( pCmd->forwardmove > fabs( cl_forwardspeed.GetFloat() ) )
	{
		flForwardScale = fabs( cl_forwardspeed.GetFloat() ) / pCmd->forwardmove;
	}
	else if ( pCmd->forwardmove < -fabs( cl_backspeed.GetFloat() ) )
	{
		flForwardScale = fabs( cl_backspeed.GetFloat() ) / fabs( pCmd->forwardmove );
	}
	
	float flSideScale = 1.0f;
	if ( fabs( pCmd->sidemove ) > fabs( cl_sidespeed.GetFloat() ) )
	{
		flSideScale = fabs( cl_sidespeed.GetFloat() ) / fabs( pCmd->sidemove );
	}
	
	float flScale = MIN( flForwardScale, flSideScale );
	pCmd->forwardmove *= flScale;
	pCmd->sidemove *= flScale;

	//Msg( "POSTCLAMP: forwardmove=%f, sidemove=%f\n", pCmd->forwardmove, pCmd->sidemove );
}


void C_VectronicPlayer::PerformClientSideNPCSpeedModifiers( float flFrameTime, CUserCmd *pCmd )
{
	if ( m_hClosestNPC == NULL )
	{
		if ( m_flSpeedMod != cl_forwardspeed.GetFloat() )
		{
			float flDeltaTime = (m_flSpeedModTime - gpGlobals->curtime);
			m_flSpeedMod = RemapValClamped( flDeltaTime, cl_npc_speedmod_outtime.GetFloat(), 0, m_flExitSpeedMod, cl_forwardspeed.GetFloat() );
		}
	}
	else
	{
		C_AI_BaseNPC *pNPC = dynamic_cast< C_AI_BaseNPC *>( m_hClosestNPC.Get() );

		if ( pNPC )
		{
			float flDist = (GetAbsOrigin() - pNPC->GetAbsOrigin()).LengthSqr();
			bool bShouldModSpeed = false; 

			// Within range?
			if ( flDist < pNPC->GetSpeedModifyRadius() )
			{
				// Now, only slowdown if we're facing & running parallel to the target's movement
				// Facing check first (in 2D)
				Vector vecTargetOrigin = pNPC->GetAbsOrigin();
				Vector los = ( vecTargetOrigin - EyePosition() );
				los.z = 0;
				VectorNormalize( los );
				Vector facingDir;
				AngleVectors( GetAbsAngles(), &facingDir );
				float flDot = DotProduct( los, facingDir );
				if ( flDot > 0.8 )
				{
					/*
					// Velocity check (abort if the target isn't moving)
					Vector vecTargetVelocity;
					pNPC->EstimateAbsVelocity( vecTargetVelocity );
					float flSpeed = VectorNormalize(vecTargetVelocity);
					Vector vecMyVelocity = GetAbsVelocity();
					VectorNormalize(vecMyVelocity);
					if ( flSpeed > 1.0 )
					{
						// Velocity roughly parallel?
						if ( DotProduct(vecTargetVelocity,vecMyVelocity) > 0.4  )
						{
							bShouldModSpeed = true;
						}
					} 
					else
					{
						// NPC's not moving, slow down if we're moving at him
						//Msg("Dot: %.2f\n", DotProduct( los, vecMyVelocity ) );
						if ( DotProduct( los, vecMyVelocity ) > 0.8 )
						{
							bShouldModSpeed = true;
						} 
					}
					*/

					bShouldModSpeed = true;
				}
			}

			if ( !bShouldModSpeed )
			{
				m_hClosestNPC = NULL;
				m_flSpeedModTime = gpGlobals->curtime + cl_npc_speedmod_outtime.GetFloat();
				m_flExitSpeedMod = m_flSpeedMod;
				return;
			}
			else 
			{
				if ( m_flSpeedMod != pNPC->GetSpeedModifySpeed() )
				{
					float flDeltaTime = (m_flSpeedModTime - gpGlobals->curtime);
					m_flSpeedMod = RemapValClamped( flDeltaTime, cl_npc_speedmod_intime.GetFloat(), 0, cl_forwardspeed.GetFloat(), pNPC->GetSpeedModifySpeed() );
				}
			}
		}
	}

	if ( pCmd->forwardmove > 0.0f )
	{
		pCmd->forwardmove = clamp( pCmd->forwardmove, -m_flSpeedMod, m_flSpeedMod );
	}
	else
	{
		pCmd->forwardmove = clamp( pCmd->forwardmove, -m_flSpeedMod, m_flSpeedMod );
	}
	pCmd->sidemove = clamp( pCmd->sidemove, -m_flSpeedMod, m_flSpeedMod );
   
	//Msg( "fwd %f right %f\n", pCmd->forwardmove, pCmd->sidemove );
}


//-----------------------------------------------------------------------------
// Purpose: Input handling
//-----------------------------------------------------------------------------
bool C_VectronicPlayer::CreateMove( float flInputSampleTime, CUserCmd *pCmd )
{
	bool bResult = BaseClass::CreateMove( flInputSampleTime, pCmd );

	if ( !IsInAVehicle() )
	{
		PerformClientSideObstacleAvoidance( TICK_INTERVAL, pCmd );
		PerformClientSideNPCSpeedModifiers( TICK_INTERVAL, pCmd );
	}

	return bResult;
}

//-----------------------------------------------------------------------------
// Purpose: Input handling
//-----------------------------------------------------------------------------
void C_VectronicPlayer::BuildTransformations( CStudioHdr *hdr, Vector *pos, Quaternion q[], const matrix3x4_t& cameraTransform, int boneMask, CBoneBitList &boneComputed )
{
	BaseClass::BuildTransformations( hdr, pos, q, cameraTransform, boneMask, boneComputed );
#ifndef SWARM_DLL
	BuildFirstPersonMeathookTransformations( hdr, pos, q, cameraTransform, boneMask, boneComputed, "ValveBiped.Bip01_Head1" );
#endif
}


C_VectronicPlayer* C_VectronicPlayer::GetLocalPlayer( int nSlot )
{
	return To_VectronicPlayer( C_BasePlayer::GetLocalPlayer() );
}

/*
C_BaseSDKCombatWeapon*	C_VectronicPlayer::GetActiveSDKWeapon( void ) const
{
	C_BaseCombatWeapon *pWpn = GetActiveWeapon();

	if ( !pWpn )
		return NULL;

	return dynamic_cast< C_BaseSDKCombatWeapon* >( pWpn );
}
*/