#pragma once
#include "headers.h"
#include "TS2/cTSGlobals.h"
#include "TS2/cWorldDB.h"
#include "TS2/cTSGameStateController.h"
#include "TS2/cTSNHoodOccupant.h"

struct cVector
{
    float x, y, z;
};

typedef cVector cSlice;

struct cQuaternion
{
    float x, y, z, w;
};

struct cRZRect
{
    int minX, minY, maxX, maxY;
};

namespace Module
{
    extern DWORD base;
}

namespace Floats
{
    extern const float nhoodSize;
}

namespace nGZNumerics
{
    void Normalize(cSlice *normal);
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