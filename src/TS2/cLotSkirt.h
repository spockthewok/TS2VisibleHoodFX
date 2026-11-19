#pragma once
#include "headers.h"
#include "TS2/types.h"

namespace nTSSG
{
    class cLotSkirt
    {
    public:
        // Not real methods belonging to this class
        const cRZRect *GetBounds() const;
        UINT GetRotation() const;
        int GetOffsetX() const;
        int GetOffsetY() const;
    };
}