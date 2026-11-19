#pragma once
#include "TS2/types.h"
#include <vector>

namespace nTSSG
{
    // This is obviously really a class in the game's code
    // We can't access the object in lot view, so we're just using our own basic struct
    struct cNHoodTerrain
    {
        float gridScale;
        int gridWidth;
        std::vector<cVector> heightMap;
        std::vector<cSlice> normals;
    };

    extern cNHoodTerrain nhoodTerrain;
}