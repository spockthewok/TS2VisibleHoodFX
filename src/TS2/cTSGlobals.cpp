#include "TS2/cTSGlobals.h"
#include "common.h"

cTSNHoodTerrain *cTSGlobals::NeighborhoodTerrain()
{
    using Func = cTSNHoodTerrain *(__thiscall *)(cTSGlobals *);
    auto vFunc = Common::GetVTableFn<Func>(this, 0x2C);
    return vFunc(this);
}