#include "TS2/base.h"

namespace Floats
{
    // All TS2 neighbourhoods are small SC4 maps of this size
    const float nhoodSize = 1280.0f;
}

// Base addresses of various TS2 methods
namespace TS
{
    GetGlobals Globals = reinterpret_cast<GetGlobals>(0x799A0D);
    GetGameStateController GameStateController = reinterpret_cast<GetGameStateController>(0x799B65);
}

namespace nGZNumerics
{
    GetEulerToQuaternion EulerToQuaternion = reinterpret_cast<GetEulerToQuaternion>(0xEBDEB0);
}

const DWORD OffsetPositionToLot = 0xAD0460;