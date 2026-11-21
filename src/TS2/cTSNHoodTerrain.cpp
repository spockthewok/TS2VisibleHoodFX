#include "TS2/cTSNHoodTerrain.h"
#include "common.h"

float cTSNHoodTerrain::SeaLevel()
{
    using Func = float(__thiscall *)(cTSNHoodTerrain *);
    auto vFunc = Common::GetVTableFn<Func>(this, 0x40);
    return vFunc(this);
}