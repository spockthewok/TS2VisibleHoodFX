#include "TS2/cTSSGSystem.h"
#include "common.h"

namespace nTSSG
{
    cLotSkirt *cTSSGSystem::LotSkirt()
    {
        using Func = cLotSkirt *(__thiscall *)(cTSSGSystem *);
        auto vFunc = Common::GetVTableFn<Func>(this, 0x94);
        return vFunc(this);
    }

    uintptr_t cTSSGSystem::Terrain()
    {
        using Func = uintptr_t(__thiscall *)(cTSSGSystem *);
        auto vFunc = Common::GetVTableFn<Func>(this, 0x9C);
        return vFunc(this);
    }
}