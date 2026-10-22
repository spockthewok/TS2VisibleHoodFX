#include "TS2/cTSLotInfo.h"
#include "common.h"

float cTSLotInfo::NHoodToLotHeightOffset()
{
    using Func = float(__thiscall *)(cTSLotInfo *);
    auto vFunc = Common::GetVTableFn<Func>(this, 0x40);
    return vFunc(this);
}