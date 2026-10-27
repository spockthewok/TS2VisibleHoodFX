#include "TS2/base.h"

namespace Module
{
    DWORD base = 0x400000; // Default module base address
}

namespace Floats
{
    // All vanilla TS2 neighbourhoods are small SC4 maps of this size
    const float nhoodSize = 1280.0f;
}

// Base addresses of various TS2 methods
namespace TS
{
    GetGlobals Globals = reinterpret_cast<GetGlobals>(0x799A0D);
    GetWorldDB WorldDB = reinterpret_cast<GetWorldDB>(0x799AB9);
    GetGameStateController GameStateController = reinterpret_cast<GetGameStateController>(0x799B65);
}

GetIsOccupantInBounds IsOccupantInBounds = reinterpret_cast<GetIsOccupantInBounds>(0xAD0280);
DoOffsetPositionToLot OffsetPositionToLot = reinterpret_cast<DoOffsetPositionToLot>(0xAD0460);