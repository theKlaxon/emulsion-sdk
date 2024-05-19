#include "cbase.h"
#include "../EventLog.h"

CEventLog g_EventLog;

#ifndef SWARM_DLL
IGameSystem* GameLogSystem()
{
	return &g_EventLog;
}
#else
CEventLog* GameLogSystem()
{
	return &g_EventLog;
}
#endif