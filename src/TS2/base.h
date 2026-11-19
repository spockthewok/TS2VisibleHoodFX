#pragma once
#include "headers.h"
#include "TS2/types.h"
#include "TS2/cTSGlobals.h"
#include "TS2/cWorldDB.h"
#include "TS2/cTSGameStateController.h"
#include "TS2/cTSNHoodOccupant.h"

namespace Module
{
    extern DWORD base;
}

namespace TS
{
    using GlobalsFn = cTSGlobals *(__cdecl *)();
    extern GlobalsFn Globals;

    using WorldDBFn = nTSWorld::cWorldDB *(__cdecl *)();
    extern WorldDBFn WorldDB;

    using GameStateControllerFn = cTSGameStateController *(__cdecl *)();
    extern GameStateControllerFn GameStateController;
}

using IsOccupantInBoundsFn = bool(__cdecl *)(cTSNHoodOccupant *, cRZRect *, bool);
extern IsOccupantInBoundsFn IsOccupantInBounds;

using OffsetPositionToLotFn = void(__cdecl *)(cVector *);
extern OffsetPositionToLotFn OffsetPositionToLot;

using lerpFn = void(__cdecl *)(cSlice *, float, float, float, float, float, float, float);
extern lerpFn lerp;