#include "TS2/base.h"
#include <cmath>

namespace Module
{
    DWORD base = 0x400000; // Default module base address
}

namespace Floats
{
    // All vanilla TS2 neighbourhoods are small SC4 maps of this size
    const float nhoodSize = 1280.0f;
}

namespace nGZNumerics
{
    void Normalize(cSlice *normal)
    {
        float invLen = 1.0f / std::sqrt((normal->z * normal->z) +
                                        (normal->y * normal->y) +
                                        (normal->x * normal->x));

        normal->x *= invLen;
        normal->y *= invLen;
        normal->z *= invLen;
    }
}

// Base addresses of various TS2 methods
namespace TS
{
    GlobalsFn Globals = reinterpret_cast<GlobalsFn>(0x799A0D);
    WorldDBFn WorldDB = reinterpret_cast<WorldDBFn>(0x799AB9);
    GameStateControllerFn GameStateController = reinterpret_cast<GameStateControllerFn>(0x799B65);
}

IsOccupantInBoundsFn IsOccupantInBounds = reinterpret_cast<IsOccupantInBoundsFn>(0xAD0280);
OffsetPositionToLotFn OffsetPositionToLot = reinterpret_cast<OffsetPositionToLotFn>(0xAD0460);
lerpFn lerp = reinterpret_cast<lerpFn>(0xB7EFB0);