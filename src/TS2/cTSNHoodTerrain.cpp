#include "TS2/cTSNHoodTerrain.h"
#include "common.h"

float cTSNHoodTerrain::GetTerrainHeight(float x, float y)
{
    using Func = float(__thiscall *)(cTSNHoodTerrain *, float, float);
    auto vFunc = Common::GetVTableFn<Func>(this, 0x2C);
    return vFunc(this, x, y);
}