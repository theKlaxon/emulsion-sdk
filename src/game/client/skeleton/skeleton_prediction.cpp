#include "cbase.h"
#include "prediction.h"
#include "igamemovement.h"

static CMoveData g_MoveData;
CMoveData *g_pMoveData = &g_MoveData;

static CPrediction g_Prediction;

EXPOSE_SINGLE_INTERFACE_GLOBALVAR( CPrediction, IPrediction, VCLIENT_PREDICTION_INTERFACE_VERSION, g_Prediction );

CPrediction* prediction = &g_Prediction;