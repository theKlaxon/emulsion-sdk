#include "cbase.h"
#include "skeleton/multiplayer/basenetworkedplayer_gamemove.h"

//static CNetworkedPlayerMovement g_GameMovement;
//IGameMovement* g_pGameMovement = (IGameMovement*)&g_GameMovement;
//
//EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CNetworkedPlayerMovement, IGameMovement,INTERFACENAME_GAMEMOVEMENT, g_GameMovement );


#ifdef CLIENT_DLL
#include "skeleton/multiplayer/basenetworkedplayer_cl.h"
#else
#include "skeleton/multiplayer/basenetworkedplayer.h"
#endif

bool CNetworkedPlayerMovement::CheckJumpButton()
{
	bool HasJumped = BaseClass::CheckJumpButton();

	if (HasJumped)
		static_cast<CBaseNetworkedPlayer*>(player)->DoAnimationEvent(PLAYERANIMEVENT_JUMP);

	return HasJumped;
}