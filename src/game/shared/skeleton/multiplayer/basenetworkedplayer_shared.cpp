#include "cbase.h"
#ifdef GAME_DLL
#include "skeleton/multiplayer/basenetworkedplayer.h"
#else
#include "skeleton/multiplayer/basenetworkedplayer_cl.h"
#endif

void CBaseNetworkedPlayer::MakeAnimState()
{
#ifdef CLIENT_DLL
	MDLCACHE_CRITICAL_SECTION();
#endif
	MultiPlayerMovementData_t mv;
	mv.m_flBodyYawRate = 360;
	mv.m_flRunSpeed = 320;
	mv.m_flWalkSpeed = 75;
	mv.m_flSprintSpeed = -1.0f;
	m_PlayerAnimState = new CMultiPlayerAnimState( this,mv );
}