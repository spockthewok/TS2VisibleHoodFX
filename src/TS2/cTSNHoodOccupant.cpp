#include "TS2/cTSNHoodOccupant.h"
#include "common.h"

void cTSNHoodOccupant::GetPosition(float *coords)
{
    using Func = void(__thiscall *)(cTSNHoodOccupant *, float *);
    auto vFunc = Common::GetVTableFn<Func>(this, 0x18);
    vFunc(this, coords);
}