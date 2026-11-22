#include "common.h"
#include "TS2/base.h"
#include "TS2/cTSSGSystem.h"
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
            return 1280.0f; // All vanilla TS2 neighbourhoods are small SC4 maps of this size

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

    bool IsInLotBounds(const float x, const float y)
    {
        nTSWorld::cWorldDB *worldDB = TS::WorldDB();

        if (!worldDB)
            return false;

        float minX = worldDB->MinX();
        float minY = worldDB->MinY();
        float maxX = worldDB->MaxX();
        float maxY = worldDB->MaxY();

        return (x >= minX && x <= maxX) && (y >= minY && y <= maxY);
    }

    bool IsInLotBounds(cTSNHoodOccupant *occupant)
    {
        cVector coords = {0.0f, 0.0f, 0.0f};

        occupant->GetPosition(&coords);
        OffsetPositionToLot(&coords);

        return IsInLotBounds(coords.x, coords.y);
    }

    // This is a custom version of cLotSkirt::TranslateLotCoordsToNHoodCoords
    void TranslateEffectCoordsToNHoodCoords(float &x, float &y)
    {
        nTSSG::cTSSGSystem *system = *reinterpret_cast<nTSSG::cTSSGSystem **>(0x1478F10);
        nTSSG::cLotSkirt *lotSkirt = system->LotSkirt();

        if (!lotSkirt)
            return;

        const UINT rotation = lotSkirt->GetRotation();
        const float offsetX = static_cast<float>(lotSkirt->GetOffsetX() * 10);
        const float offsetY = static_cast<float>(lotSkirt->GetOffsetY() * 10);
        float currX = x, currY = y;

        switch (rotation)
        {
        case 1:
            currX = y;
            currY = -x;
            break;
        case 2:
            currX = -x;
            currY = -y;
            break;
        case 3:
            currX = -y;
            currY = x;
        }

        x = offsetX + currX;
        y = offsetY + currY;
    }
}