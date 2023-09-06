//========= Copyright © 1996-2009, Valve Corporation, All rights reserved. ============//

#ifndef _IMATCHSYSTEM_H_
#define _IMATCHSYSTEM_H_

class IPlayerManager;
class IGameManager;
class IServerManager;
class ISearchManager;
class IMatchVoice;
class IDatacenter;

#ifdef P2_DLL
//class IDlcManager;
#include "matchmaking/idlcmanager.h"
#endif

class IMatchSystem
{
public:
	virtual IPlayerManager * GetPlayerManager() = 0;

	virtual IMatchVoice * GetMatchVoice() = 0;

	virtual IServerManager * GetUserGroupsServerManager() = 0;

	virtual ISearchManager * CreateGameSearchManager( KeyValues *pParams ) = 0;

	virtual IDatacenter * GetDatacenter() = 0;

#ifdef P2_DLL
	virtual IDlcManager* GetDlcManager() = 0;
#endif
};

#endif

