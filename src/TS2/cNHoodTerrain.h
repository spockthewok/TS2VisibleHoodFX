#pragma once
#include "TS2/base.h"
#include <vector>

namespace nTSSG
{
    struct cNHoodTerrain
    {
        std::vector<cVector> heightMap;
        std::vector<cSlice> normals;
    };

    extern cNHoodTerrain nhoodTerrain;
}