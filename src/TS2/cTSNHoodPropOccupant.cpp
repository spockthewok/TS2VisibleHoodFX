#include "TS2/cTSNHoodPropOccupant.h"
#include "common.h"

void cTSNHoodPropOccupant::GetPosition(float *coords)
{
    using Func = void(__thiscall *)(cTSNHoodPropOccupant *, float *);
    auto vFunc = Common::GetVTableFn<Func>(this, 0x18);
    vFunc(this, coords);
}