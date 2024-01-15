#include "cbase.h"
#include "skeleton_player.h"

LINK_ENTITY_TO_CLASS( player, CSkeletonPlayer );

IMPLEMENT_SERVERCLASS_ST(CSkeletonPlayer,DT_SkeletonPlayer)
SendPropVector(SENDINFO(m_vecVelocity), -1, SPROP_NOSCALE, 0.0f, HIGH_DEFAULT),
END_SEND_TABLE()

void CSkeletonPlayer::Spawn()
{
	// Dying without a player model crashes the client
	SetModel("models/skeleton/player.mdl");

	BaseClass::Spawn();
}

void CSkeletonPlayer::CreateViewModel( int index )
{
	Assert( index >= 0 && index < MAX_VIEWMODELS );

	if ( GetViewModel(index) )
		return;

	CPredictedViewModel* vm = (CPredictedViewModel*)CreateEntityByName( "predicted_viewmodel" );
	if ( vm )
	{
		vm->SetAbsOrigin( GetAbsOrigin() );
		vm->SetOwner( this );
		vm->SetIndex( index );
		DispatchSpawn( vm );
		vm->FollowEntity( this, false );
		m_hViewModel.Set( index, vm );
	}
}

void CSkeletonPlayer::PostThink()
{
	BaseClass::PostThink();

	// Keep the model upright; pose params will handle pitch aiming.
	QAngle angles = GetLocalAngles();
	angles[PITCH] = 0;
	SetLocalAngles(angles);
}

#include "..\server\ilagcompensationmanager.h"

void CSkeletonPlayer::FireBullets ( const FireBulletsInfo_t &info )
{
	lagcompensation->StartLagCompensation( this, LAG_COMPENSATE_HITBOXES);

	BaseClass::FireBullets(info);

	lagcompensation->FinishLagCompensation( this );
}