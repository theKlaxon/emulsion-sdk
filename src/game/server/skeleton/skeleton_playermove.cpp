#include "cbase.h"
#include "player_command.h"
#include "igamemovement.h"
#include "ipredictionsystem.h"

static CMoveData g_MoveData;
CMoveData* g_pMoveData = &g_MoveData;

IPredictionSystem* IPredictionSystem::g_pPredictionSystems = NULL;

static CPlayerMove g_PlayerMove;

CPlayerMove* PlayerMove() { return &g_PlayerMove; }