#include "common.h"
#include "TS2/base.h"
#include "TS2/cTSNHoodTerrain.h"

namespace Common
{
    static float GetNHoodSize(const BYTE offset)
    {
        cTSGlobals *globals = TS::Globals();
        cTSNHoodTerrain *nhoodTerrain = globals->NeighborhoodTerrain();

        if (!nhoodTerrain)
            return Floats::nhoodSize;

        return *reinterpret_cast<float *>(nhoodTerrain + offset);
    }

    float GetNHoodSizeX()
    {
        return GetNHoodSize(0x24);
    }

    float GetNHoodSizeY()
    {
        return GetNHoodSize(0x28);
    }
}