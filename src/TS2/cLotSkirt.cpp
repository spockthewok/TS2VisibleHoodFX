#include "TS2/cLotSkirt.h"

namespace nTSSG
{
    void cLotSkirt::TranslateLotCoordsToNhoodCoords(float x, float y, float *newX, float *newY)
    {
        using Func = void(__thiscall *)(cLotSkirt *, float, float, float *, float *);
        auto func = reinterpret_cast<Func>(0xA7DD90);
        func(this, x, y, newX, newY);
    }
}