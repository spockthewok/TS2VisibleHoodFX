#include "TS2/cLotSkirt.h"

namespace nTSSG
{
    const cRZRect *cLotSkirt::GetBounds() const
    {
        return reinterpret_cast<const cRZRect *>(this + 0x2C);
    }

    UINT cLotSkirt::GetRotation() const
    {
        return *reinterpret_cast<const UINT *>(this + 0xEC);
    }

    int cLotSkirt::GetOffsetX() const
    {
        return *reinterpret_cast<const int *>(this + 0xF0);
    }

    int cLotSkirt::GetOffsetY() const
    {
        return *reinterpret_cast<const int *>(this + 0xF4);
    }
}