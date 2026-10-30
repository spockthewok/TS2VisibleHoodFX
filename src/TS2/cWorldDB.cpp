#include "TS2/cWorldDB.h"
#include "common.h"

namespace nTSWorld
{
    float cWorldDB::MinX()
    {
        using Func = float(__thiscall *)(cWorldDB *);
        auto vFunc = Common::GetVTableFn<Func>(this, 0x12C);
        return vFunc(this);
    }

    float cWorldDB::MinY()
    {
        using Func = float(__thiscall *)(cWorldDB *);
        auto vFunc = Common::GetVTableFn<Func>(this, 0x130);
        return vFunc(this);
    }

    float cWorldDB::MaxX()
    {
        using Func = float(__thiscall *)(cWorldDB *);
        auto vFunc = Common::GetVTableFn<Func>(this, 0x134);
        return vFunc(this);
    }

    float cWorldDB::MaxY()
    {
        using Func = float(__thiscall *)(cWorldDB *);
        auto vFunc = Common::GetVTableFn<Func>(this, 0x138);
        return vFunc(this);
    }
}