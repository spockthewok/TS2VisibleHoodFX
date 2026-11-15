#pragma once
#include "TS2/base.h"

namespace nTSSG
{
    class cTerrainGeometryData
    {
    public:
        float Height(float x, float y);
        cSlice *Normal(cSlice *normal, float x, float y);
    };
}