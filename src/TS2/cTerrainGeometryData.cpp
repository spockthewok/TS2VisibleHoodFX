#include "TS2/cTerrainGeometryData.h"
#include "common.h"
#include "TS2/cNHoodTerrain.h"
#include <cmath>

// These methods are never actually called without nhood effects activated in lot view
// Safe to replace them to use nhood data without affecting standard lot effects
// Need our own as nhood equivalent object is destroyed in lot view, so can't use its methods
namespace nTSSG
{
    float cTerrainGeometryData::Height(float x, float y)
    {
        cTSGlobals *globals = TS::Globals();
        cTSGameStateController *gameState = TS::GameStateController();
        cTSNHoodTerrain *nhoodTerrain = globals->NeighborhoodTerrain();
        cTSLotInfo *lotInfo = gameState->CurrentLotInfo();

        if (!nhoodTerrain || !lotInfo)
            return Floats::nhoodSize; // Return large value so later checks fail

        std::array<float, 2> coords = {x, y};

        Common::TranslateEffectCoordsToNHoodCoords(coords);

        float terrainHeight = nhoodTerrain->GetTerrainHeight(coords[0], coords[1]);
        float offsetZ = lotInfo->NHoodToLotHeightOffset();

        if (std::isnan(terrainHeight) || std::isinf(terrainHeight) || terrainHeight == 0)
            return Floats::nhoodSize;

        // Game will subtract water level from this value
        // Because lot water level is negative, calculation becomes height + lotWaterLvl
        // Equivalent to terrainHeight - nhoodWaterLvl, which is what nhood uses to get height
        return terrainHeight - offsetZ;
    }

    // This is a rough recreation of cNHoodTerrain::Normal
    // Ghidra thinks this is a void method, but its callers try to dereference EAX after it returns
    cSlice *cTerrainGeometryData::Normal(cSlice *normal, float x, float y)
    {
        std::array<float, 2> coords = {x, y};

        Common::TranslateEffectCoordsToNHoodCoords(coords);

        float gridScale = 10.0f; // Value taken from cNHoodTerrain object var
        float currX = coords[0] / gridScale;
        float currY = coords[1] / gridScale;
        // Game does slightly more involved floating-point maths for gridX/gridY conversion
        // Seems to just round towards 0, which a direct cast already does
        int gridX = static_cast<int>(currX);
        int gridY = static_cast<int>(currY);
        int gridWidth = static_cast<int>(Common::GetNHoodSizeX() / gridScale) + 1;
        float tX = currX - static_cast<float>(gridX);
        float tY = currY - static_cast<float>(gridY);

        // I'm assuming the coord system is the same as an array, where (0,0) is top left
        // (0,0) would be bottom left with conventional bilinear interpolation
        cSlice &n00 = nhoodTerrain.normals[gridX + (gridY * gridWidth)];
        cSlice &n10 = nhoodTerrain.normals[(gridX + 1) + (gridY * gridWidth)];
        cSlice &n01 = nhoodTerrain.normals[gridX + ((gridY + 1) * gridWidth)];
        cSlice &n11 = nhoodTerrain.normals[(gridX + 1) + ((gridY + 1) * gridWidth)];

        cSlice top, bottom;

        lerp(&top, n00.x, n00.y, n00.z, n10.x, n10.y, n10.z, tX);
        lerp(&bottom, n01.x, n01.y, n01.z, n11.x, n11.y, n11.z, tX);
        lerp(normal, top.x, top.y, top.z, bottom.x, bottom.y, bottom.z, tY);

        nGZNumerics::Normalize(normal);

        return normal;
    }
}