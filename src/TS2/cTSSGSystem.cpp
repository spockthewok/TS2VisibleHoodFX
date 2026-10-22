#include "TS2/cTSSGSystem.h"

namespace nTSSG
{
    cLotSkirt *cTSSGSystem::LotSkirt()
    {
        using Func = cLotSkirt *(__thiscall *)(cTSSGSystem *);
        auto vFunc = Common::GetVTableFn<Func>(this, 0x94);
        return vFunc(this);
    }
}