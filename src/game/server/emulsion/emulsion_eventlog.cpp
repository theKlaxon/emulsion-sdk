#include "cbase.h"
#include "../EventLog.h"

CEventLog g_EventLog;

CEventLog* GameLogSystem()
{
	return &g_EventLog;
}
