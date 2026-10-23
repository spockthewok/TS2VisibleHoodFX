#pragma once
#include "TS2/cTSGlobals.h"
#include "TS2/cWorldDB.h"
#include "TS2/cTSGameStateController.h"
#include "TS2/cLotSkirt.h"

namespace Floats
{
    extern const float nhoodSize;
}

namespace TS
{
    using GetGlobals = cTSGlobals *(__cdecl *)();
    extern GetGlobals Globals;

    using GetWorldDB = nTSWorld::cWorldDB *(__cdecl *)();
    extern GetWorldDB WorldDB;

    using GetGameStateController = cTSGameStateController *(__cdecl *)();
    extern GetGameStateController GameStateController;
}

using DoOffsetPositionToLot = void(__cdecl *)(float *);
extern DoOffsetPositionToLot OffsetPositionToLot;