#pragma once
#include "TS2/cLotSkirt.h"

namespace nTSSG
{
    class cTSSGSystem
    {
    public:
        cLotSkirt *LotSkirt();
        // Actual return type is cTerrain object
        // We don't use any cTerrain methods, so no point creating skeleton class
        uintptr_t Terrain();
    };
}