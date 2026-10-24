#pragma once
#include "headers.h"

namespace nTSSG
{
    class cLotSkirt
    {
    public:
        // Not real methods belonging to this class
        const int *GetBounds() const;
        float SeaLevel() const;
        UINT GetRotation() const;
        int GetOffsetX() const;
        int GetOffsetY() const;
    };
}