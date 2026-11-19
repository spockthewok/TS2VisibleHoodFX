#pragma once
#include "TS2/types.h"

namespace nTSSG
{
    class cTerrainGeometryData
    {
    public:
        float Height(float x, float y);
        cSlice *Normal(cSlice *normal, float x, float y);
        float WaterLevel();
    };
}