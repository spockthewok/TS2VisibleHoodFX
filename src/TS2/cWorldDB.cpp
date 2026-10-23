#include "TS2/cWorldDB.h"
#include "common.h"

namespace nTSWorld
{
    float cWorldDB::LotSizeX()
    {
        using Func = float(__thiscall *)(cWorldDB *);
        auto vFunc = Common::GetVTableFn<Func>(this, 0x13C);
        return vFunc(this);
    }

    float cWorldDB::LotSizeY()
    {
        using Func = float(__thiscall *)(cWorldDB *);
        auto vFunc = Common::GetVTableFn<Func>(this, 0x140);
        return vFunc(this);
    }
}