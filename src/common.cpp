#include "common.h"
#include "TS2/base.h"
#include "TS2/cTSNHoodTerrain.h"
#include <string>

namespace Common
{
    bool inLot = false;

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

    bool IsInLot()
    {
        inLot = false;

        cTSGameStateController *gameState = TS::GameStateController();
        cTSLotInfo *lotInfo = gameState->CurrentLotInfo();

        if (!lotInfo)
            return inLot;

        // Game's vars often haven't been updated when we want to check state
        // We instead use the current lot's name to determine state
        std::string lotName(*reinterpret_cast<const char **>(lotInfo + 0x1C));

        // All lot names follow this structure: "NHoodID-LotX", where X is lot number
        // CAS lots are either "CAS!" or "YACAS!", string will be blank if in neighbourhood
        if (!lotName.empty() && lotName != "CAS!" && lotName != "YACAS!")
            inLot = true;

        return inLot;
    }
}