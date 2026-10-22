#pragma once
#include "headers.h"
#include "TS2/cTSGlobals.h"
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

    using GetGameStateController = cTSGameStateController *(__cdecl *)();
    extern GetGameStateController GameStateController;
}

namespace nGZNumerics
{
    using GetEulerToQuaternion = void(__thiscall *)(float *, float *);
    extern GetEulerToQuaternion EulerToQuaternion;
}

extern const DWORD OffsetPositionToLot;