#include "TS2/cTSNHoodOccupant.h"
#include "common.h"

void cTSNHoodOccupant::GetPosition(cVector *coords)
{
    using Func = void(__thiscall *)(cTSNHoodOccupant *, cVector *);
    auto vFunc = Common::GetVTableFn<Func>(this, 0x18);
    vFunc(this, coords);
}