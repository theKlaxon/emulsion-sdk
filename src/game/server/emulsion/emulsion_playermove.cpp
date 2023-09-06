#include "cbase.h"
#include "player_command.h"
#include "igamemovement.h"
#include "ipredictionsystem.h"
#include "vehicle_base.h"
#include "gamestats.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

// basically re-used sdk playermove
class CEmulsionPlayerMove : public CPlayerMove {
	DECLARE_CLASS(CEmulsionPlayerMove, CPlayerMove);
public:

};

static CEmulsionPlayerMove g_PlayerMove;
static CMoveData g_MoveData;

IPredictionSystem* IPredictionSystem::g_pPredictionSystems = NULL;
CPlayerMove* PlayerMove() { return &g_PlayerMove; }
CMoveData* g_pMoveData = &g_MoveData;
