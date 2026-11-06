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

    void *cTSSGSystem::Terrain()
    {
        using Func = void *(__thiscall *)(cTSSGSystem *);
        auto vFunc = Common::GetVTableFn<Func>(this, 0x9C);
        return vFunc(this);
    }

    void *cTSSGSystem::OverlayManager(int renderType)
    {
        using Func = void *(__thiscall *)(cTSSGSystem *, int);
        auto vFunc = Common::GetVTableFn<Func>(this, 0xB4);
        return vFunc(this, renderType);
    }
}