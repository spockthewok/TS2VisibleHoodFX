#pragma once
#include "TS2/base.h"

namespace nTSSG
{
    class cLotSkirt
    {
    public:
        // Not real methods belonging to this class
        const cRZRect *GetBounds() const;
        float SeaLevel() const;
        UINT GetRotation() const;
        int GetOffsetX() const;
        int GetOffsetY() const;
    };
}