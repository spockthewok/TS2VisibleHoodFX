#pragma once
#include "headers.h"

namespace nTSSG
{
    class cLotSkirt
    {
    public:
        void TranslateLotCoordsToNhoodCoords(float x, float y, float *newX, float *newY);
    };
}