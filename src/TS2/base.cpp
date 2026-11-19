#include "TS2/base.h"

namespace Module
{
    DWORD base = 0x400000; // Default module base address
}

// Base addresses of various TS2 methods
namespace TS
{
    GlobalsFn Globals = reinterpret_cast<GlobalsFn>(0x799A0D);
    WorldDBFn WorldDB = reinterpret_cast<WorldDBFn>(0x799AB9);
    GameStateControllerFn GameStateController = reinterpret_cast<GameStateControllerFn>(0x799B65);
}

// These belong to anonymous namespaces (likely private methods)
IsOccupantInBoundsFn IsOccupantInBounds = reinterpret_cast<IsOccupantInBoundsFn>(0xAD0280);
OffsetPositionToLotFn OffsetPositionToLot = reinterpret_cast<OffsetPositionToLotFn>(0xAD0460);
lerpFn lerp = reinterpret_cast<lerpFn>(0xB7EFB0);