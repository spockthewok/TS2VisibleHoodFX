#include "TS2/cLotSkirt.h"

namespace nTSSG
{
    const int *cLotSkirt::GetBounds() const
    {
        return reinterpret_cast<const int *>(this + 0x2C);
    }

    float cLotSkirt::SeaLevel() const
    {
        return *reinterpret_cast<const float *>(this + 0xB4);
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