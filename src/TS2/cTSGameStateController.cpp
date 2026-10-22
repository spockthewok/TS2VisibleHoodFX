#include "TS2/cTSGameStateController.h"
#include "TS2/cTSLotInfo.h"
#include "common.h"

cTSLotInfo *cTSGameStateController::CurrentLotInfo()
{
    using Func = cTSLotInfo *(__thiscall *)(cTSGameStateController *);
    auto vFunc = Common::GetVTableFn<Func>(this, 0x24);
    return vFunc(this);
}