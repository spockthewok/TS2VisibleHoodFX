#include "TS2/cLotSkirt.h"

namespace nTSSG
{
    float cLotSkirt::SeaLevel()
    {
        return *reinterpret_cast<float *>(this + 0xB4);
    }

    UINT cLotSkirt::GetRotation()
    {
        return *reinterpret_cast<UINT *>(this + 0xEC);
    }

    int cLotSkirt::GetOffsetX()
    {
        return *reinterpret_cast<int *>(this + 0xF0);
    }

    int cLotSkirt::GetOffsetY()
    {
        return *reinterpret_cast<int *>(this + 0xF4);
    }
}